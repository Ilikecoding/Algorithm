#include <iostream>
using namespace std;

/*a[]保存二进制数的位，每调用一次，二进制数加1*/
void increment(int a[], int n)
{
	int i = 0;
	while(i < n && a[i] == 1)
	{
		a[i] = 0;
		i++;
	}
	if(i < n)
	{
		a[i] = 1;
	}
}

/*测试程序*/
int main(int argc, char const *argv[])
{
	int a[5] = {0, 0, 0, 0, 0};

	for(int i = 0; i <= 15; i++)
	{
		increment(a, 5);
	}

	for(int i = 4; i >= 0; i--)
	{
		cout << a[i] << " ";
	}
	return 0;
}