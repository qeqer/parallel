#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<vector<float>> InitMatrix(uint row_num, uint col_num) 
{
	srand(time(0));
	vector<vector<float>> a(row_num, vector<float>(col_num));
	for (uint i = 0; i < row_num; i++) 
	{
		for (uint j = 0; j < col_num; j++) 
		{
			a[i][j] = rand() % 2;
		}
	}
	return a;
}
vector<vector<float>> FileInputMatr(const char *name)
{
	vector<vector<float>> a;
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
void FileOutMatr(vector<vector<T>> result) 
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
vector<vector<T>> MultMatr(vector<vector<T>> a, vector<vector<T>> b) {
	vector<vector<float>> c(a.size(), vector<float>(b[0].size()));
	if (a[0].size() != b.size())
	{
		cout << "I cann't multiply matrices, remember NxM and MxK = NxK" << endl;
		exit(0);
	}
	for (uint i = 0; i < a.size(); i++) 
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
	return c;
}

template<class T>
vector<vector<T>> MultMatrOpenMP(vector<vector<T>> a, vector<vector<T>> b) {
	vector<vector<float>> c(a.size(), vector<float>(b[0].size()));
	if (a[0].size() != b.size())
	{
		cout << "I cann't multiply matrices, remember NxM and MxK = NxK" << endl;
		exit(0);
	}
	#pragma omp parallel for 
		for (uint i = 0; i < a.size(); i++) 
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
	return c;
}

#define iter 1
template<class T>
void Test(vector<vector<T>> A, vector<vector<T>> B) //compute average 
//time by iter iterations
{
	auto sum = clock();
	for (uint i = 0; i < iter; i++)
	{
		MultMatr(A, B);
	}
	cout << "Without OMP: " <<  static_cast<double>(clock() - sum) / 
		iter / CLOCKS_PER_SEC << endl;
	
	sum = clock();
	for (uint i = 0; i < iter; i++)
	{
		MultMatrOpenMP(A, B);
	}
	cout << "With OMP: " <<  static_cast<double>(clock() - sum) / 
		iter / CLOCKS_PER_SEC << endl;
	

	return;
}



#define size_n 1000
#define size_m 1000
#define size_k 100
int main(int argc, char **argv)
{
	char ans;
	vector<vector<float>> A, B;
	if (argc == 3)
	{
		A = FileInputMatr(argv[1]);
		B = FileInputMatr(argv[2]);
	} else {
		A = InitMatrix(size_n, size_m);
		B = InitMatrix(size_m, size_k);		
	}
	Test(A, B);
	//FileOutMatr(result);
	return 0;
}