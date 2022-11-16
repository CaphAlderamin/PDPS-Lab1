#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>
using namespace std;

//Выражение общего члена ряда (может быть любым)
double seriesMember(double n)
{
	return n;
}

//Вычисление суммы ряда в однопоточном режиме
double oneThreadSeriesCalculation(double seriesLowerLimit, double seriesUpperLimit)
{
	double result = 0;
	for (double i = seriesLowerLimit; i < seriesUpperLimit; i++)
	{
		result += seriesMember(i);
	}
	cout << "Thread " << setw(7) << this_thread::get_id() << " is stopped calculating" << endl;
	return result;
}

int firstTask()
{
	// "seriesMember" - общий член ряда
	double seriesUpperLimit = 10000000000;
	double seriesLowerLimit = 1;
	int threadNumber = 6;
	int repeats = 10;

	cout << "Series: " << seriesUpperLimit << "\nThread number: " << threadNumber << "\nRepeats number: " << repeats << endl; 

	//Вектор для результатов поступающих из потоков
	vector<double> threadsResults(threadNumber, 0.);
	//Анонимная функция вычисление суммы ряда
	auto seriesCalculation
	{
		[&threadsResults](int threadID, double seriesLowerLimit, double seriesUpperLimit)
		{
			double result = 0;
			for (double i = seriesLowerLimit; i < seriesUpperLimit; i++)
			{
				result += seriesMember(i);
			}
			threadsResults.at(threadID) = result;
			cout << "Thread " << setw(7) << this_thread::get_id() << " is stopped calculating" << endl;
		}
	};

	//Вычисление шага для распределения по потокам
	double stepForThread = seriesUpperLimit / threadNumber;

	vector<chrono::duration<float>> severalThreadDuration;
	vector<chrono::duration<float>> oneThreadDuration;

	for (int n = 0; n < repeats; n++)
	{
		cout << endl << "============== Repeat " << n << " ==============" << endl;
		
		//=======Многопоточное вычисление суммы ряда========
		//Вектор для потоков
		vector<std::thread> threads;

		auto start = chrono::high_resolution_clock::now(); //Точка для начала счета времени

		//Распределение вычислений на потоки
		for (int i = 0; i < threadNumber; i++) 
		{
			double divisionRemains = (i == threadNumber - 1 ? fmod(seriesUpperLimit, threadNumber) : 0);
			threads.emplace_back(seriesCalculation, i, i * stepForThread, (i + 1) * stepForThread + divisionRemains);
		}

		//Ожидание выполнения всех потоков
		for (auto& thread : threads) 
		{
			thread.join();
		}
		
		auto end = chrono::high_resolution_clock::now(); //Точка для конца счета времени
		severalThreadDuration.emplace_back(end - start);
		
		cout << "Result of several threads calculation:    " << std::accumulate(threadsResults.begin(), threadsResults.end(), 0.) << endl;
		//cout << "Time of threads calculation:      " << severalThreadDuration.back().count() << endl << endl;
		//==================================================
		
		//=======Однопоточное вычисление суммы ряда=========
		start = chrono::high_resolution_clock::now(); //Точка для начала счета времени
		
		double oneThreadResult = oneThreadSeriesCalculation(seriesLowerLimit, seriesUpperLimit);
		
		end = chrono::high_resolution_clock::now(); //Точка для конца счета времени
		oneThreadDuration.emplace_back(end - start);
		
		cout << "Result of one thread calculation: " << oneThreadResult << endl;
		//cout << "Time of one thread calculation:   " << oneThreadDuration.back().count() << endl << endl;
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

	return 1;
}