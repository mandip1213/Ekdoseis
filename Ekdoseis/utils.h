#pragma once
#include<string>
#include<iostream>
#include<filesystem>
#include"enums.h"

namespace fs = std::filesystem;
namespace utils {

	int getHex(char c);

	void toHexEncoding(std::string hash, unsigned char* ptr, int len = 20);

	std::string toAsciiEncoding(const std::string& hash);
	//ReturnFlag createObjectDir(const fs::path& newDir,CreateFlagI/
	ReturnFlag createObjectDir(const fs::path& newDir, CreateFlag flag = CreateFlag::NO_OVERRIDE);

	bool validateHash(const std::string& hash);
	uint64_t getTime();
	void printDate(uint64_t time);
}

