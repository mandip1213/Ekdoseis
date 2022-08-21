#include <iostream>
#include"dose.h"
#include"command.h"

#define endl '\n'
using std::cout;
int main(int argc, char* argv[])
{
	if (argc < 3) {
		cout << "error: not enough argument" << endl;
		return 0;
	}
	if (argc > 10) {
		cout << "Too many arguments" << endl;
		return 0;
	}
	Dose dose{ argc,argv };

	//dose.parseRootCommand().execCommand();

	Command* command = dose.getCommand();
	command->runCommand();

	delete command;

	return 0;
}

