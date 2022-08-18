#pragma once
#include<iostream>
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
	void printError(const std::string& message);

	enum class Color {
		BLACK = 0,
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_CYAN = 3,
		DARK_RED = 4,
		DARK_MAGENTA = 5,
		DARK_YELLOW = 6,
		DARK_WHITE = 7,
		BRIGHT_BLACK = 8,
		BRIGHT_BLUE = 9,
		BRIGHT_GREEN = 0xA,
		BRIGHT_CYAN = 0xB,
		BRIGHT_RED = 0xC,
		BRIGHT_MAGENTA = 0xD,
		BRIGHT_YELLOW = 0xE,
		WHITE = 0xF,
	};


	class ConsoleHandler {
	private:
		HANDLE mhandler;
		_CONSOLE_SCREEN_BUFFER_INFO minitialAttr;
		WORD mbgcolor = BACKGROUND_INTENSITY;
	public:
		ConsoleHandler();
		~ConsoleHandler();

		void setColor(WORD color) const;
		void setColor(WORD color, WORD bgcolor) const;
		void resetColor()const;
		void setColor(utils::Color color) const;
		void setColor(utils::Color color, utils::Color bgcolor) const;

	};


	using StringColorPair = std::pair<std::string, Color>;

	template<size_t size>
	inline 	void printColorful(const std::array <StringColorPair, size>& array) {
		ConsoleHandler handler;
		for (const auto& pair : array) {
			handler.setColor(pair.second);
			std::cout << pair.first;
		}
	}

};

