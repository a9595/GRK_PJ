#include <iostream>
using namespace std;

int foo[] = { 16, 2, 77, 40, 12071 };
int n, result = 0;

struct MyMatrix {

	MyMatrix()
	{
		//matrix = { 0 };
	}
	void print()
	{
		cout << "hello";
	}


	int matrix[3][3] = { 0 };
};


int main()
{
	for (n = 0; n<5; ++n)
	{
		result += foo[n];
	}
	cout << result;

	MyMatrix matrix;
	matrix.print();
	return 0;
}