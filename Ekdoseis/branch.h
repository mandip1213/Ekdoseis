#pragma once
#include<filesystem>
#include<string>
#include<array>
#include<iostream>
#include<utility>
#include<fstream>
#include"utils.h"

namespace fs = std::filesystem;

using std::cout;
#define endl '\n'
std::ofstream& operator<<(std::ofstream& out, utils::Color color) {

	return out;
}
class Branch {
private:
	std::string mbranchName;
	fs::path mrootPath;
public:
	Branch(const fs::path& rpath, const std::string& name) :
		mbranchName{ name }, mrootPath{ rpath } {}

	Branch(const fs::path& rpath) :mrootPath{ rpath } {
	}

	void createBranch() {
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

	std::string static getCommitFromBranch(const fs::path& rootPath, const std::string& bName) {
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

	void createBranch(const std::string& object) {
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
			{"Successfullyl created branch ", utils::Color::BRIGHT_GREEN},
			{"'" + mbranchName + "'", utils::Color::BRIGHT_YELLOW},
			{" from ",utils::Color::BRIGHT_GREEN},
			{"'" + object + "'\n", utils::Color::BRIGHT_YELLOW}
		} };
		utils::printColorful(text);
	}

	static bool isBranch(const fs::path& rootPath, const std::string& bName) {
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

	static void listBranch(const fs::path& rootPath) {
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

	void deleteBranch() {
		/*delete a branch only if it is not a current branch*/
		std::ifstream headfptr{ mrootPath / ".dose/HEAD" };
		std::string reference;
		std::getline(headfptr, reference);
		const fs::path branchPath{ mrootPath / ".dose/refs/heads" / mbranchName };

		if (!fs::exists(branchPath)) {
			cout << "Error: branch '" << mbranchName << "' doesnot exist." << endl;

		}
		else if (reference.ends_with(mbranchName)) {
			cout << "ERROR: unable to delete.\n'" << mbranchName << "' is current branch.\n";
		}
		else {
			std::error_code ec;
			if (fs::remove(branchPath, ec)) {
				fs::remove(mrootPath / ".dose/logs/refs" / mbranchName, ec);//I expect log to be deleted
				cout << "Branch '" << mbranchName << "' deleted successfully." << endl;
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
};

class Merge {
private:
	std::string mcurrBranch;
	std::string mbranchToMerge;
	std::string mancestor;
	fs::path mrootPath;

public:
	Merge(const fs::path& rpath, const std::string& btom) :
		mbranchToMerge{ btom },
		mrootPath{ rpath }
	{
		mcurrBranch = getHeadReference();
		if (mcurrBranch == "") {
			utils::printError("ERROR: Invalide reference in Head");
			exit(EXIT_SUCCESS);
		}
	}

	std::string getHeadReference() {
		std::ifstream headfptr{ mrootPath / ".dose/HEAD" };
		std::string reference;
		std::getline(headfptr, reference);
		if (reference.starts_with("ref: refs/heads/")) {
			reference = reference.erase(0, std::string{ "ref: refs/heads/" }.size());
			if (!Branch::isBranch(mrootPath, reference)) {
				utils::printError("HEAD does not reference to any branch");
				exit(0);
			}
			const std::string branchName = reference;
			return Branch::getCommitFromBranch(mrootPath, branchName);
		}
		else {
			utils::printError("HEAD does not reference to any branch");
			exit(0);

		}

	}



	void merge() {
		/*
		To merge one branch into another(we couldnot implement the algorithm like git). But the workaround:
		- find the common branch
		- make three tree, tree of common branch, current branch, branch to merge

		create a new tree :
		new tree = tree of current brnach + (tree of btomerge -tree of common ancestor)
		// what happen to deleted file on btomerge but exist in common branch?


		For each existing file : if file is not changed in both branch, nothing
			- if any file is changed only in current branch, don't touch it.
			- if any file is changed only in branch to merge, update the file in current branch.
			- if file is changed in both branch, asks the user which file he / she wants to keep

			for new file in current branch :
		don't touch it

			for new file in branch to merge :
		bring the file in current branch

			for new file in both branch with same name:
		ask the user which file he / she wants to keep
			* /

			/*
			fast forward
			automatic merging
			user resolved
			*/

		const std::string  ca = findCommonAcestor();
		if (ca == "") {
			utils::printError("Cannot find common ancestor to merge");
			exit(EXIT_SUCCESS);
		}

		auto getTreeHash = [this](const std::string& commitHash) {
			//create the tree recursively 
			std::ifstream commitptr{ mrootPath / ".dose/objects" / commitHash.substr(0,2) / commitHash.substr(2,40 - 2) };
			if (!commitptr) {
				utils::printError("ERROR: illegal commit reference ");
				cout << "'" << commitHash << "'" << endl;
				exit(EXIT_FAILURE);
			}
			std::string treehash;
			commitptr >> treehash >> treehash;
			if (treehash.size() != 40) {
				utils::printError("ERROR: illegal tree reference in object ");
				cout << "'" << commitHash << "'" << endl;
				exit(EXIT_FAILURE);
			}
			return treehash;

		};

		Tree commonAncestorTree{ "" ,getTreeHash(ca) };
		Tree currBranchTree{ mcurrBranch,getTreeHash(mcurrBranch) };
		Tree btoMergeTree{ mbranchToMerge,getTreeHash(mbranchToMerge) };
		commonAncestorTree.createTreeFromObject();
		currBranchTree.createTreeFromObject();
		btoMergeTree.createTreeFromObject();

		Tree diffTree = btoMergeTree - commonAncestorTree;

		currBranchTree.mergeTree(btoMergeTree, commonAncestorTree, mrootPath);
		currBranchTree.mergeDiffTree(diffTree);/*diffTree is invalide after this call*/

		currBranchTree.createTreeObjects();
		Index newIndex{ mrootPath };
		currBranchTree.createNewIndex(newIndex);
		Index currIndex{mrootPath};
		currIndex.fetchFromIndex();

		Commit mergeCommit{ mrootPath };
		mergeCommit.setMessage("merged ref " + mbranchToMerge + " to ref " + mcurrBranch);/*change to branch name*/
		mergeCommit.setParenthash(mcurrBranch);//set curr hash to currbranch's hash
		mergeCommit.createCommit();
		mergeCommit.setTree(currBranchTree);/*currbranch tree becomes invalid after thsi*/

		currIndex.checkcout(newIndex);
		//Dose::updateHead(mrootPath,)//no need










			return;
	}

	std::string findCommonAcestor() {
		/*
		To find the common parent of A and B
		add A current commit to the commitList A
		add b current commit to the commitList b
		For every commit in commitlistA match in commitlist B
		if(any matches) that is the current common commit
		if not push one more commit to both list i.e parent of end commit
		*/

		std::vector<std::string> commitListsA{ mcurrBranch };
		std::vector<std::string> commitListsB{ mbranchToMerge };

		auto getCommit = [this](const std::string& commit) {
			/*I am assumnig everything is perfect for now */
			const fs::path cpath = this->mrootPath / ".dose/objects/" / commit.substr(0, 2) / commit.substr(2, 40 - 2);
			std::ifstream fptr{ cpath };
			if (!fptr) {
				utils::printError("Invalid Commit object " + commit);
				exit(EXIT_SUCCESS);
			}
			std::string line;
			std::getline(fptr, line);//ignore first line
			std::getline(fptr, line);
			try {
				line = line.substr(std::string{ "parent " }.size(), 40);
				return line;
			}
			catch (const std::out_of_range&) {
				utils::printError("Illegal commit object found");
				exit(EXIT_SUCCESS);
			}
		};

		while (1) {
			for (auto commitA : commitListsA) {
				/*TODO: use reverse iterator*/
				for (auto commitB : commitListsB) {
					/*RIP DSA till fourth semester*/
					if (commitA == commitB) {
						return commitA;
					}
				}
			}

			if (commitListsA.back() != std::string(40, '0')) {
				const std::string parentA = getCommit(commitListsA.back());
				commitListsA.push_back(parentA);
			}
			if (commitListsB.back() != std::string(40, '0')) {
				const std::string parentB = getCommit(commitListsB.back());
				commitListsB.push_back(parentB);
			}
			if (commitListsA.back() == std::string(40, '0') && commitListsB.back() == std::string(40, '0'))
				return "";
		}
	}


};








