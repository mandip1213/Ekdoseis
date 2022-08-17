#include <iostream>
#include"dose.h"

#define endl '\n'
using std::cout;
int main(int argc, const char* argv[])
{
	if (argc < 3) {
		cout << "error: not enough argument" << endl;
		//return EXIT_SUCCESS;
	}
	if (argc > 10) {
		cout << "Too many arguments" << endl;
		//return EXIT_SUCCESS;
	}
	//std::error_code ec;
	//fs::path p1{ "C:\\Users\\mandip\\Desktop\\git\\a\\c\\chello\ -\ Copy.txt" };
	//fs::path p2{ " C:\\Users\\mandip\\Desktop\\git\\a\\c\\chello\ -\ Copy.txt" };
	//bool b = fs::equivalent(p1, p2,ec);
	//if (ec) {
	//	cout << ec.message() << endl;
	//	cout << ec.value() << endl;
	//}
	//cout << std::boolalpha << b << endl;
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
#include<chrono>
#include<ctime>
#include<iomanip>
namespace fs = std::filesystem;

int ain() {
	//1658150019017
	//1658145036292

	auto currTime = std::chrono::system_clock::now();
	const std::time_t t_c = std::chrono::system_clock::to_time_t(currTime);
	//const std::time_t t_c{ 1658152104342    };
	//const std::time_t t_c{ 1658150019019   };
	//const std::time_t t_c{ 1658151446099    };
	std::tm t;
	gmtime_s(&t, &t_c);
	localtime_s(&t, &t_c);
	//LINK:https://en.cppreference.com/w/cpp/io/manip/put_time
	cout << std::put_time(&t, "%a %b %d %Y %H:%M:%S %xz %Z") << endl;

	using namespace std::chrono;
	uint64_t ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()).count();
	cout << t_c << endl;

	/*
	uint64_t time = 1658145036292;
	const std::time_t t_c{ static_cast<long long>(time) };
	std::tm t;
	//gmtime_s(&t, &t_c); for gmtime
	localtime_s(&t, &t_c);
	//LINK:https://en.cppreference.com/w/cpp/io/manip/put_time
	std::cout << std::put_time(&t, "%a %b %d %Y %H:%M:%S %z %Z");
	*/

	return 0;
}
