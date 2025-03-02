import numpy as np
import sys
import os  

def read_matrix(filename):
    try:
        if not os.path.exists(filename):
            raise FileNotFoundError(f"File {filename} don't have!")

        with open(filename, 'r') as f:
            lines = f.readlines()

            lines = lines[1:]

            if "resultMatrix" in filename:
                lines = lines[2:]

            matrix = [list(map(int, line.split())) for line in lines]
        
        return np.array(matrix)

    except Exception as e:
        print(f"Error{filename}: {e}")
        sys.exit(1)

def verify_multiplication(size):
    fileA = f"matrixA_{size}.txt"
    fileB = f"matrixB_{size}.txt"
    fileResult = f"resultMatrix_{size}.txt"

    A = read_matrix(fileA)
    B = read_matrix(fileB)
    result_cpp = read_matrix(fileResult)

    if A.shape[1] != B.shape[0]:
        print(f"Error: Size {A.shape} x {B.shape}")
        return False

    result_numpy = np.dot(A, B)

    if np.array_equal(result_numpy, result_cpp):
        print(f"Correct{size}x{size}!")
        return True
    else:
        print(f"Verification failed for size {size}x{size}!")
        diff = np.abs(result_numpy - result_cpp)
        print("Total absolute error:", np.sum(diff)) 
        return False

if __name__ == "__main__":
    sizes = [100, 200, 300, 400, 500]

    all_passed = True
    for size in sizes:
        if not verify_multiplication(size):
            all_passed = FalseS

    if all_passed:
        print("All matrix multiplications are correct!")
    else:
        print("Some matrix multiplications are incorrect, check the data.")