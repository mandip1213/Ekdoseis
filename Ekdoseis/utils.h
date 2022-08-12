#pragma once
#include<string>
#include<iostream>
#include<filesystem>
#include<windows.h>
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

	enum class Color {


	};
	class ConsoleHandler {
	private:
		HANDLE mhandler;
		_CONSOLE_SCREEN_BUFFER_INFO minitialAttr;
		WORD mbgcolor = BACKGROUND_INTENSITY;
	public:
		ConsoleHandler();

		void setColor(WORD color) const;
		void setColor(WORD color, WORD bgcolor) const;
		void resetColor()const;
	};
}
/*
 BLACK 	0
 DARK_RED 4
 DARK_GREEN █			2
 DARK_YELLOW █			6
 DARK_BLUE █			1
 DARK_MAGENTA █			5
 DARK_CYAN █			3
 DARK_WHITE █			7
 BRIGHT_BLACK █			8
 BRIGHT_RED █			C
 BRIGHT_GREEN █			A
 BRIGHT_YELLOW █			E
 BRIGHT_BLUE █			9
 BRIGHT_MAGENTA █			D
 BRIGHT_CYAN █			B
 WHITE █			F

*/
