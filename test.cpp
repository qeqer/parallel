#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
using namespace std;

int main()
{

	
	auto sum = omp_get_wtime();
	for (int i = 0; i < 1000000000; i++)
	{
		i++;
		i--;
		i++;
		i--;
	}
	cout << "Without OMP: " <<  static_cast<double>(omp_get_wtime() - sum) << endl;

	auto sum2 = omp_get_wtime();
	#pragma omp parallel for num_threads(2)
	for (int i = 0; i < 1000000000; i++)
	{
		i++;
		i--;
		i++;
		i--;
	}
	cout << "With OMP: " <<  static_cast<double>(omp_get_wtime() - sum2) << endl;
	return 0;
}