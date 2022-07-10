#include<iostream>
#include<fstream>
#include<windows.h>//for fileapi.h
#include<fileapi.h>
#include"dose.h"
#define endl '\n'

using std::cout;
Dose::Dose(int n, const char* ptr[]) :margc{ n }, margv{ ptr } {

}
std::ostream& operator<<(std::ostream& out, const Dose& dose) {
	out << " command 'dose " << dose.margv[2] << "' ";

	return out;
}

Dose& Dose::parse() {
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
			}
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
	headptr<< "refs/heads/main" << endl;
	_headf.close();
	headptr.close();
	cout << "Successfully Initialized empty repo in" << *this << endl;

	return *this;
}
Dose& Dose::execCommand() {
	if (mcommand == INIT) {
		init();
	}
	return *this;

}
