/*
===============================================================================
    EXPENSE TRACKER APPLICATION
    Console-Based Financial Management System (ANSI C)
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- CONFIG ---------------- */
#define MAX_RECORDS 1000
#define MAX_CATEGORY 30
#define MAX_DESC 100
#define DATE_STR 11
#define DATA_FILE "expenses.dat"
#define CLEAR system("clear || cls")
#define HR "--------------------------------------------------------------------------\n"

/* ---------------- STRUCTURES ---------------- */
typedef enum {
    INCOME,
    EXPENSE
} TransactionType;

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int id;
    Date date;
    float amount;
    TransactionType type;
    char category[MAX_CATEGORY];
    char description[MAX_DESC];
} Record;

/* ---------------- GLOBALS ---------------- */
Record records[MAX_RECORDS];
int recordCount = 0;
int nextId = 1;

/* ---------------- PROTOTYPES ---------------- */
void showMenu();
void addIncome();
void addExpense();
void viewAllRecords();
void monthlySummary();
void categorySummary();
void deleteRecord();
void saveToFile();
void loadFromFile();
void pauseScreen();
void clearBuffer();

int getSmartDate(Date *d);
int validateDate(Date d);

float totalIncome();
float totalExpense();

/* ---------------- MAIN ---------------- */
int main() {
    int choice;
    loadFromFile();

    while (1) {
        CLEAR;
        showMenu();

        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: addIncome(); break;
            case 2: addExpense(); break;
            case 3: viewAllRecords(); break;
            case 4: monthlySummary(); break;
            case 5: categorySummary(); break;
            case 6: deleteRecord(); break;
            case 7:
                saveToFile();
                printf("\nData saved successfully. Goodbye.\n");
                return 0;
            default:
                printf("\nInvalid choice. Try again.\n");
                pauseScreen();
        }
    }
}

/* ---------------- MENU ---------------- */
void showMenu() {
    float balance = totalIncome() - totalExpense();

    printf(HR);
    printf("EXPENSE TRACKER MENU\n");
    printf(HR);

    printf("1. Add Income\n");
    printf("2. Add Expense\n");
    printf("3. View All Records\n");
    printf("4. Monthly Summary\n");
    printf("5. Category Summary\n");
    printf("6. Delete Record\n");
    printf("7. Exit\n");

    printf(HR);
    printf("Records : %d\n", recordCount);
    printf("Balance : ₹%.2f\n", balance);
    printf(HR);
}

/* ---------------- SMART DATE INPUT ---------------- */
int getSmartDate(Date *d) {
    char input[20];
    int dd, mm, yy;

    printf("Enter date (DD/MM/YYYY or DDMMYYYY): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    if (sscanf(input, "%d/%d/%d", &dd, &mm, &yy) == 3 ||
        sscanf(input, "%d %d %d", &dd, &mm, &yy) == 3) {
        d->day = dd;
        d->month = mm;
        d->year = yy;
    } else if (strlen(input) == 8 && sscanf(input, "%d", &dd) == 1) {
        d->day = dd / 1000000;
        d->month = (dd / 10000) % 100;
        d->year = dd % 10000;
    } else {
        return 0;
    }

    return validateDate(*d);
}

/* ---------------- ADD INCOME ---------------- */
void addIncome() {
    Record r;
    CLEAR;

    printf(HR "ADD INCOME\n" HR);

    r.id = nextId++;
    r.type = INCOME;

    if (!getSmartDate(&r.date)) {
        printf("\nInvalid date.\n");
        nextId--;
        pauseScreen();
        return;
    }

    printf("Amount: ₹");
    if (scanf("%f", &r.amount) != 1 || r.amount <= 0) {
        clearBuffer();
        printf("\nInvalid amount.\n");
        nextId--;
        pauseScreen();
        return;
    }
    clearBuffer();

    printf("Category: ");
    fgets(r.category, MAX_CATEGORY, stdin);
    r.category[strcspn(r.category, "\n")] = 0;

    printf("Description: ");
    fgets(r.description, MAX_DESC, stdin);
    r.description[strcspn(r.description, "\n")] = 0;

    records[recordCount++] = r;
    saveToFile();

    printf(HR "Income added successfully (ID: %d)\n" HR, r.id);
    pauseScreen();
}

/* ---------------- ADD EXPENSE ---------------- */
void addExpense() {
    Record r;
    CLEAR;

    printf(HR "ADD EXPENSE\n" HR);

    r.id = nextId++;
    r.type = EXPENSE;

    if (!getSmartDate(&r.date)) {
        printf("\nInvalid date.\n");
        nextId--;
        pauseScreen();
        return;
    }

    printf("Amount: ₹");
    if (scanf("%f", &r.amount) != 1 || r.amount <= 0) {
        clearBuffer();
        printf("\nInvalid amount.\n");
        nextId--;
        pauseScreen();
        return;
    }
    clearBuffer();

    printf("Category: ");
    fgets(r.category, MAX_CATEGORY, stdin);
    r.category[strcspn(r.category, "\n")] = 0;

    printf("Description: ");
    fgets(r.description, MAX_DESC, stdin);
    r.description[strcspn(r.description, "\n")] = 0;

    records[recordCount++] = r;
    saveToFile();

    printf(HR "Expense added successfully (ID: %d)\n" HR, r.id);
    pauseScreen();
}

/* ---------------- VIEW ALL ---------------- */
void viewAllRecords() {
    CLEAR;
    printf(HR "ALL RECORDS\n" HR);

    if (recordCount == 0) {
        printf("No records found.\n");
        pauseScreen();
        return;
    }

    printf("ID   DATE        TYPE     AMOUNT      CATEGORY           DESCRIPTION\n");
    printf(HR);

    for (int i = 0; i < recordCount; i++) {
        printf("%-4d %02d/%02d/%04d %-8s ₹%-10.2f %-18s %s\n",
            records[i].id,
            records[i].date.day,
            records[i].date.month,
            records[i].date.year,
            records[i].type == INCOME ? "INCOME" : "EXPENSE",
            records[i].amount,
            records[i].category,
            records[i].description);
    }

    printf(HR);
    printf("Total Income : ₹%.2f\n", totalIncome());
    printf("Total Expense: ₹%.2f\n", totalExpense());
    printf("Balance      : ₹%.2f\n", totalIncome() - totalExpense());
    printf(HR);

    pauseScreen();
}

/* ---------------- MONTHLY SUMMARY ---------------- */
void monthlySummary() {
    int m, y;
    float in = 0, ex = 0;

    CLEAR;
    printf(HR "MONTHLY SUMMARY\n" HR);

    printf("Enter month and year (MM YYYY): ");
    scanf("%d %d", &m, &y);
    clearBuffer();

    for (int i = 0; i < recordCount; i++) {
        if (records[i].date.month == m && records[i].date.year == y) {
            if (records[i].type == INCOME)
                in += records[i].amount;
            else
                ex += records[i].amount;
        }
    }

    printf(HR);
    printf("Income : ₹%.2f\n", in);
    printf("Expense: ₹%.2f\n", ex);
    printf("Balance: ₹%.2f\n", in - ex);
    printf(HR);

    pauseScreen();
}

/* ---------------- CATEGORY SUMMARY ---------------- */
void categorySummary() {
    char categories[MAX_RECORDS][MAX_CATEGORY];
    float totals[MAX_RECORDS];
    int catCount = 0;

    CLEAR;
    printf(HR "CATEGORY SUMMARY (EXPENSES)\n" HR);

    for (int i = 0; i < recordCount; i++) {
        if (records[i].type == EXPENSE) {
            int found = -1;
            for (int j = 0; j < catCount; j++) {
                if (strcmp(categories[j], records[i].category) == 0)
                    found = j;
            }
            if (found == -1) {
                strcpy(categories[catCount], records[i].category);
                totals[catCount++] = records[i].amount;
            } else {
                totals[found] += records[i].amount;
            }
        }
    }

    for (int i = 0; i < catCount; i++)
        printf("%-20s : ₹%.2f\n", categories[i], totals[i]);

    pauseScreen();
}

/* ---------------- DELETE ---------------- */
void deleteRecord() {
    int id, index = -1;
    CLEAR;

    printf(HR "DELETE RECORD\n" HR);
    printf("Enter record ID: ");
    scanf("%d", &id);
    clearBuffer();

    for (int i = 0; i < recordCount; i++)
        if (records[i].id == id)
            index = i;

    if (index == -1) {
        printf("\nRecord not found.\n");
        pauseScreen();
        return;
    }

    for (int i = index; i < recordCount - 1; i++)
        records[i] = records[i + 1];

    recordCount--;
    saveToFile();

    printf("\nRecord deleted successfully.\n");
    pauseScreen();
}

/* ---------------- FILE ---------------- */
void saveToFile() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) return;

    fwrite(&recordCount, sizeof(int), 1, fp);
    fwrite(&nextId, sizeof(int), 1, fp);
    fwrite(records, sizeof(Record), recordCount, fp);
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return;

    fread(&recordCount, sizeof(int), 1, fp);
    fread(&nextId, sizeof(int), 1, fp);
    fread(records, sizeof(Record), recordCount, fp);
    fclose(fp);
}

/* ---------------- UTILITIES ---------------- */
float totalIncome() {
    float sum = 0;
    for (int i = 0; i < recordCount; i++)
        if (records[i].type == INCOME)
            sum += records[i].amount;
    return sum;
}

float totalExpense() {
    float sum = 0;
    for (int i = 0; i < recordCount; i++)
        if (records[i].type == EXPENSE)
            sum += records[i].amount;
    return sum;
}

int validateDate(Date d) {
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (d.year < 1900 || d.year > 2100) return 0;
    if (d.month < 1 || d.month > 12) return 0;

    if ((d.year % 4 == 0 && d.year % 100 != 0) || d.year % 400 == 0)
        daysInMonth[1] = 29;

    if (d.day < 1 || d.day > daysInMonth[d.month - 1]) return 0;
    return 1;
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

void clearBuffer() {
    while (getchar() != '\n');
}
