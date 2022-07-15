#pragma once
#include<filesystem>
#include<string_view>
#include<string>

#include "parser.h"
#include "index.h"
#include "doseignore.h"

//using namespace std::filesystem;
namespace fs = std::filesystem;
//using namespace filesystem;

enum ReturnFlag {
	CREATE_SUCCESS,
	CREATE_FAILURE,
	ALREADY_EXISTS,
	OVERRIDE_FAILURE,
	OVERRIDE_SUCCESS,
};
enum CreateFlag {
	NO_OVERRIDE,//default
	OVERRIDE_IF_EXISTS,
};
enum DoseCommand {
	INIT,
	ADD,
	COMMIT,
	NOCOMMAND,

};
class Dose {
private:
	fs::path mrootPath;
	DoseCommand mcommand;
	int margc;
	const char** margv;
	void errorExit();
	DoseIgnore doseIgnore;
	Index index;
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
	friend	void parseCommand(int argc, char* argv[], std::vector<struct Token> tokenList);

};
