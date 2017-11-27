#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
using namespace std;

int main()
{
	#pragma omp parallel for
	for (int i = 0; i < 1000; i++)
	{
		cout << i << " ";
	}
	return 0;
}