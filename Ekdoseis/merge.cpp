#include<iostream>
#include<sstream>
#include<algorithm>
#include"tree.h"
#include"commit.h"
#include"index.h"
#include "merge.h"
#include"branch.h"
#include"utils.h"
#define endl '\n'
using std::cout;


Merge::Merge(const fs::path& rpath, const std::string& btom) :
	mbranchToMerge{ btom },
	mrootPath{ rpath }
{
	mcurrBranch = getHeadReference();
	if (mcurrBranch == "") {
		utils::printError("ERROR: Invalide reference in Head");
		exit(EXIT_SUCCESS);
	}
}

std::string Merge::getHeadReference() {
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



void Merge::merge() {
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
	Index currIndex{ mrootPath };
	currIndex.fetchFromIndex();

	Commit mergeCommit{ mrootPath };
	mergeCommit.setMessage("merged ref " + mbranchToMerge + " to ref " + mcurrBranch);/*change to branch name*/
	mergeCommit.setParenthash(mcurrBranch);//set curr hash to currbranch's hash
	mergeCommit.setTree(currBranchTree);/*currbranch tree becomes invalid after thsi*/
	mergeCommit.createCommit();

	currIndex.checkcout(newIndex);
	//Dose::updateHead(mrootPath,)//no need


	const std::array<utils::StringColorPair, 1> arr{ {
				   {"Successfullly merged branch", utils::Color::BRIGHT_YELLOW},
				   } };
	utils::printColorful(arr);








	return;
}

std::string Merge::findCommonAcestor() {
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








