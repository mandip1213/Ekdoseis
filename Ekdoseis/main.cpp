#include <iostream>
#include"dose.h"

#define endl '\n'
using std::cout;
int main(int argc, const char* argv[])
{
	if (argc < 3) {
		cout << "error: not enough argument" << endl;
		return EXIT_FAILURE;
	}
	if (argc > 10) {
		cout << "Too many arguments" << endl;
		return EXIT_FAILURE;
	}
	Dose dose{ argc,argv };
	dose.parse().execCommand();
	return 0;
}
