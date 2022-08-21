#pragma once
#include "command.h"



class Init :public Command {
public:
	Init(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		const std::string defaultBranchName = "main";
		//LINK:https://git-scm.com/docs/git-init

		const std::array<utils::StringColorPair, 2> arr{ {
				{"Initializing empty ekdoseis repo in", utils::Color::BRIGHT_YELLOW},
				{" '" + mrootPath.string() + "' \n",utils::Color::BRIGHT_GREEN},
				} };
		utils::printColorful(arr);

		ReturnFlag _rflag = createDirectory(".dose");
		if (_rflag == ALREADY_EXISTS) {
			const std::array<utils::StringColorPair, 2> arr{ {
					{"Ekdoesis is already initialized in", utils::Color::BRIGHT_YELLOW},
					{" '" + mrootPath.string() + "' \n",utils::Color::BRIGHT_GREEN},
					} };
			utils::printColorful(arr);
			return;
		}

		if (_rflag == CREATE_FAILURE) {
			const std::array<utils::StringColorPair, 2> arr{ {
					{"Error: couldnot initialize Ekdoseis repo on ", utils::Color::BRIGHT_RED},
					{" '" + mrootPath.string() + "' \n",utils::Color::BRIGHT_GREEN},
					} };
			utils::printColorful(arr);
			exit(EXIT_SUCCESS);
		}

		bool _s = SetFileAttributesA((mrootPath / ".dose").string().c_str(), FILE_ATTRIBUTE_HIDDEN);//for hiding the folder
		ReturnFlag _r1 = createDirectory(".dose/objects");
		if (_r1 != CREATE_SUCCESS)errorExit();
		ReturnFlag _r2 = createDirectory(".dose/refs");
		if (_r2 != CREATE_SUCCESS)errorExit();
		ReturnFlag _r3 = createDirectory(".dose/refs/heads");
		if (_r3 != CREATE_SUCCESS)errorExit();
		std::ofstream _headf{ (mrootPath / ".dose/refs/heads" / defaultBranchName) };
		if (!(_headf))errorExit();
		std::ofstream headptr{ mrootPath / ".dose/HEAD" };
		if (!headptr)errorExit();
		headptr << "ref: refs/heads/" << defaultBranchName << endl;
		_headf.close();
		headptr.close();

		const std::array<utils::StringColorPair, 2> arrr{ {
				{"Successfully Initialized empty repo in", utils::Color::BRIGHT_YELLOW},
				{" '" + mrootPath.string() + "' \n",utils::Color::BRIGHT_GREEN},
				} };
		utils::printColorful(arrr);
	};

};


class  Status :public Command {
public:
	Status(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		using iterator = fs::recursive_directory_iterator;
		//for (auto& curr : fs::recursive_directory_iterator(p)) {
			//cout << curr << endl;
		//}
		doseIgnore = DoseIgnore(mrootPath);//we only need it now
		mindex = Index(mrootPath);
		mindex.fetchFromIndex();
		std::vector<fs::path> modified;
		std::vector<fs::path> staged;
		std::vector<fs::path>untracked;
		std::vector<fs::path>committed;
		std::string symbol("|___");
		for (iterator i = fs::recursive_directory_iterator(mrootPath);
			i != fs::recursive_directory_iterator();
			++i) {
			fs::path p{ i->path() };
			if (doseIgnore.has(p)) {
				i.disable_recursion_pending();
				continue;
			}
			if (fs::equivalent(p, mrootPath / ".dose")) {
				//cout << "Dose found" << endl;
				i.disable_recursion_pending();
				continue;
			}
			if (fs::exists(mrootPath / ".git")) {

				if (fs::equivalent(p, mrootPath / ".git")) {
					//cout << "Dose found" << endl;
					i.disable_recursion_pending();
					continue;
				}
			}
			if (fs::is_directory(p)) {
				continue;
			}

			using namespace index;
			index::FileStatus status = mindex.getFileStatus(i->path());
			if (status == FileStatus::STAGED) {
				staged.push_back(fs::relative(p, mrootPath));
			}
			else if (status == FileStatus::COMMITTED) {
				committed.push_back(fs::relative(p, mrootPath));
			}
			else if (status == FileStatus::UNTRACKED) {
				untracked.push_back(fs::relative(p, mrootPath));
			}
			else if (status == FileStatus::MODIFIED) {
				modified.push_back(fs::relative(p, mrootPath));
			};

			if (i.depth() != 0) {
				//std::cout << std::string((i.depth() - 1) * 4, ' ');
				// std::cout << symbol;
			}
			//std::cout << p.filename() << "    " << endl;
			//cout << i->path()<<endl;
		}
		utils::ConsoleHandler handler;
		cout << endl << endl;
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << "committed: " << endl;
		handler.setColor(utils::Color::BRIGHT_YELLOW);
		for (auto it : committed) {
			std::cout << it << "    " << endl;
		}
		cout << endl << endl;
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << "staged: " << endl;
		handler.setColor(utils::Color::BRIGHT_YELLOW);
		for (auto it : staged) {
			std::cout << it << "    " << endl;
		}
		cout << endl << endl;
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << "modified: " << endl;
		handler.setColor(utils::Color::BRIGHT_YELLOW);
		for (auto it : modified) {
			std::cout << it << "    " << endl;
		}
		cout << endl << endl;
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << "untracked: " << endl;
		handler.setColor(utils::Color::BRIGHT_YELLOW);
		for (auto it : untracked) {
			std::cout << it << "    " << endl;
		}
		handler.resetColor();


	}

};

class BranchAction :public Command {
public:
	BranchAction(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		//if(margc>5)
		if (margc == 3 || strcmp(margv[3].c_str(), "--list") == 0 || strcmp(margv[3].c_str(), "-l") == 0 && margc < 5) {
			Branch::listBranch(mrootPath);

		}
		else if (strcmp(margv[3].c_str(), "--delete") == 0 || strcmp(margv[3].c_str(), "-d") == 0) {
			if (margc == 4) {
				utils::printError("ERROR: branch name is required\n");
			}
			for (int i = 4; i < margc; i++) {
				Branch branch{ mrootPath,margv[i] };
				branch.deleteBranch();
			}
		}
		else if (!margv[3].starts_with("-")) {
			if (margc > 5) {
				//exit
				exit(EXIT_SUCCESS);
			}
			const std::string branchName = margv[3];
			const bool isValidBranchName = std::all_of(branchName.begin(), branchName.end(), [](char c) {
				return std::isalpha(c);
				});
			if (!isValidBranchName) {
				const std::array<utils::StringColorPair, 2> arr{ {
						{" '" + branchName + "' ",utils::Color::BRIGHT_GREEN},
						{"is not a valid branch name\n", utils::Color::BRIGHT_YELLOW}
						} };
				utils::printColorful(arr);
				exit(EXIT_SUCCESS);
			}
			Branch branch{ mrootPath, branchName };
			if (margc == 4)
				branch.createBranch();
			else  if (margc == 5)
				branch.createBranch(margv[4]);
			else {

			}
		}
		else {
			const std::array<utils::StringColorPair, 2> arr{ {
				{"ERROR: Illegal Argument: ", utils::Color::BRIGHT_RED},
				{std::string{"'"} + margv[3] + "'",utils::Color::BRIGHT_YELLOW},
				} };
			utils::printColorful(arr);
		}
	};
};



class MergeAction :public Command {
public:
	MergeAction(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		if (margc != 4) {
			utils::printError("Invalid args\n");
			exit(EXIT_SUCCESS);
		}
		if (Branch::isBranch(mrootPath, margv[3])) {
			const  std::string commitObj = Branch::getCommitFromBranch(mrootPath, margv[3]);
			if (isValidCommit(commitObj)) {
				Merge merge{ mrootPath,commitObj };
				merge.merge();
			}
		}
		else if (this->isValidCommit(margv[3])) {
			Merge merge{ mrootPath,margv[3] };
			merge.merge();
		}
		else {
			const std::array<utils::StringColorPair, 3> arr{ {
				{"ERROR: cannot merge ", utils::Color::BRIGHT_RED},
				{std::string{"'"} + margv[3] + "'. ",utils::Color::BRIGHT_YELLOW},
				{"Invalid Reference.", utils::Color::BRIGHT_RED}
				} };
			utils::printColorful(arr);
		}
	};
};


class Log :public Command {
public:
	Log(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override {
		/*
		- todo: implement for detached head state
		*/

		std::ifstream headfptr{ mrootPath / ".dose/HEAD" };
		std::string reference;
		std::getline(headfptr, reference);
		if (reference.starts_with("ref: refs/heads/")) {
			reference.erase(0, std::string("ref: refs/heads/").size());
		}
		else {
			/* HEAD is in detahced state */
			cout << "HEAD is in detached state " << endl;
			/*MUST change the way of logging*/
		}
		std::ifstream logFile{ mrootPath / ".dose/logs/refs" / reference };
		if (!logFile) {
			cout << "NO logs";
		}

		std::string line;
		std::string hash, author, message;
		uint64_t time;//time are stroed in 

		utils::ConsoleHandler handler;
		while (std::getline(logFile, line)) {
			std::istringstream stream{ line };
			stream.ignore(41, ' ');
			stream >> hash >> time >> author;
			std::getline(stream >> std::ws, message);

			handler.setColor(utils::Color::BRIGHT_YELLOW);
			cout << std::left << std::setw(10) << "Commit:";
			handler.setColor(utils::Color::BRIGHT_GREEN);
			cout << hash << endl;

			handler.setColor(utils::Color::BRIGHT_YELLOW);
			cout << std::left << std::setw(10) << "Author:";
			handler.setColor(utils::Color::BRIGHT_GREEN);
			cout << author << endl;

			handler.setColor(utils::Color::BRIGHT_YELLOW);
			cout << std::left << std::setw(10) << "Date:";
			handler.setColor(utils::Color::BRIGHT_GREEN);
			utils::printDate(time);
			cout << endl;

			handler.setColor(utils::Color::BRIGHT_YELLOW);
			cout << std::left << std::setw(10) << "Message:";
			handler.setColor(utils::Color::BRIGHT_GREEN);
			cout << message << endl;
			cout << endl << endl;
		}
		handler.resetColor();
	}
};


