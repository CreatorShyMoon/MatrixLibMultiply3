#ifndef MATRIX_LIB_HPP
#define MATRIX_LIB_HPP

#include <string>

namespace MatrixLib {

int** allocateMatrix(int rows, int cols);
void deallocateMatrix(int** matrix, int rows);
int readMatrixFromFile(const std::string& filename, int**& matrix, int& rows, int& cols);
void writeMatrixToFile(const std::string& filename, int** matrix, int rows, int cols);
void subtract(int** A, int** B, int** result, int size);
void add(int** A, int** B, int** result, int size);

// Алгоритмы
void multiplyClassic(int** A, int** B, int** C, int m, int n, int p);
void multiplyBlocked(int** A, int** B, int** C, int m, int n, int p, int blockSize);
void multiplyStrassen(int** A, int** B, int** C, int size);

} // namespace MatrixLib

#endif // MATRIX_LIB_HPP
