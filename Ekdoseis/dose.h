#pragma once
#include<filesystem>
#include<string_view>
#include<string>

#include "parser.h"

using namespace std::filesystem;
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
	NOCOMMAND,

};
class DoseIgnore;
class DoseIgnore {
private:
	std::vector<std::string> ignorePaths;
	template<typename T>
	bool pHas(const T& path)const {
		for (std::string current : ignorePaths) {
			if (current.compare(path) == 0) {
				return true;
			}
		}
		return false;
	}
public:
	DoseIgnore();
	DoseIgnore(const path& rootPath);
	bool has(const path& path)const;
	bool has(const std::string& path)const;
	bool has(const char* path)const;
};
class Dose {
private:
	path mrootPath;
	DoseCommand mcommand;
	int margc;
	const char** margv;
	void errorExit();
	DoseIgnore doseIgnore;
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
	friend	void parseCommand(int argc, char* argv[], std::vector<struct Token> tokenList);

};
