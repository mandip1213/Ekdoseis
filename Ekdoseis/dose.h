#pragma once
#include<filesystem>
#include<string_view>

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
	NOCOMMAND,

};
class Dose {
private:
	path mrootPath;
	DoseCommand mcommand;
	int margc;
	const char** margv;
	void errorExit();
	ReturnFlag createDirectory(const std::string_view& dirName, CreateFlag flags = NO_OVERRIDE);
	//default root directory is mrootPath all dirName should be passed relativ to that

public:
	//	Dose() = default;

	Dose(int n, const  char* ptr[]);
	friend	 std::ostream& operator<<(std::ostream& out, const Dose& dose);
	Dose& parse();
	Dose& execCommand();
	Dose& init();
};;