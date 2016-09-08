#include <bits/stdc++.h>

using namespace std;

typedef int matrix[3][3];

class triple
{
public:
	int A;
	int B;
	int C;

	bool operator==(const triple& a)
	{
		return A == a.A && B == a.B && B == a.B;
	}

	triple(int, int, int);

	triple mult(matrix);
};

triple::triple(int s1, int s2, int s3)
{
	this->A = s1;
	this->B = s2;
	this->C = s3;
}

triple triple::mult(matrix X)
{
	int a1 = this->A;
	int a2 = this->B;
	int a3 = this->C;

	return triple(a1*X[0][0] + a2*X[1][0] + a3*X[2][0], a1*X[0][1] + a2*X[1][1] + a3*X[2][1], a1*X[0][2] + a2*X[1][2] + a3*X[2][2]);
}

matrix U = {
	{1, 2, 2},
	{-2, -1 , -2},
	{2, 2, 3}
};

matrix A = {
	{1, 2, 2},
	{2, 1, 2},
	{2, 2, 3}
};

matrix D = {
	{-1, -2, -2},
	{2, 1, 2},
	{2, 2, 3}
};

#define LIMIT 5000
#define EPS 0.0000001

bool is_pythag(long a, long b)
{
	long c = sqrt(a*a + b*b);
	return c*c == a*a + b*b;
}

int main()
{
	int result = 0;

	int j = 0;

	// for(int i = 1; i < LIMIT; i++)
	// {
		while(result < 1000000)
		{
			j++;
			//check if there is a pythagorean triple containing j as a leg
			for(int k = 2; k <= 2*j; k++)
			{
				if(is_pythag(j, k))
				{
					if(k <= j + 1)
						result += k/2;
					else
					{
						result += k/2 - (k - j) + 1;
					}
				}
			}

		}
	// }

	cout << result << " " << j <<  endl;
}