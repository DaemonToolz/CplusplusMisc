// MiscTester.cpp : main project file.

#include "stdafx.h"
#include <iostream>
#include <chrono>
using namespace std;
using namespace System;
using namespace Misc::Math;
using namespace chrono;
int main()
{
	Misc::Math::XMath^ math = gcnew XMath();
	time_point<high_resolution_clock> start, end;

	start = high_resolution_clock::now();
	cout << " + " << math->bitwise_add(4, 2) 
			<< "  - " << math->bitwise_sub(4, 2) 
			<< "  * " << math->bitwise_mul(4, 2) 
			<< "  / " << math->bitwise_div(4, 2) 
			<< endl;

	cout << " *2 " << math->bitwise_mul_2(4) 
			<< "  /2 " << math->bitwise_div_2(4) 
			<< endl;

	end = high_resolution_clock::now();

	long long duration = duration_cast<nanoseconds>(end - start).count();
	cout << " All bitwise operations took " << duration << " ns" << endl;
	start = high_resolution_clock::now();
	cout << " + " << 4+2
		<< "  - " << 4-2
		<< "  * " << 4*2
		<< "  / " << 4/2
		<< endl;
	cout << " *2 " << 4*2
		<< "  /2 " << 4/2
		<< endl;
	end = high_resolution_clock::now();
	duration = duration_cast<nanoseconds>(end - start).count();
	cout << " All normal operations took " << duration << " ns" << endl;

	getchar();
    return 0;
}
