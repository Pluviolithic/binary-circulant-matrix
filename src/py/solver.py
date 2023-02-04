import os
import git
import json
import numpy as np

from sympy import Matrix
from more_itertools import distinct_combinations


def generateMatrix(a, b, c, size=24):
    m = []
    for i in range(size):
        row = [0] * size
        for j in range(size):
            if j == ((i + a) % size):
                row[j] = 1
            elif j == ((i + b) % size):
                row[j] = 1
            elif j == ((i + c) % size):
                row[j] = 1
            else:
                row[j] = 0
        m.append(row[::-1])

    return Matrix(np.array(m[::-1]))


def findMatrixInverse(i, j, k, solutions, size=24):
    m = generateMatrix(i, j, k, size)

    try:
        inv = np.array(m.inv_mod(2)).astype(np.float64)
    except:
        print("value error: ", i, j, k)
        return

    indices = []
    for x in range(size):
        if inv[size - 1][::-1][x] == 1:
            indices.append(str(x))

    solutions[", ".join(map(str, [i, j, k]))] = ", ".join(indices)


def inverseAll(solutions, size=24):
    combinations = distinct_combinations([x for x in range(size)], 3)
    for combination in combinations:
        i, j, k = combination
        if i == j and j == k:
            continue
        findMatrixInverse(i, j, k, solutions, size)


for i in range(4, 100):
    solutions = {}

    solutions["size"] = 0
    inverseAll(solutions, i)
    solutions["size"] = len(solutions) - 1

    git_repo = git.Repo(os.getcwd(), search_parent_directories=True)
    git_root = git_repo.git.rev_parse("--show-toplevel")

    with open(git_root + "\\json\\py_out\\" + str(i) + ".json", "w") as outfile:
        json.dump(solutions, outfile, indent=4)

        for key, value in solutions.items():
            print(key, " -> ", value)
