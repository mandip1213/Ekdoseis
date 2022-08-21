#pragma once
#include"command.h"

class Restore :public Command {
public:
	Restore(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		//0 command 1 location 2 root command

		if (strcmp(margv[3].c_str(), "--staged") == 0) {
			//unstage file from staging area
			mindex = Index(mrootPath);
			mindex.fetchFromIndex();
			for (int i = 4; i < margc; i++) {
				const fs::path _path = mrootPath / margv[i];
				mindex.restoreFile(mrootPath / margv[i], true);
			}
		}
		else {
			//restore the modified content of the file to that of latest commit
			//TODO
			mindex = Index(mrootPath);
			mindex.fetchFromIndex();
			for (int i = 3; i < margc; i++) {
				mindex.restoreFile(mrootPath / margv[i], false);
			}
		}
	}

};
