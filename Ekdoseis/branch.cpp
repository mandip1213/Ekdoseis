#include"branch.h"
#include"utils.h"
#define endl '\n'
using std::cout;

Branch::Branch(const fs::path& rpath, const std::string& name) :
	mbranchName{ name }, mrootPath{ rpath } {}

Branch::Branch(const fs::path& rpath) : mrootPath{ rpath } {
}

void Branch::createBranch() {
	/*
	- To create a new branch , create a new file in refs/heads with the branch name
	- if no object or branch is provided
	- find the latest commit of current branch and make the branch points to the commit
	-if branch or commit hash is provided
	- let the branch points to the corresponding commit
	-- create  log file .dose/logs/refs with a branch name
	*/
	std::ifstream headptr{ mrootPath / ".dose/HEAD" };
	std::string reference;
	std::getline(headptr, reference);
	if (reference.size() == 40)
		createBranch(reference);
	else {
		reference.erase(0, std::string{ "ref: refs/heads/" }.size());
		createBranch(reference);
	}
}

std::string  Branch::getCommitFromBranch(const fs::path& rootPath, const std::string& bName) {
	std::ifstream bfptr{ rootPath / ".dose/refs/heads" / bName };
	if (!bfptr)
		return "";
	std::string hash;
	bfptr >> hash;
	if (hash.length() != 40) {
		return "";
	}
	return hash;
}

void Branch::createBranch(const std::string& object) {
	if (fs::exists(mrootPath / ".dose/refs/heads" / mbranchName)) {
		//{"ERROR: branch ", utils::Color::BRIGHT_RED}
		const std::array<utils::StringColorPair, 3> arr
		{
			{
			{"Error: branch ", utils::Color::BRIGHT_RED },
			{"'" + mbranchName + "'", utils::Color::BRIGHT_MAGENTA},
			{" already exists\n", utils::Color::BRIGHT_RED}
			}
		};
		utils::printColorful(arr);
		exit(EXIT_SUCCESS);
	}

	std::string commitObj;
	if (Branch::isBranch(mrootPath, object)) {
		std::ifstream refptr{ mrootPath / ".dose/refs/heads/" / object };
		if (!refptr) {
			utils::printError("Error: unable to open the correspoding file.");
			exit(0);
		}
		refptr >> commitObj;
	}
	else {
		commitObj = object;
	}

	if (commitObj.size() != 40 || !fs::exists(mrootPath / ".dose/objects" / commitObj.substr(0, 2) / commitObj.substr(2, 40 - 2))) {
		utils::printError("ERROR: Illegal reference '" + object + "'");
		exit(0);
	}

	/* Now do what you need to do*/
	std::ofstream newBranchPtr{ mrootPath / ".dose/refs/heads/" / mbranchName };
	newBranchPtr << commitObj;
	std::ofstream newLogPtr{ mrootPath / ".dose/logs/refs" / mbranchName };
	/* TODO MUST : refactor : in logfile object*/
	auto currTime = utils::getTime();

	newLogPtr << "0000000000000000000000000000000000000000" << " " << commitObj << " "
		<< currTime << " author " << "branch: created from " << object << endl;

	const std::array<utils::StringColorPair, 4> text{ {
		{"Successfullyl created branch ", utils::Color::BRIGHT_YELLOW},
		{"'" + mbranchName + "'", utils::Color::BRIGHT_GREEN},
		{" from ",utils::Color::BRIGHT_YELLOW},
		{"'" + object + "'\n", utils::Color::BRIGHT_GREEN}
	} };
	utils::printColorful(text);
}

bool Branch::isBranch(const fs::path& rootPath, const std::string& bName) {
	/*
	const fs::path headPath{ rootPath / ".dose/refs/heads" };
	if (fs::exists(headPath)) {
		for (const auto& entry : fs::directory_iterator{ headPath }) {
			if (entry.path().filename() == bName)
				return true;
		}
	}
	*/
	const fs::path branchPath{ rootPath / ".dose/refs/heads" / bName };
	if (fs::exists(branchPath))
		return true;

	return false;
}

void Branch::listBranch(const fs::path& rootPath) {
	std::ifstream headfptr{ rootPath / ".dose/HEAD" };
	std::string reference;
	std::getline(headfptr, reference);

	utils::ConsoleHandler handler;
	if (reference.starts_with("ref: ")) {
		reference.erase(0, std::string("ref: refs/heads/").size());
	}
	else if (reference.size() == 40) {
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << "* ( HEAD Detached at " << reference.substr(0, 8) << " )" << endl;
	}

	std::vector<std::string> branches;
	const fs::path headPath{ rootPath / ".dose/refs/heads" };
	if (fs::exists(headPath)) {
		for (const auto& entry : fs::directory_iterator{ headPath }) {
			auto size = entry.file_size();
			if (size != 0)
				branches.push_back(entry.path().filename().string());
		}
	}
	std::sort(branches.begin(), branches.end());
	handler.setColor(utils::Color::BRIGHT_YELLOW);
	for (auto branch : branches) {
		if (branch == reference) {
			handler.setColor(utils::Color::BRIGHT_GREEN);
			cout << "* " << branch << endl;
			handler.setColor(utils::Color::BRIGHT_YELLOW);
			continue;
		}
		cout << "  " << branch << endl;
	}

}

void Branch::deleteBranch() {
	/*delete a branch only if it is not a current branch*/
	std::ifstream headfptr{ mrootPath / ".dose/HEAD" };
	std::string reference;
	std::getline(headfptr, reference);
	const fs::path branchPath{ mrootPath / ".dose/refs/heads" / mbranchName };

	if (!fs::exists(branchPath)) {
		const std::array<utils::StringColorPair, 3> text{ {
			{"ERROR: branch", utils::Color::BRIGHT_RED},
			{" '" + mbranchName + "' ", utils::Color::BRIGHT_GREEN},
			{"doesnot exist.\n",utils::Color::BRIGHT_RED},
		} };
		utils::printColorful(text);
	}
	else if (reference.ends_with(mbranchName)) {
		const std::array<utils::StringColorPair, 3> text{ {
			{"ERROR: unable to delete\n", utils::Color::BRIGHT_RED},
			{"'" + mbranchName + "' ", utils::Color::BRIGHT_GREEN},
			{"is current branch.\n",utils::Color::BRIGHT_YELLOW},
		} };
		utils::printColorful(text);
	}
	else {
		std::error_code ec;
		if (fs::remove(branchPath, ec)) {
			fs::remove(mrootPath / ".dose/logs/refs" / mbranchName, ec);//I expect log to be deleted
			const std::array<utils::StringColorPair, 3> text{ {
				{"Branch", utils::Color::BRIGHT_YELLOW},
				{" '" + mbranchName + "' ", utils::Color::BRIGHT_GREEN},
				{"deleted successfully.\n",utils::Color::BRIGHT_YELLOW},
			} };
			utils::printColorful(text);
		}
		else {
			if (ec) {
				cout << "ERROR: couldnot delete branch '" << mbranchName << "'." << endl;
				cout << "Error msg: " << ec.message() << endl;
			}
			else {
				cout << "Error: branch '" << mbranchName << "' doesnot exist." << endl;
			}

		}

	}
}
