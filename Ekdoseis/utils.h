#pragma once
#include<string>
#include<iostream>

using namespace std;
namespace utils {

	int getHex(char c);

	void toHexEncoding(std::string hash, unsigned char* ptr, int len = 20);

	std::string toAsciiEncoding(const std::string& hash);
	}
