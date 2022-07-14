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

#include <filesystem>
#include <fstream>
#include<sys/stat.h>
#include<stdio.h>
#include <bitset>
namespace fs = std::filesystem;
/*(
int main() {
	struct _stat buf;
	int result;
	char timebuf[26];
	const char* filename = "main.cpp";
	errno_t err;
	result = _stat(filename, &buf);
	printf("File size     : %ld\n", buf.st_size);
	printf("Drive         : %c:\n", buf.st_dev + 'A');
	err = ctime_s(timebuf, 26, &buf.st_mtime);
	if (err)
	{
		printf("Invalid arguments to ctime_s.");
		exit(1);
	}
	printf("Time modified : %s", timebuf);


	return 0;

}*/

