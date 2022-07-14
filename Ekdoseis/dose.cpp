#include<iostream>
#include<fstream>
#include<windows.h>//for fileapi.h
#include<fileapi.h>
#include"dose.h"
#define endl '\n'

using namespace std::filesystem;
using std::cout;
Dose::Dose(int n, const char* ptr[]) :margc{ n }, margv{ ptr } {

}
std::ostream& operator<<(std::ostream& out, const Dose& dose) {
	out << " command 'dose " << dose.margv[2] << "' ";

	return out;
}
#include "SHA1.h"
Dose& Dose::parseRootCommand() {
	const char* arg_path = margv[1];
	const char* arg_cmd = margv[2];
	/*
	for (int i = 3; i < margc; i++) {
		if (strncmp(margv[i], "-", 1) == 0) {
			if (i == 3)cout << endl << "options: ";
			cout << margv[i];
		}
	}
	cout << endl;
	*/
	if (strcmp(arg_cmd, "init") == 0) {
		mcommand = INIT;
	}
	else if (strcmp(arg_cmd, "add") == 0) {
		mcommand = ADD;
	}
	else {
		cout << "Error:" << *this << "doesnot exist" << endl;
		exit(EXIT_FAILURE);
	}
	mrootPath = path{ arg_path };
	return *this;
}
ReturnFlag Dose::createDirectory(const std::string_view& dirName, CreateFlag flags) {
	path newDir = mrootPath / dirName;
	if (exists(newDir)) {
		if (flags == NO_OVERRIDE) {
			return ALREADY_EXISTS;
		}
		else if (flags == OVERRIDE_IF_EXISTS) {
			if (!remove(newDir)) {
				return OVERRIDE_FAILURE;
			}//todo : create a new won now
			return OVERRIDE_SUCCESS;
		}
	}
	bool _success = create_directory(newDir);
	if (flags == NO_OVERRIDE) {
		return (_success ? CREATE_SUCCESS : CREATE_SUCCESS);
	}
	else	if (flags == OVERRIDE_IF_EXISTS) {
		return (_success ? OVERRIDE_SUCCESS : OVERRIDE_SUCCESS);
	}
	return CREATE_SUCCESS;
}
void Dose::errorExit() {
	cout << *this << "An error occured" << endl;
	exit(EXIT_FAILURE);
}

Dose& Dose::init() {
	//LINK:https://git-scm.com/docs/git-init
	cout << "initializing empty ekdoseis repo in " << mrootPath << endl;
	ReturnFlag _rflag = createDirectory(".dose");
	if (_rflag == ALREADY_EXISTS) {
		cout << "Ekdoseis is already initialized in " << mrootPath << endl;
		return *this;
	}
	if (_rflag == CREATE_FAILURE) {
		cout << "Error: couldnot initialize Ekdoseis repo on " << mrootPath << endl;
		exit(EXIT_FAILURE);
	}
	bool _s = SetFileAttributesA((mrootPath / ".dose").string().c_str(), FILE_ATTRIBUTE_HIDDEN);//for hiding the folder
	ReturnFlag _r1 = createDirectory(".dose/objects");
	if (_r1 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r2 = createDirectory(".dose/refs");
	if (_r2 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r3 = createDirectory(".dose/refs/heads");
	if (_r3 != CREATE_SUCCESS)errorExit();
	std::ofstream _headf{ (mrootPath / ".dose/refs/heads/main") };
	if (!(_headf))errorExit();
	std::ofstream headptr{ mrootPath / ".dose/HEAD" };
	if (!headptr)errorExit();
	headptr << "refs/heads/main" << endl;
	_headf.close();
	headptr.close();
	cout << "Successfully Initialized empty repo in" << *this << endl;

	return *this;
}
Dose& Dose::execCommand() {
	switch (mcommand) {
	case INIT:init(); break;
	case ADD:add(); break;
	}
	return *this;
}
Dose& Dose::add() {
	//check for git init
	if (!exists(mrootPath / ".dose/index")) {
		std::ofstream _headptr{ mrootPath / ".dose/index" };
		_headptr.close();
	}
	for (int i = 3; i < margc; i++) {
		if (i == 3) {
			doseIgnore = DoseIgnore(mrootPath);//we only need it now
			index = Index(mrootPath);

		}
		const path _filePath = mrootPath / margv[i];
		//if(isFile)//todo
		if (!exists(_filePath)) {
			cout << "Error: " << _filePath.string() << " doesnot exists." << endl;
			continue;
			//exit(EXIT_FAILURE);
		}
		if (doseIgnore.has(_filePath)) {
			cout << "Error: " << margv[i] << " already exists in the gitignore." << endl;
			continue;
			//exit(EXIT_FAILURE);

		}
		//todo: cout to cerr
		SHA1 fileSHA;
		const path objectPath = mrootPath / ".dose/objects";
		std::string hash{ fileSHA.from_file(_filePath.string()) };
		size_t   hashLength{ hash.length() };//read: size_t
		if (!hashLength == 40) {
			std::cerr << "Error: " << "Unwanted hash length" << endl;
		}
		const path hashDirPath = objectPath / hash.substr(0, 2);
		ReturnFlag _rf1 = createDirectory(hashDirPath.string());
		if (!(_rf1 == CREATE_SUCCESS || _rf1 == ALREADY_EXISTS)) {
			cout << "Error: " << "cannot perform the required action" << endl;
			exit(EXIT_FAILURE);
		}
		const path _hashFilePath{ hashDirPath / hash.substr(2,hashLength - 2) };
		std::error_code ec;

		copy_file(_filePath, _hashFilePath, copy_options::skip_existing, ec); //no encryption now maybe later
		if (ec) {
			cout << "Error: " << ec.message() << endl;
			cout << "Error: " << "cannot perform the required action" << endl;
			exit(EXIT_FAILURE);
		}
		cout << "File: " << _filePath << " staged successfully" << endl;
		index.add(_filePath, hash);

		//TODO:
		//add staged files to .dose/index LINK:
		//show corresponding commit message if the file is up to date
	}
	return *this;
}

DoseIgnore::DoseIgnore() = default;
DoseIgnore::DoseIgnore(const path & rootPath)
	:refToRootPath{ rootPath } {
	const path ignoreFile = rootPath / ".doseIgnore";
	if (exists(ignoreFile)) {
		std::ifstream ignoreptr{ ignoreFile };
		std::string _templine;
		size_t i{ 0 };
		while (std::getline(ignoreptr, _templine)) {
			if (_templine.length() != 0) {
				//ignorePaths[i++] = _templine;
				ignorePaths.push_back(_templine);
			}
		}
	}
}

bool DoseIgnore::has(const path & path)const {
	return pHas(path);
}
bool DoseIgnore::has(const std::string & pathstr)const {
	return pHas(path{ pathstr });
}
bool DoseIgnore::has(const char* pathstr)const {
	return pHas(path{ pathstr });
}

