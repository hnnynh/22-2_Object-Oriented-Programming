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
	inf_int a, e;
	inf_int b(101);
	inf_int c("321111111111122222222222233333333333444444444445555555555");
	inf_int d("123451987651234572749499923455022211");
	inf_int f = d;
	inf_int g(f);

	a = b * c;
	// e=g/f;       // not required

	b = c - d;

	if (f == d) {
		cout << "a : " << a << endl;
		cout << "b : " << b << endl;
		cout << "c : " << c << endl;
		cout << "d : " << d << endl;
		// cout << "e : " << e << endl;
		cout << "f : " << f << endl;
		// cout << "g : " << g << endl;
	}
}


/*
	inf_int a, e;
	inf_int b(101);
	inf_int c("321111111111122222222222233333333333444444444445555555555");
	inf_int d("123451987651234572749499923455022211");
	inf_int f = d;
	inf_int g(f);

	a = b * c;
	// e=g/f;       // not required

	b = c - d;

	if (f == d) {
		cout << "a : " << a << endl;
		cout << "b : " << b << endl;
		cout << "c : " << c << endl;
		cout << "d : " << d << endl;
		// cout << "e : " << e << endl;
		cout << "f : " << f << endl;
		// cout << "g : " << g << endl;
	}

*/
/*
	inf_int a;
	inf_int b(-101);
	inf_int c("-321111111111122222222222233333333333444444444445555555555");
	inf_int d("-123451987651234572749499923455022211");

	a = c + b;

	cout << a << endl;
	cout << (c < d) << endl;
	cout << (b < c) << endl;
	cout << c * b << endl;
	cout << a - d << endl;
	cout << a + d << endl;

	return 0;
*/
/*
	inf_int a;
	inf_int b(-101);
	inf_int c("321111111111122222222222233333333333444444444445555555555");
	inf_int d("-123451987651234572749499923455022211");

	a = c - b;

	cout << a << endl;
	cout << c * b << endl;
	cout << c * d << endl;
	cout << a - d << endl;
	cout << a + d << endl;

	return 0;
*/
/*
	inf_int a(1004);
	inf_int b(-101);
	inf_int c("321111111111122222222222233333333333444444444445555555555");
	inf_int d("-123451987651234572749499923455022211");


	cout << a + c << endl;					// + + +
	cout << a + b << endl;					// + + -
	cout << d + a << endl;					// - + +
	cout << b + d << endl;					// - + -
	cout << a - c << endl;					// + - +
	cout << a - b << endl;					// + - -
	cout << d - c << endl;					// - - +
	cout << b - d << endl;					// - - -
	cout << a * c << endl;					// + * +
	cout << a * d << endl;					// + * -
	cout << b * c << endl;					// - * +
	cout << d * b << endl;					// - * -


	return 0;
*/