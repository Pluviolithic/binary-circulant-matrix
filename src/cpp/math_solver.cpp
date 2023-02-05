#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>

// used for debugging purposes
void printMatrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// generate a binary circulant matrix with three high bits in each row
// the high bits are determined by the following formula:
// x_i = y_((i + a) % size) ^ y_((i + b) % size) ^ y_((i + c) % size)
int **generateMatrix(int a, int b, int c, int size) {
    int **matrix = new int *[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new int[size];
        for (int j = 0; j < size; j++) {
            if (j == ((i + a) % size) || j == ((i + b) % size) ||
                j == ((i + c) % size)) {
                matrix[i][j] = 1;
            } else {
                matrix[i][j] = 0;
            }
        }
    }
    return matrix;
}

int **multiplyMatrices(int **a, int **b, int size) {
    int **result = new int *[size];
    for (int i = 0; i < size; i++) {
        result[i] = new int[size];
        for (int j = 0; j < size; j++) {
            result[i][j] = 0;
            for (int k = 0; k < size; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

int **addModMatrices(int **a, int **b, int size) {
    int **result = new int *[size];
    for (int i = 0; i < size; i++) {
        result[i] = new int[size];
        for (int j = 0; j < size; j++) {
            result[i][j] = (a[i][j] + b[i][j]);
        }
    }
    return result;
}

int **subtractModMatrices(int **a, int **b, int size) {
    int **result = new int *[size];
    for (int i = 0; i < size; i++) {
        result[i] = new int[size];
        for (int j = 0; j < size; j++) {
            result[i][j] = (a[i][j] - b[i][j]);
        }
    }
    return result;
}

int **strassenMultiply(int **a, int **b, int N) {
    if (N == 1) {
        int **result = new int *[1];
        result[0] = new int[1];
        result[0][0] = a[0][0] * b[0][0];
        return result;
    }

    int K = N / 2;

    int **a11 = new int *[K];
    int **a12 = new int *[K];
    int **a21 = new int *[K];
    int **a22 = new int *[K];
    int **b11 = new int *[K];
    int **b12 = new int *[K];
    int **b21 = new int *[K];
    int **b22 = new int *[K];

    for (int i = 0; i < K; i++) {
        a11[i] = new int[K];
        a12[i] = new int[K];
        a21[i] = new int[K];
        a22[i] = new int[K];
        b11[i] = new int[K];
        b12[i] = new int[K];
        b21[i] = new int[K];
        b22[i] = new int[K];
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            a11[i][j] = a[i][j];
            a12[i][j] = a[i][j + K];
            a21[i][j] = a[i + K][j];
            a22[i][j] = a[i + K][j + K];
            b11[i][j] = b[i][j];
            b12[i][j] = b[i][j + K];
            b21[i][j] = b[i + K][j];
            b22[i][j] = b[i + K][j + K];
        }
    }

    int **S1 = subtractModMatrices(b12, b22, K);
    int **S2 = addModMatrices(a11, a12, K);
    int **S3 = addModMatrices(a21, a22, K);
    int **S4 = subtractModMatrices(b21, b11, K);
    int **S5 = addModMatrices(a11, a22, K);
    int **S6 = addModMatrices(b11, b22, K);
    int **S7 = subtractModMatrices(a12, a22, K);
    int **S8 = addModMatrices(b21, b22, K);
    int **S9 = subtractModMatrices(a11, a21, K);
    int **S10 = addModMatrices(b11, b12, K);

    int **P1 = strassenMultiply(a11, S1, K);
    int **P2 = strassenMultiply(S2, b22, K);
    int **P3 = strassenMultiply(S3, b11, K);
    int **P4 = strassenMultiply(a22, S4, K);
    int **P5 = strassenMultiply(S5, S6, K);
    int **P6 = strassenMultiply(S7, S8, K);
    int **P7 = strassenMultiply(S9, S10, K);

    int **C11 = subtractModMatrices(
        addModMatrices(addModMatrices(P5, P4, K), P6, K), P2,
        K);
    int **C12 = addModMatrices(P1, P2, K);
    int **C21 = addModMatrices(P3, P4, K);
    int **C22 = subtractModMatrices(
        subtractModMatrices(addModMatrices(P5, P1, K), P3, K), P7,
        K);

    int **C = new int *[N];
    for (int i = 0; i < N; i++) {
        C[i] = new int[N];
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            C[i][j] = C11[i][j];
            C[i][j + K] = C12[i][j];
            C[i + K][j] = C21[i][j];
            C[i + K][j + K] = C22[i][j];
        }
    }

    return C;
}

int main() {
    int **a = generateMatrix(15, 28, 31, 32);
    int **b = generateMatrix(4, 7, 23, 32);
    int **c = strassenMultiply(a, b, 32);
    int **d = multiplyMatrices(a, b, 32);

    printMatrix(a, 32);
    std::cout << std::endl;
    printMatrix(b, 32);
    std::cout << std::endl;
    printMatrix(c, 32);
    std::cout << std::endl;
    printMatrix(d, 32);
}