#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>
#include <string.h>

#define FILENAME "fuel_monitoring_data.txt"  // Файл для хранения данных
#define MAX_SPEEDS 100                        // Максимальное количество скоростей

// Плотности для различных типов топлива
#define DENSITY_GASOLINE 0.725
#define DENSITY_DIESEL 0.825

// Структура для хранения данных о мониторинге топлива
typedef struct {
    double fuel_level;           // Уровень топлива в литрах
    double specific_consumption; // Удельный расход топлива в г/кВт·ч
    double engine_power;         // Мощность двигателя в кВт
    double fuel_density;         // Плотность топлива (бензин или дизель)
    int formula_choice;          // Выбор формулы для расчета расхода топлива
} FuelMonitoring;

// Глобальные переменные
FuelMonitoring current_monitoring; // Текущие данные о мониторинге
double speeds[MAX_SPEEDS];          // Массив для хранения скоростей
int speed_count = 0;                // Количество скоростей
int record_number = 0;              // Номер записи

// Объявление функций
int import_data();                  // Импорт данных из файла
int manual_input_data();            // Ввод данных вручную
void start_monitoring();            // Начало мониторинга
int save_record();                  // Сохранение записи в файл
int display_all_records();          // Отображение всех записей из файла
int clear_all_records();            // Очистка всех записей из файла
void user_interface();              // Пользовательский интерфейс

int main() {
    setlocale(LC_ALL, "RUS");       // Установка локали для русского языка
    system("chcp 1251");            // Установка кодировки консоли

    printf("****************************************\n");
    printf("*         Программа анализа уровня      *\n");
    printf("*              топлива                  *\n");
    printf("****************************************\n");

    user_interface();                // Запуск пользовательского интерфейса

    return 0;
}

// Функция для импорта данных из файла
int import_data() {
    FILE* file = fopen(FILENAME, "r");  // Открытие файла для чтения

    if (file == NULL) {
        return -1;                       // Ошибка открытия файла
    }

    int fuel_type;

    // Считываем тип топлива (int)
    if (fscanf(file, "%d", &fuel_type) != 1) {
        fclose(file);
        return -1;                       // Ошибка чтения типа топлива
    }

    if (fuel_type != 1 && fuel_type != 2) {
        fclose(file);
        return -1;                       // Неверный тип топлива
    }

    current_monitoring.fuel_density = (fuel_type == 1) ? DENSITY_GASOLINE : DENSITY_DIESEL;

    // Считываем уровень топлива (double)
    if (fscanf(file, "%lf", &current_monitoring.fuel_level) != 1) {
        fclose(file);
        return -1;                       // Ошибка чтения уровня топлива
    }

    // Считываем удельный расход топлива (double)
    if (fscanf(file, "%lf", &current_monitoring.specific_consumption) != 1) {
        fclose(file);
        return -1;                       // Ошибка чтения удельного расхода топлива
    }

    // Считываем мощность двигателя (double)
    if (fscanf(file, "%lf", &current_monitoring.engine_power) != 1) {
        fclose(file);
        return -1;                       // Ошибка чтения мощности двигателя
    }

    // Считываем выбор формулы (int)
    if (fscanf(file, "%d", &current_monitoring.formula_choice) != 1 ||
        current_monitoring.formula_choice < 1 || current_monitoring.formula_choice > 3) {
        fclose(file);
        return -1;                       // Ошибка чтения выбора формулы или неверное значение
    }

    // Считываем скорости (double)
    speed_count = 0;
    while (fscanf(file, "%lf", &speeds[speed_count]) == 1 && speed_count < MAX_SPEEDS) {
        speed_count++;                   // Увеличиваем счетчик скоростей
    }

    fclose(file);

    if (speed_count == 0) {
        return -1;                       // Нет данных о скоростях
    }

    return 0;                           // Успешный импорт данных
}

// Функция для ручного ввода данных пользователем
int manual_input_data() {
    int fuel_type;

    printf("Введите тип топлива:\n");
    printf("1. Бензин\n");
    printf("2. Дизель\n");

    scanf("%d", &fuel_type);

    if (fuel_type != 1 && fuel_type != 2) {
        return -1;                       // Неверный ввод типа топлива
    }

    current_monitoring.fuel_density = (fuel_type == 1) ? DENSITY_GASOLINE : DENSITY_DIESEL;

    printf("Введите уровень топлива в баке (литры): ");
    scanf("%lf", &current_monitoring.fuel_level);

    printf("Введите удельный расход топлива (г/кВт·ч): ");
    scanf("%lf", &current_monitoring.specific_consumption);

    printf("Введите мощность двигателя (кВт): ");
    scanf("%lf", &current_monitoring.engine_power);

    printf("Выберите формулу для расчета расхода топлива:\n");
    printf("1. Qs = qe * Ne * 100 / (103 * v * yt)\n");
    printf("2. P100 = 80/V + 5.7 + (0.000002 * V^3)\n");
    printf("3. Q = P * k / v\n");

    scanf("%d", &current_monitoring.formula_choice);

    if (current_monitoring.formula_choice < 1 || current_monitoring.formula_choice > 3) {
        return -1;                       // Неверный ввод формулы
    }

    speed_count = 0;
    double speed;
    while (speed_count < MAX_SPEEDS) {
        printf("Введите скорость #%d в км/ч (или отрицательное число для завершения ввода): ", speed_count + 1);
        scanf("%lf", &speed);

        if (speed < 0) break;            // Завершаем ввод скоростей

        speeds[speed_count++] = speed;   // Сохраняем скорость в массиве
    }

    if (speed_count == 0) {
        return -1;                       // Не введено ни одной скорости
    }

    return 0;                           // Успешный ввод данных вручную
}

// Функция для начала мониторинга и расчета расхода топлива на основе скоростей
void start_monitoring() {
    if (speed_count == 0) {
        return;                          // Нет данных о скоростях для мониторинга
    }

    for (int i = 0; i < speed_count; i++) {
        double fuel_used = 0;

        switch (current_monitoring.formula_choice) {
        case 1:
            fuel_used +=
                ((current_monitoring.specific_consumption /
                    current_monitoring.engine_power));
            break;
        case 2:
            fuel_used += ((80.0 / speeds[i]) +
                ((5.7 + ((0.000002 * pow(speeds[i], 3)))))) * speeds[i] / 100;
            break;
        case 3:
            fuel_used +=
                ((current_monitoring.engine_power *
                    .01) / speeds[i]);
            break;
        default:
            return;                    // Неверная формула для расчета.
        }

        current_monitoring.fuel_level -= fuel_used;

        if (current_monitoring.fuel_level <= 0) {
            current_monitoring.fuel_level = 0;
            break;                        // Завершение расчета при исчерпании топлива.
        }

        printf("На скорости %.2f км/ч использовано %.2f л топлива.\n",
            speeds[i], fuel_used);
    }

    printf("\nМониторинг завершен. Остаток топлива: %.2f л.\n",
        current_monitoring.fuel_level);
}

// Функция для сохранения текущих данных о мониторинге в файл.
int save_record() {
    FILE* file = fopen(FILENAME, "a");

    if (file == NULL) {
        return -1;                     // Ошибка открытия файла.
    }

    record_number++;

    fprintf(file, "Запись #%d: Уровень топлива: %.2f л, Удельный расход: %.2f г/кВт·ч, Мощность: %.2f кВт, Плотность: %.2f кг/л, Выбор формулы: %d\n",
        record_number, current_monitoring.fuel_level, current_monitoring.specific_consumption, current_monitoring.engine_power, current_monitoring.fuel_density, current_monitoring.formula_choice);

    fclose(file);

    return 0;                           // Успешное сохранение записи.
}

// Функция для отображения всех записей из файла.
int display_all_records() {
    FILE* file = fopen(FILENAME, "r");

    if (file == NULL) {
        return -1;                     // Ошибка открытия файла.
    }

    char line[256];
    int line_number = 1;

    while (fgets(line, sizeof(line), file)) {
        printf("%d. %s", line_number++, line);
    }

    fclose(file);

    return 0;                           // Успешное отображение записей.
}

// Функция для очистки всех записей из файла.
int clear_all_records() {
    FILE* file = fopen(FILENAME, "w");

    if (file == NULL) { return-100; }     // Ошибка открытия файла.

}

// Функция пользовательского интерфейса.
void user_interface() {
    int choice;

    while (1) {
        printf("\nМеню:\n");
        printf("1. Импортировать данные из файла\n");
        printf("2. Ввести данные вручную\n");
        printf("3. Начать мониторинг\n");
        printf("4. Сохранить запись мониторинга\n");
        printf("5. Показать все записи о мониторинге\n");
        printf("6. Очистить все мониторинги\n");
        printf("7. Выход\n");
        printf("Выберите действие: ");

        scanf("%d", &choice);

        switch (choice) {
        case 1:
            import_data();
            break;
        case 2:
            manual_input_data();
            break;
        case 3:
            start_monitoring();
            break;
        case 4:
            save_record();
            break;
        case 5:
            display_all_records();
            break;
        case 6:
            clear_all_records();
            break;
        case 7:
            exit(0);
        default:
            continue;
        }
    }
}
