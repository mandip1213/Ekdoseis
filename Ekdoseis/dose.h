#pragma once
#include<filesystem>
#include<string_view>
#include<string>
#include "index.h"
#include "parser.h"
#include "doseignore.h"
#include "enums.h"

//using namespace std::filesystem;
namespace fs = std::filesystem;
//using namespace filesystem;

class Dose {
	private:
	fs::path mrootPath;
	DoseCommand mcommand;
	int margc;
	const char** margv;
	void errorExit();
	DoseIgnore doseIgnore;
	Index mindex;
	std::vector<struct parser::Token> tokenList;
	ReturnFlag createDirectory(const std::string_view& dirName, CreateFlag flags = NO_OVERRIDE);

	//default root directory is mrootPath all dirName should be passed relativ to that

public:
	//	Dose() = default;
	Dose(int n, const  char* ptr[]);
	friend	 std::ostream& operator<<(std::ostream& out, const Dose& dose);
	Dose& parseRootCommand();
	Dose& execCommand();
	Dose& init();
	Dose& add();
	Dose& commit();
	Dose& status();
	Dose& log();
	friend	void parseCommand(int argc, char* argv[], std::vector<struct Token> tokenList);
};
