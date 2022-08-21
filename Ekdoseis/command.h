#pragma once
#include<vector>
#include<array>
#include<sstream>
#include<algorithm>
#include<iomanip>
#include<string>

#include"dose.h"
#include"doseignore.h"
#include"index.h"
#include"commit.h"
#include"tree.h"
#include"utils.h"
#include"enums.h"
#include"branch.h"

using std::cout;

class Command {
protected:
	int margc;
	std::vector<std::string>margv;
	fs::path mrootPath;
	DoseIgnore doseIgnore;
	Index mindex;

public:
	Command(int argc, char* argv[]) :
		margc{ argc }
	{
		for (int i = 0; i < argc; i++) {
			margv.push_back(argv[i]);
		}
		mrootPath = margv[1];

	}
	virtual void runCommand() = 0;



	ReturnFlag createDirectory(const std::string_view& dirName, CreateFlag flags = NO_OVERRIDE) {
		fs::path newDir = mrootPath / dirName;//bug?
		if (fs::exists(newDir)) {
			if (flags == NO_OVERRIDE) {
				return ALREADY_EXISTS;
			}
			else if (flags == OVERRIDE_IF_EXISTS) {
				if (!remove(newDir)) {
					return OVERRIDE_FAILURE;
				}
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



	bool isValidCommit(const std::string& ch_point) {
		if (ch_point.size() != 40)
			return false;
		const fs::path dirPath = mrootPath / ".dose/objects" / ch_point.substr(0, 2);
		const fs::path requiredPath = dirPath / ch_point.substr(2, 40 - 2);
		if (fs::exists(requiredPath)) {
			return true;
		}
		/*
		using iterator = fs::recursive_directory_iterator;
		for (iterator i = fs::recursive_directory_iterator(mrootPath / ".dose/objects");
			i != fs::recursive_directory_iterator();
			++i) {
			//refactor into normal iterator
			fs::path tmp = i->path();
			std::error_code ec;
			if (!fs::equivalent(i->path(), dirPath, ec)) {
				i.disable_recursion_pending();
			}
			if (fs::equivalent(i->path(), requiredPath, ec)) {
				cout << "found the checkout point";
				//sutffs
				return true;
			}
		}
		*/
		return false;
	}


	void errorExit() {
		cout << *this << "An error occured" << endl;
		exit(EXIT_SUCCESS);
	}

	friend std::ostream& operator<<(std::ostream& out, const Command& command) {
		out << " command 'dose " << command.margv[2] << "' ";
		return out;
	}
};

