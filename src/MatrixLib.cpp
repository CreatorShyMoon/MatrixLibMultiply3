#include "MatrixLib.hpp" 
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

namespace MatrixLib {

// Функция для динамического выделения памяти под матрицу
int** allocateMatrix(int rows, int cols) {
    int** matrix = new int*[rows];             // Выделяем массив указателей на строки
    for (int i = 0; i < rows; ++i)
        matrix[i] = new int[cols];             // Для каждой строки выделяем массив столбцов
    return matrix;
}

// Функция для освобождения памяти матрицы
void deallocateMatrix(int** matrix, int rows) {
    for (int i = 0; i < rows; ++i)
        delete[] matrix[i];                    // Удаляем каждую строку
    delete[] matrix;                           // Удаляем массив указателей
}

// Чтение матрицы из файла и выделение под неё памяти
int readMatrixFromFile(const string& filename, int**& matrix, int& rows, int& cols) {
    ifstream file(filename);                   // Открываем файл
    if (!file.is_open()) {                     // Проверка открытия
        cerr << "Ошибка открытия файла " << filename << endl;
        return -1;                             // Возвращаем ошибку, если файл не открылся
    }

    string line;
    rows = 0;
    cols = 0;
    vector<vector<int> > temp;                  // Временное хранилище данных матрицы

    // Построчное чтение файла
    while (getline(file, line)) {
        istringstream iss(line);               // Оборачиваем строку как поток
        int value;
        vector<int> row;

        while (iss >> value)                   // Считываем все числа в строке
            row.push_back(value);

        if (rows == 0)
            cols = row.size();                 // Запоминаем количество столбцов по первой строке
        else if (row.size() != cols) {         // Проверка на несовпадение числа столбцов
            cerr << "Ошибка: строка " << (rows + 1) << " имеет " << row.size() << " столбцов, ожидалось " << cols << endl;
            return -1;                         // Возвращаем ошибку, если строки не одинаковы по количеству столбцов
        }

        temp.push_back(row);                   // Сохраняем строку во временный вектор
        rows++;                                // Увеличиваем счётчик строк
    }
    file.close();                              // Закрываем файл

    // Выделяем память под результирующую матрицу
    matrix = allocateMatrix(rows, cols);

    // Копируем данные из временного вектора в выделенную матрицу
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            matrix[i][j] = temp[i][j];

    return 0;                                   
}
// Запись матрицы в файл
void writeMatrixToFile(const string& filename, int** matrix, int rows, int cols) {
    ofstream file(filename);                    // Открываем файл для записи
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла для записи: " << filename << endl;
        return;                                  // Возвращаемся, если файл не открылся
    }

    // Построчная запись
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << matrix[i][j];               // Запись элемента в файл
            if (j < cols - 1) file << " ";      // Разделитель между элементами
        }
        file << endl;                           // Переход на новую строку
    }

    file.close();                       
}

// Умножение матриц A (m x n) и B (n x p), результат записывается в C (m x p)
void multiplyClassic(int** A, int** B, int** C, int m, int n, int p) {
    for (int i = 0; i < m; ++i)               // Перебор строк матрицы A
        for (int k = 0; k < p; ++k) {         // Перебор столбцов матрицы B
            C[i][k] = 0;                      // Обнуляем элемент результата
            for (int j = 0; j < n; ++j)       // Перемножаем элементы строк A и столбцов B
                C[i][k] += A[i][j] * B[j][k];  // Стандартное перемножение
        }
}

// Блочное умножение матриц
void multiplyBlocked(int** A, int** B, int** C, int m, int n, int p, int blockSize) {
    // Перебор блоков по строкам матрицы A
    for (int iBlock = 0; iBlock < m; iBlock += blockSize) {
        int iMax = min(iBlock + blockSize, m);   // Ограничиваем максимальный индекс для блока

        // Перебор блоков по столбцам матрицы B
        for (int jBlock = 0; jBlock < p; jBlock += blockSize) {
            int jMax = min(jBlock + blockSize, p);  // Ограничиваем максимальный индекс для блока

            // Перебор блоков по общей координате (внутренний размер)
            for (int kBlock = 0; kBlock < n; kBlock += blockSize) {
                int kMax = min(kBlock + blockSize, n);  // Ограничиваем максимальный индекс для блока

                // Проход по строкам внутри блока
                for (int i = iBlock; i < iMax; ++i) {
                    // Проход по столбцам внутри блока
                    for (int k = kBlock; k < kMax; ++k) {
                        int a_ik = A[i][k]; // читаем A[i][k] один раз
                        // Проход по столбцам матрицы B
                        for (int j = jBlock; j < jMax; ++j) {
                            C[i][j] += a_ik * B[k][j]; // Накопление результата умножения
                        }
                    }
                }
            }
        }
    }
}

// Сложение матриц
void add(int** A, int** B, int** result, int size) {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            result[i][j] = A[i][j] + B[i][j];
}

// Вычитание матриц
void subtract(int** A, int** B, int** result, int size) {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            result[i][j] = A[i][j] - B[i][j];
}
// Алгоритм Штрассена
void multiplyStrassen(int** A, int** B, int** C, int size) {
    if (size <= 64) { // На малых размерах — обычное умножение
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                for (int k = 0; k < size; ++k)
                    C[i][j] += A[i][k] * B[k][j];
        return;
    }

    int newSize = size / 2;
    int** A11 = allocateMatrix(newSize, newSize);
    int** A12 = allocateMatrix(newSize, newSize);
    int** A21 = allocateMatrix(newSize, newSize);
    int** A22 = allocateMatrix(newSize, newSize);
    int** B11 = allocateMatrix(newSize, newSize);
    int** B12 = allocateMatrix(newSize, newSize);
    int** B21 = allocateMatrix(newSize, newSize);
    int** B22 = allocateMatrix(newSize, newSize);
    int** M1 = allocateMatrix(newSize, newSize);
    int** M2 = allocateMatrix(newSize, newSize);
    int** M3 = allocateMatrix(newSize, newSize);
    int** M4 = allocateMatrix(newSize, newSize);
    int** M5 = allocateMatrix(newSize, newSize);
    int** M6 = allocateMatrix(newSize, newSize);
    int** M7 = allocateMatrix(newSize, newSize);
    int** T1 = allocateMatrix(newSize, newSize);
    int** T2 = allocateMatrix(newSize, newSize);

    // Разделение матриц A и Bj
    for (int i = 0; i < newSize; ++i)
        for (int j = 0; j < newSize; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + newSize];
            A21[i][j] = A[i + newSize][j];
            A22[i][j] = A[i + newSize][j + newSize];
            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + newSize];
            B21[i][j] = B[i + newSize][j];
            B22[i][j] = B[i + newSize][j + newSize];
        }

    add(A11, A22, T1, newSize);
    add(B11, B22, T2, newSize);
    multiplyStrassen(T1, T2, M1, newSize);

    add(A21, A22, T1, newSize);
    multiplyStrassen(T1, B11, M2, newSize);

    subtract(B12, B22, T1, newSize);
    multiplyStrassen(A11, T1, M3, newSize);

    subtract(B21, B11, T1, newSize);
    multiplyStrassen(A22, T1, M4, newSize);

    add(A11, A12, T1, newSize);
    multiplyStrassen(T1, B22, M5, newSize);

    subtract(A21, A11, T1, newSize);
    add(B11, B12, T2, newSize);
    multiplyStrassen(T1, T2, M6, newSize);

    subtract(A12, A22, T1, newSize);
    add(B21, B22, T2, newSize);
    multiplyStrassen(T1, T2, M7, newSize);

    // C11 = M1 + M4 - M5 + M7
    for (int i = 0; i < newSize; ++i)
        for (int j = 0; j < newSize; ++j)
            C[i][j] = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j];

    // C12 = M3 + M5
    for (int i = 0; i < newSize; ++i)
        for (int j = 0; j < newSize; ++j)
            C[i][j + newSize] = M3[i][j] + M5[i][j];

    // C21 = M2 + M4
    for (int i = 0; i < newSize; ++i)
        for (int j = 0; j < newSize; ++j)
            C[i + newSize][j] = M2[i][j] + M4[i][j];

    // C22 = M1 - M2 + M3 + M6
    for (int i = 0; i < newSize; ++i)
        for (int j = 0; j < newSize; ++j)
            C[i + newSize][j + newSize] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];


    }
} // namespace MatrixLib