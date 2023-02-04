#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// used for debugging purposes
void printMatrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// simple method to get all combinations of a set of indices
// implementation from rosetta code
// https://rosettacode.org/wiki/Combinations#C++
std::vector<int *> comb(int n, int k) {
    std::vector<int *> combinations;
    std::string bitmask(k, 1);
    bitmask.resize(n, 0);

    do {
        int *combination = new int[k];
        int j = 0;
        for (int i = 0; i < n; ++i) {
            if (bitmask[i]) {
                combination[j++] = i;
            }
        }
        combinations.push_back(combination);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    return combinations;
}

// convert every element in the matrix to base 2
int **modMatrix(int **matrix, int size, int mod) {
    int **result = new int *[size];
    for (int i = 0; i < size; i++) {
        result[i] = new int[size];
        for (int j = 0; j < size; j++) {
            result[i][j] = matrix[i][j] % mod;
        }
    }
    return result;
}

bool matricesMatch(int **a, int **b, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (a[i][j] != b[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// used to verify a generated inverse
// since A * A^-1 = I, we can simply check (A * A^-1) % 2 == I
int **getIdentityMatrix(int size) {
    int **matrix = new int *[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new int[size];
        for (int j = 0; j < size; j++) {
            if (i == j) {
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

// check if a matrix generated by a combination of three indices (for the high bits)
// is an inverse of the original matrix
bool combinationIsInverse(int **matrix, int **identity, int *combination,
                          int size) {
    int **proposedInverse =
        generateMatrix(combination[0], combination[1], combination[2], size);
    int **product = multiplyMatrices(matrix, proposedInverse, size);
    int **moddedMatrix = modMatrix(product, size, 2);

    bool result = matricesMatch(moddedMatrix, identity, size);

    for (int i = 0; i < size; i++) {
        delete[] proposedInverse[i];
        delete[] moddedMatrix[i];
        delete[] product[i];
    }

    delete[] proposedInverse;
    delete[] moddedMatrix;
    delete[] product;

    return result;
}

// for a given matrix, find the inverse by bruteforcing all possible combinations
// that can be used to generate a "valid" binary circulant matrix
int *findMatrixInverse(int **matrix, std::vector<int *> combinations,
                       int size) {
    int *inverse = nullptr;
    int **identity = getIdentityMatrix(size);

    for (int i = 0; i < combinations.size(); i++) {
        int *combination = combinations[i];
        if (combination[0] == combination[1] &&
            combination[1] == combination[2]) {
            continue;
        }
        if (combinationIsInverse(matrix, identity, combination, size)) {
            inverse = combination;
            break;
        }
    }

    for (int i = 0; i < size; i++) {
        delete[] identity[i];
        delete[] matrix[i];
    }

    delete[] identity;
    delete[] matrix;

    return inverse;
}

// generate all combinations of indices and their resultant matrices
// for each such matrix, see if an inverse can be found by iterating overall the other combinations
std::unordered_map<std::string, std::string> findAllMatrixInverses(int size) {
    std::vector<int *> combinations = comb(size, 3);
    std::unordered_map<std::string, std::string> map;
    for (int i = 0; i < combinations.size(); i++) {
        int *combination = combinations[i];
        if (combination[0] == combination[1] &&
            combination[1] == combination[2]) {
            continue;
        }

        int *inverse =
            findMatrixInverse(generateMatrix(combination[0], combination[1],
                                             combination[2], size),
                              combinations, size);

        if (inverse == nullptr) {
            continue;
        }

        std::string key = std::to_string(combination[0]) + ", " +
                          std::to_string(combination[1]) + ", " +
                          std::to_string(combination[2]);
        std::string value = std::to_string(inverse[0]) + ", " +
                            std::to_string(inverse[1]) + ", " +
                            std::to_string(inverse[2]);

        // store results in a map so the number of inverses can be counted
        // and the results can be written to a json file for further analysis
        map[key] = value;
    }

    for (int i = 0; i < combinations.size(); i++) {
        delete[] combinations[i];
    }

    return map;
}

void writeResultsToJSONFile(std::unordered_map<std::string, std::string> map,
                            int size) {
    std::ofstream file;
    file.open("..\\..\\json\\cpp_out\\" + std::to_string(size) + ".json",
              std::ios::app);
    file << "{\n";
    file << "\t\"size\": " << map.size() << ",\n";
    file << "\t\"results\": [\n";
    for (auto it = map.begin(); it != map.end(); it++) {
        file << "\t\t{\n";
        file << "\t\t\t\"key\": \"" << it->first << "\",\n";
        file << "\t\t\t\"value\": \"" << it->second << "\"\n";
        file << "\t\t}";
        if (std::next(it) != map.end()) {
            file << ",";
        }
        file << "\n";
    }
    file << "\t]\n";
    file << "}";
    file.close();
}

// this was separated out into its own function
// to potentially make future multithread tests possible/easier
void performCalculations(int start, int stop) {
    for (int i = start; i < stop; i++) {
        std::cout << "Size: " << i << std::endl;
        std::unordered_map<std::string, std::string> map =
            findAllMatrixInverses(i);

        writeResultsToJSONFile(map, i);

        for (auto it = map.begin(); it != map.end(); it++) {
            std::cout << it->first << " -> " << it->second << std::endl;
        }
    }
}

int main() {
    performCalculations(4, 33);
    return 0;
}