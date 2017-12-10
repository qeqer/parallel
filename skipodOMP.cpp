#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;
vector<vector<float> > InitMatrix(uint row_num, uint col_num) 
{
	srand(time(0));
	vector<vector<float> > a(row_num, vector<float>(col_num));
	for (uint i = 0; i < row_num; i++) 
	{
		for (uint j = 0; j < col_num; j++) 
		{
			a[i][j] = rand() % 2;
		}
	}
	return a;
}
vector<vector<float> > FileInputMatr(const char *name)
{
	vector<vector<float> > a;
	ifstream file(name);
	int i = 0;
	float d;
	bool check = 1;
	vector<float> temp;
	while (file >> d)
	{
		if (check)
		{
			temp.push_back(d);
		} else {
			temp[i] = d;
			++i;
		}
		if (file.peek() == '\n' || file.peek() == EOF) 
		{
			i = 0;
			check = 0;
			a.push_back(temp);

		}
		if (file.peek() == EOF) {
			break;
		}
	}
	return a;
}
template<class T>
void FileOutMatr(vector<vector<T> > result) 
{
	ofstream o("out");
	for (uint i = 0; i < result.size(); i++) 
	{
		for (uint j = 0; j < result[0].size(); j++)
		{
			o << result[i][j] << " ";
		}
		o << endl;
	}
	return;
}

template<class T>
vector<vector<T> > MultMatrOpenMP(vector<vector<T> > a, 
	vector<vector<T> > b, uint thread) {
	vector<vector<float> > c(a.size(), vector<float>(b[0].size()));
	if (a[0].size() != b.size())
	{
		cout << "I cann't multiply matrices, remember NxM and MxK = NxK" << endl;
		exit(0);
	}
	double sum = omp_get_wtime();
	for (int i = 0; i < 100; i++)
	#pragma omp parallel for num_threads(thread) if (thread)
		for (int i = 0; i < a.size(); i++) 
		{
			for (uint j = 0; j < b[0].size(); j++)
			{
				float temp = 0;
				for (uint k = 0; k < b.size(); k++)
				{
					temp += a[i][k] * b[k][j];
				}
				c[i][j] = temp;
			}
		}
	cout << "With " << thread << " threads: " <<  
		static_cast<double>((omp_get_wtime() - sum) / 100) << endl;
	return c;
}

template<class T>
void Test(vector<vector<T> > A, vector<vector<T> > B, uint thread) //compute average time
{
	cout << "Testing Multiplying Matrices of sizes: " << A.size() << " "
		<< A[0].size() << " " << B.size() << " " << B[0].size() << endl;
	MultMatrOpenMP(A, B, thread);
	return;
}

template<class T>
void TestThreads(vector<vector<T> > A, vector<vector<T> > B) {
	Test(A, B, 0);	
	Test(A, B, 1);	
	Test(A, B, 2);	
	Test(A, B, 4);
	Test(A, B, 8);	
	Test(A, B, 16);
}


int main(int argc, char **argv)
{
	uint size_n = 100, size_m = 100, size_k = 1;
	char ans;
	vector<vector<float> > A, B;
	if (argc == 4) //u should enter 2 file names with matricies and thread_num
		//or 3 sizes and thread_num or nothing
	{
		A = FileInputMatr(argv[1]);
		B = FileInputMatr(argv[2]);
	} else if (argc == 5) 
		{
			sscanf(argv[1], "%u", &size_n);
			sscanf(argv[2], "%u", &size_m);
			sscanf(argv[3], "%u", &size_k);
			A = InitMatrix(size_n, size_m);
			B = InitMatrix(size_m, size_k);		
		} else {
			A = InitMatrix(size_n, size_m);
			B = InitMatrix(size_m, size_k);
		}
	uint i = 0;
	if (argc >= 2) 
	{
		sscanf(argv[4], "%u", &i);
	}
	TestThreads(A, B);
	//FileOutMatr(MultMatrOpenMP(A, B));
	return 0;
}
