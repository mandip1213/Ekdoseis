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
	dose.parseRootCommand().execCommand();
	return 0;
}


//don't go below this
#include <filesystem>
#include <fstream>
#include<sys/stat.h>
#include<stdio.h>
#include <bitset>
namespace fs = std::filesystem;

int _main() {
	std::string name{"mandip"};
	cout << name.size()<<endl;
	cout << name.length()<<endl;
	for (auto ch : name) {
		//cout << i<<" "<<ch << endl;
	}
	return 0;


}
