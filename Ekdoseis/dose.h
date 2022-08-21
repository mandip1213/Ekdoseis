#pragma once
#ifndef DOSE_H
#define DOSE_H
#include<filesystem>
#include<string_view>
#include<string>

#include"enums.h"
#include"doseignore.h"
#include "index.h"

//using namespace std::filesystem;
namespace fs = std::filesystem;
//using namespace filesystem;
class Command;

class Dose {
private:
	fs::path mrootPath;
	DoseCommand mcommand;
	int margc;
	 char** margv;
	void errorExit();
	DoseIgnore doseIgnore;
	Index mindex;
	ReturnFlag createDirectory(const std::string_view& dirName, CreateFlag flags = NO_OVERRIDE);
	bool isBranch(const std::string& ch_point);
	bool isValidCommit(const std::string& ch_point);

	//default root directory is mrootPath all dirName should be passed relativ to that

public:
	//	Dose() = default;
	Dose(int n, char* ptr[]);
	friend	 std::ostream& operator<<(std::ostream& out, const Dose& dose);
	static void  updateHead(const fs::path& rootPath, const std::string& reference);
	Dose& parseRootCommand();
	Dose& execCommand();
	Dose& init();
	//Dose& commit();
	Dose& status();
	Dose& log();
	Command* getCommand();
	Dose& checkout();
	Dose& restore();
	Dose& branch();
	Dose& merge();
	friend	void parseCommand(int argc, char* argv[], std::vector<struct Token> tokenList);
};
#endif
