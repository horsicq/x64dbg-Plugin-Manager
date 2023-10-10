//bsef21m028
//amna shahazadi
#include <iostream>
#include <string>
#include "stack.cpp"
using namespace std;


void nextGreaterElements(int a[], int n, int result[])
{
	for (int i = 0; i < n; i++)
	{
		result[i] = -1;
	}

	Stack<int> st;
	for (int i = 2 * n - 1; i >= 0; i--) {

		while (!st.isEmpty() && (a[i % n] >= st.Top())) {
			st.pop();
		}
		if (!st.isEmpty() && (i < n)) {
			result[i] = st.Top();
		}

		st.push(a[i % n]);
	}
}
int main() {
	int input1[] = { 3, 5, 2, 4 };

	int result1[4];

	nextGreaterElements(input1, 4, result1);


	for (int i = 0; i < 4; i++) {
		cout << result1[i] << " ";
	}

}


