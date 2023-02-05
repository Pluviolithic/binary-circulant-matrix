import os
import git
import json
import numpy as np

from sympy import Matrix
from more_itertools import distinct_combinations


def generateMatrix(combination, size=24):
    m = []
    for i in range(size):
        row = [0] * size
        for j in range(size):
            for k in combination:
                if j == ((i + k) % size):
                    row[j] = 1
                    break
        m.append(row[::-1])

    return Matrix(np.array(m[::-1]))


def findMatrixInverse(combination, solutions, size=24):
    m = generateMatrix(combination, size)

    try:
        inv = np.array(m.inv_mod(2)).astype(np.float64)
    except:
        return

    indices = []
    for x in range(size):
        if inv[size - 1][::-1][x] == 1:
            indices.append(str(x))

    solutions[", ".join(map(str, combination))] = ", ".join(indices)


def inverseAll(solutions, size=24):
    for n in range(size):
        combinations = distinct_combinations([x for x in range(size)], n)
        for combination in combinations:
            findMatrixInverse(combination, solutions, size)


for i in range(2, 100):
    solutions = {}

    inverseAll(solutions, i)
    for key, value in solutions.items():
            print(key, " -> ", value)
            
    solutions["size"] = len(solutions)

    git_repo = git.Repo(os.getcwd(), search_parent_directories=True)
    git_root = git_repo.git.rev_parse("--show-toplevel")

    with open(git_root + "\\json\\py_out\\" + str(i) + ".json", "w") as outfile:
        json.dump(solutions, outfile, indent=4)
