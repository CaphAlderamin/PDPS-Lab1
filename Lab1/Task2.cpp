#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>
using namespace std;

//Умножение матриц
__int64** oneThreadMatrixMultiplication(int dimension, __int64** matrixA, __int64** matrixB)
{
    __int64** matrixC = new __int64* [dimension];
    for (int i = 0; i < dimension; i++)
    {
        matrixC[i] = new __int64[dimension];
    }

    for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
        {
            __int64 result = 0;
            for (int k = 0; k < dimension; k++)
            {
                result += matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] = result;
        }
    return matrixC;
}

//Заполнение матриц
__int64** matrixFilling(int dimension)
{
    __int64** matrix = new __int64* [dimension];
    for (int i = 0; i < dimension; i++)
    {
        matrix[i] = new __int64[dimension];
    }

    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            matrix[i][j] = rand() % 10;
        }
    }

    return matrix;
}

//Печать матриц
void matrixPrint(int dimension, __int64** matrix)
{
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}
void matrixPrint(int rows, int cols, __int64** matrix)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}
void matrixPrint(int dimension, __int64** matrixA, __int64** matrixB, __int64** matrixC)
{
    cout << endl;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            cout << matrixA[i][j] << " ";
        }
        cout << "   ";
        for (int j = 0; j < dimension; j++)
        {
            cout << matrixB[i][j] << " ";
        }
        cout << "   ";
        for (int j = 0; j < dimension; j++)
        {
            cout << matrixC[i][j] << " ";
        }
        cout << endl;
    }
}

//Соединения матриц из вектора
__int64** matrixConnection(int dimension, int stepForThread, int threadNumber, vector<__int64**> threadsResults)
{
    __int64** matrix = new __int64* [dimension];
    for (int i = 0; i < dimension; i++)
    {
        matrix[i] = new __int64[dimension];
    }

    int row = 0;
    int i = 0;
    for (auto threadResult : threadsResults)
    {
        int divisionRemains = (i == threadNumber - 1 ? fmod(dimension, threadNumber) : 0);
        //int size = sizeof(*threadResult) / sizeof(threadResult[0]);
        for (int i = 0; i < stepForThread + divisionRemains; i++)
        {
            for (int j = 0; j < dimension; j++)
            {
                matrix[row][j] = threadResult[i][j];
            }
            row++;
        }
        i++;
    }

    return matrix;
}

//Вырезание нужных строк из матрицы
__int64** matrixCutting(int dimension, int firsRow, int lastRow, __int64** matrix)
{
    __int64** cuttedMatrix = new __int64* [lastRow - firsRow];
    for (int i = 0; i < lastRow - firsRow; i++)
    {
        cuttedMatrix[i] = new __int64[dimension];
    }

    for (int i = 0; i < lastRow - firsRow; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            cuttedMatrix[i][j] = matrix[i + firsRow][j];
        }
    }

    return cuttedMatrix;
}

int secondTask()
{
    int dimension = 2048;
    int printDimension = 4;
    int threadNumber = 6;
    int repeats = 10;

    cout <<  "Dimension: " << dimension << "\nThread number: " << threadNumber << "\nRepeats number: " << repeats << endl;

    //Вычисление шага для распределения по потокам
    int stepForThread = dimension / threadNumber;

    srand(time(NULL));

    //Вектор для результатов поступающих из потоков
    vector<__int64**> threadsResults(threadNumber, 0);
    //Анонимная функция произведения матриц
    auto matrixMultiplication
    {
        [&threadsResults](int threadID, int rowsA, int colsA, __int64** matrixA, int rowsB, int colsB, __int64** matrixB)
        {
            __int64** matrixC = new __int64* [rowsA];
            for (int i = 0; i < rowsA; i++)
            {
                matrixC[i] = new __int64[rowsB];
            }

            for (int i = 0; i < rowsA; i++)
            {
                for (int j = 0; j < colsB; j++)
                {
                    __int64 result = 0;
                    for (int k = 0; k < rowsB; k++)
                    {
                        result += matrixA[i][k] * matrixB[k][j];
                    }
                    matrixC[i][j] = result;
                }
            }
            threadsResults.at(threadID) = matrixC;
            cout << "Thread " << setw(7) << this_thread::get_id() << " is stopped calculating" << endl;
        }
    };

    vector<chrono::duration<float>> severalThreadDuration;
    vector<chrono::duration<float>> oneThreadDuration;

    __int64** matrixA;
    __int64** matrixB;
    __int64** matrixC;

    cout << endl;
    for (int n = 0; n < repeats; n++)
    {
        cout << endl << "============== Repeat " << n << " ==============" << endl;

        //Заполнение матриц
        matrixA = matrixFilling(dimension);
        matrixB = matrixFilling(dimension);

        //========Многопоточное произведение матриц========
        //Вектор для потоков
        vector<std::thread> threads;

        auto start = chrono::high_resolution_clock::now(); //Точка для начала счета времени

        //Распределение вычислений на потоки
        for (int i = 0; i < threadNumber; i++)
        {
            double divisionRemains = (i == threadNumber - 1 ? fmod(dimension, threadNumber) : 0);
            threads.emplace_back(matrixMultiplication, i, stepForThread + divisionRemains, dimension, matrixCutting(dimension, i * stepForThread, (i + 1) * stepForThread + divisionRemains, matrixA), dimension, dimension, matrixB);
        }

        //Ожидание выполнения всех потоков
        for (auto& thread : threads)
        {
            thread.join();
        }

        //matrixPrint(1, dimension, threadsResults[0]); matrixPrint(1, dimension, threadsResults[1]); matrixPrint(1, dimension, threadsResults[2]); matrixPrint(2, dimension, threadsResults[3]);

        //Соединение матриц
        matrixC = matrixConnection(dimension, stepForThread, threadNumber, threadsResults);

        auto end = chrono::high_resolution_clock::now(); //Точка для конца счета времени
        severalThreadDuration.emplace_back(end - start);

        //Вывод полученной матрицы
        matrixPrint(printDimension, matrixA, matrixB, matrixC);
        //==================================================
        

        //=========Однопоточное произведение матриц=========
        start = chrono::high_resolution_clock::now(); //Точка для начала счета времени
        
        matrixC = oneThreadMatrixMultiplication(dimension, matrixA, matrixB);

        end = chrono::high_resolution_clock::now(); //Точка для конца счета времени
        oneThreadDuration.emplace_back(end - start);

        //Вывод полученной матрицы
        matrixPrint(printDimension, matrixA, matrixB, matrixC);
        //==================================================
        
    }
    cout << "=====================================" << endl << endl;

    //Вывод результата для многопоточного вычисления
    float severalThreadDurationTime = 0;
    cout << "All several threads calculation times: " << endl;
    for (auto& time : severalThreadDuration)
    {
        cout << time.count() << endl;
        severalThreadDurationTime += time.count();
    }
    cout << "General time of several threads calculation    " << severalThreadDurationTime << endl;
    cout << "Average time of several threads calculation    " << severalThreadDurationTime / repeats << endl << endl;

    //Вывод результата для однопоточного вычисления
    float oneThreadDurationTime = 0;
    cout << "All one thread calculation times: " << endl;
    for (auto& time : oneThreadDuration)
    {
        cout << time.count() << endl;
        oneThreadDurationTime += time.count();
    }
    cout << "General time of one thread calculation " << oneThreadDurationTime << endl;
    cout << "Average time of one thread calculation " << oneThreadDurationTime / repeats << endl << endl;

    //Память
    for (int i = 0; i < dimension; i++)
    {
        delete[] matrixA[i];
        delete[] matrixB[i];
        delete[] matrixC[i];
    }
    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixC;

	return 1;
}