/*
The result of this problem should be submitting compilable source code package (inf_int.h, inf_int.cpp, main.cpp, .sln files)

You have to effectively use dynamic memory allocation(new) and deallocation(delete)
for handling the variable sizes of infinite precision integer numbers.
*/


#include "inf_int.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

int main()
{
	while (true) {
		cout << "Input:  ";				// (positive integer)(space)(operator)(space)(positive integer)

		string pis1, pis2;
		char op;
		cin >> pis1;

		if (pis1 == "0")
			break;

		cin >> op;
		cin >> pis2;
		const char* pi1 = pis1.c_str();
		const char* pi2 = pis2.c_str();

		inf_int p1 = inf_int(pi1);
		inf_int p2 = inf_int(pi2);

		cout << "Output: ";
		switch (op) {
		case '+':
			cout << p1 + p2 << endl;
			break;
		case '-':
			cout << p1 - p2 << endl;
			break;
		case '*':
			cout << p1 * p2 << endl;
			break;
		}
	}

	return 0;
}
