#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>


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

int ceil(int a, int b) { //like in math.h
	if (a % b == 0) {
		return a / b;
	} else {
		return a / b + 1;
	}
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

vector<vector<float> > MultMatrMPI(vector<vector<float> > av, 
	vector<vector<float> > bv, uint iter_num) {
	
	int myid, numprocs;
	double sum = 0.0;
	double temp;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid); 
	int n, m;
	if (myid == 0) {
		n = av.size();
		m = av[0].size();
		if (m != bv.size()) {
			cout << "M!=M"<< endl;
			exit(1);
		}		
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//I don't know how to work with vectors in MPI(nobody knows, when MPI was created, there was no vectors
	// so this ...
	int local_n_normalized = int(ceil(n, numprocs)); //num of lines for each process
	int n_normalized = local_n_normalized * numprocs;
	float *local_a = (float *) calloc(local_n_normalized * m, sizeof(float));
	float *a = (float *) calloc(n_normalized * m, sizeof(float));
	float *b = (float *) calloc(m, sizeof(float));
	float *c = (float *) calloc(n_normalized, sizeof(float));
	float *local_c = (float *) calloc(n_normalized, sizeof(float));


	if (myid == 0) {
		for (int j = 0; j < m; j++) {
			for (int i = 0; i < n; i++) {
				a[i * m + j] = av[i][j];
			}
			b[j] = bv[j][0];
		}
	}
	if (myid == 0) { //suppose that we have arrays from the begining, 
		//so don't count vector transformation time
		temp = MPI_Wtime();
	}

	for (int oo = 0; oo < iter_num; oo++) {
		MPI_Bcast(b, m, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Scatter(a, m * local_n_normalized, MPI_FLOAT, 
			local_a, m * local_n_normalized, MPI_FLOAT, 
			0, MPI_COMM_WORLD);
		for (int i = 0; i < local_n_normalized; i++) 
		{
			float temp = 0;
			if (local_n_normalized * myid + i >= n) {
				break;
			}
			for (int k = 0; k < m; k++)
			{
				temp += local_a[i * m + k] * b[k];
			}
			local_c[i] = temp;
		}

		MPI_Gather(local_c, local_n_normalized, MPI_FLOAT,
			c, local_n_normalized, MPI_FLOAT,
			0, MPI_COMM_WORLD);
	}


	if (myid == 0) {
		temp = (MPI_Wtime() - temp) / iter_num;
	}
	vector<vector<float> > cv;
	if (myid == 0) {
		cout << "With " << numprocs << " procs: " <<  
			static_cast<double>(temp) << endl;
		cv = vector<vector<float> >(n, vector<float> (1, 0));
		for (int i = 0; i < n; i++) {
			cv[i][0] = c[i];
		}
	}

	
	return cv;
}

void Test() //compute average time
{
	vector<vector<float> >A, B;
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	A = InitMatrix(100, 100);
	B = InitMatrix(100, 1);
	if (myid == 0) {
		cout << "Testing Multiplying Matrices of sizes: " << A.size() << " "
			<< A[0].size() << " " << B.size() << " " << B[0].size() << endl;
	}
	MultMatrMPI(A, B, 100000);

	A = InitMatrix(1000, 1000);
	B = InitMatrix(1000, 1);
	if (myid == 0) {
		cout << "Testing Multiplying Matrices of sizes: " << A.size() << " "
			<< A[0].size() << " " << B.size() << " " << B[0].size() << endl;
	}
	MultMatrMPI(A, B, 1000);

	A = InitMatrix(10000, 1000);
	B = InitMatrix(1000, 1);
	if (myid == 0) {
		cout << "Testing Multiplying Matrices of sizes: " << A.size() << " "
			<< A[0].size() << " " << B.size() << " " << B[0].size() << endl;
	}
	MultMatrMPI(A, B, 100);

	A = InitMatrix(10000, 10000);
	B = InitMatrix(10000, 1);
	if (myid == 0) {
		cout << "Testing Multiplying Matrices of sizes: " << A.size() << " "
			<< A[0].size() << " " << B.size() << " " << B[0].size() << endl;
	}
	MultMatrMPI(A, B, 10);

	return;
}


int main(int argc, char **argv)
{
	uint size_n = 100, size_m = 100, size_k = 1;
	char ans;
	MPI_Init(&argc, &argv);
	int myid;
	vector<vector<float> > A, B;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (myid == 0) {
		if (argc == 3) //u should enter 2 file names with matricies
			//or 3 sizes or nothing
		{
			A = FileInputMatr(argv[1]);
			B = FileInputMatr(argv[2]);
		} else if (argc == 4) {
			sscanf(argv[1], "%u", &size_n);
			sscanf(argv[2], "%u", &size_m);
			sscanf(argv[3], "%u", &size_k);
			A = InitMatrix(size_n, size_m);
			B = InitMatrix(size_m, size_k);		
		} else {
			A = InitMatrix(size_n, size_m);
			B = InitMatrix(size_m, size_k);
		}
	}

	Test();
	//vector<vector<float> > ll = MultMatrMPI(A, B);
	
	// if (myid == 0) {
	// 	for (int i = 0; i < ll.size(); i++) {
	// 		cout << ll[i][0] << " ";
	// 	}
	// }
	MPI_Finalize();
	return 0;
}
