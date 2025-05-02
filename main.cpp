#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace chrono;

// Generate square matrix with random integers
vector<vector<int>> generateMatrix(int size) {
    vector<vector<int>> matrix(size, vector<int>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = rand() % 10;
    return matrix;
}

// Save flat result matrix to file
void saveMatrixToFile(const string& filename, const vector<int>& flat_matrix, int rows, int cols) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    file << rows << " " << cols << endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << flat_matrix[i * cols + j] << " ";
        }
        file << endl;
    }
    file.close();
}

// Save execution time to a log file
void saveExecutionTime(const string& filename, int size, int num_procs, double timeElapsed) {
    ofstream file(filename, ios::app);
    if (!file) {
        cerr << "Error: Cannot open timing file\n";
        return;
    }
    file << size << " " << num_procs << " " << timeElapsed << endl;
    file.close();
}

int main(int argc, char* argv[]) {
    vector<int> sizes = {100, 200, 400, 1000};
    int rank, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    srand(time(0) + rank);

    for (int size : sizes) {
        vector<vector<int>> A, B;

        if (rank == 0) {
            A = generateMatrix(size);
            B = generateMatrix(size);
        }

        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int rows_per_proc = size / num_procs;

        vector<vector<int>> local_A(rows_per_proc, vector<int>(size));
        vector<vector<int>> result_C(rows_per_proc, vector<int>(size));

        vector<int> flat_A, flat_B, flat_local_A(size * rows_per_proc), flat_result(rows_per_proc * size);

        if (rank == 0) {
            flat_A.reserve(size * size);
            flat_B.reserve(size * size);
            for (auto& row : A) flat_A.insert(flat_A.end(), row.begin(), row.end());
            for (auto& row : B) flat_B.insert(flat_B.end(), row.begin(), row.end());
        } else {
            flat_B.resize(size * size); // Allocate memory for receiving matrix B
        }

        MPI_Bcast(flat_B.data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank != 0) {
            flat_local_A.resize(size * rows_per_proc); // Allocate local A part
        }

        MPI_Scatter(flat_A.data(), size * rows_per_proc, MPI_INT,
                    flat_local_A.data(), size * rows_per_proc, MPI_INT,
                    0, MPI_COMM_WORLD);

        for (int i = 0; i < rows_per_proc; i++)
            for (int j = 0; j < size; j++)
                local_A[i][j] = flat_local_A[i * size + j];

        vector<vector<int>> B_matrix(size, vector<int>(size));
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                B_matrix[i][j] = flat_B[i * size + j];

        auto start = high_resolution_clock::now();

        for (int i = 0; i < rows_per_proc; i++) {
            for (int j = 0; j < size; j++) {
                int sum = 0;
                for (int k = 0; k < size; k++) {
                    sum += local_A[i][k] * B_matrix[k][j];
                }
                result_C[i][j] = sum;
                flat_result[i * size + j] = sum;
            }
        }

        vector<int> flat_final_result;
        if (rank == 0) flat_final_result.resize(size * size);

        MPI_Gather(flat_result.data(), rows_per_proc * size, MPI_INT,
                   flat_final_result.data(), rows_per_proc * size, MPI_INT,
                   0, MPI_COMM_WORLD);

        auto stop = high_resolution_clock::now();
        double elapsed_time = duration<double>(stop - start).count();

        if (rank == 0) {
            string result_file ="matrix_result_mpi_" + to_string(size) + ".txt";
            saveMatrixToFile(result_file, flat_final_result, size, size);

            string time_file = "mpi_execution_times.txt";
            saveExecutionTime(time_file, size, num_procs, elapsed_time);

            cout << "Size: " << size << "x" << size
                 << ", Processes: " << num_procs
                 << ", Time: " << elapsed_time << " seconds\n";
        }
    }

    MPI_Finalize();
    return 0;
}