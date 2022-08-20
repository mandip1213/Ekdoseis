#pragma once
#include<sstream>
#include<fstream>
#include"tree.h"
#include"index.h"
#include"blob.h"
#include"sha1.h"
#include"utils.h"
using std::cout;
#define endl '\n'

Tree::Tree(const std::string& name, const std::string& hash)
	: myName{ name }, mhash{ hash }
{
}

bool Tree::insertBlob(const std::string& hash, const fs::path& pathname) {

	Tree* currRootTree{ this };
	std::error_code ec;
	using pathIterator = fs::path::const_iterator;
	for (pathIterator it = pathname.begin();; it++) {
		bool exist = false;
		it++;
		if (it == pathname.end()) {
			for (auto& currBlob : currRootTree->mblobs) {
				//check if blob exists (won't exists anyway
				//TODO: you might omit it later
				if (currBlob.getName() == it->string()) {
					//do something if exist
				}
			}
			break;
		}
		it--;
		for (Tree& currTree : currRootTree->mtrees) {
			//if (fs::equivalent(currTree.myPath, *it, ec)) {
			if (currTree.myName == (it->string())) {
				currRootTree = &currTree;
				exist = true;
				break;
			}
		}
		if (!exist) {
			Tree newTree{ it->string() };
			currRootTree->mtrees.push_back(newTree);
			currRootTree = &(currRootTree->mtrees.back());
			exist = false;
		}
	}
	Blob newBlob{ hash,pathname.filename().string() };
	currRootTree->mblobs.push_back(newBlob);
	return true;
}

void Tree::createTreeObjects() {
	for (auto& currTree : mtrees) {
		currTree.createTreeObjects();
	}

	//TODO create a stream and finally write it into file 
	//not write into file and again read it to create hash 
	//hostel net sucks and so  no access to stackoverflow
	std::stringstream streambuf;
	for (auto& currBlob : mblobs) {
		//TODO:permissions
		streambuf << 100644 << " " << "blob" << " " << currBlob.getHash() << " " << currBlob.getName() << '\n';
	}
	for (auto& currTree : mtrees) {
		streambuf << 100644 << " " << "tree" << " " << currTree.mhash << " " << currTree.myName << '\n';
	}
	streambuf.seekg(0, std::ios::beg);//imp: setting the pointer to start 
	SHA1 hash;
	hash.update(streambuf);
	mhash = hash.final();
	//mhash = hash.final().c_str();
	fs::path objPath{ fs::current_path() / ".dose/objects" / mhash.substr(0,2) };
	ReturnFlag _rf = utils::createObjectDir(objPath);
	if (!(_rf == CREATE_SUCCESS || _rf == ALREADY_EXISTS)) {
		std::stringstream errmsg;
		errmsg << "Error: " << "cannot perform the required action" << endl;
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
	}
	std::ofstream ofptr{ objPath / mhash.substr(2,40 - 2) };
	//ofptr << streambuf.rdbuf();
	ofptr << streambuf.str();
	ofptr.close();
}

void Tree::createTreeFromObject() {
	const fs::path treeObj{ fs::current_path() / ".dose/objects" / mhash.substr(0,2) / mhash.substr(2,40 - 2) };
	std::ifstream treeiptr{ treeObj };
	if (!treeiptr) {
		utils::printError("Invalid commit object '" + mhash + "'");
		exit(EXIT_SUCCESS);
	}
	std::string line;
	std::string permissions;//later
	std::string type;
	std::string name;
	std::string hash;
	while (std::getline(treeiptr, line)) {
		std::stringstream stream{ line };
		stream >> permissions >> type >> hash >> name;
		if (type.compare("blob") == 0) {
			this->mblobs.push_back(Blob{ hash,name });
		}
		else if (type.compare("tree") == 0) {
			Tree tempTree{ name,hash };
			tempTree.createTreeFromObject();
			this->mtrees.push_back(tempTree);
		}
	}
}

void Tree::createNewIndex(Index& ii, std::string dirName) {
	//fs::path currPath = fs::current_path();
	//fs::path dirPath = fs::current_path() / dirName;
	for (auto& currBlob : mblobs) {
		fs::path filePath = fs::current_path() / dirName / currBlob.getName();
		struct _stat stat;
		//get stat from hashed obj ig
		bool _temp = _stat(filePath.string().c_str(), &stat);//_bool ?

		index::indexEntry currEntry = {
			.createdTime = static_cast<unsigned int>(stat.st_ctime),
			.modifiedTime = static_cast<unsigned int>(stat.st_mtime),
			.sd_dev = stat.st_dev,
			.sd_ino = stat.st_ino,
			.mode = stat.st_mode,
			.sd_uid = static_cast<unsigned  int>(stat.st_uid),
			.sd_gid = static_cast<unsigned  int>(stat.st_gid),
			.sd_size = static_cast<unsigned  int>(stat.st_size),
			.sha1 = currBlob.getHash(),
			.flag = static_cast<unsigned int>(index::IndexFileStatus::COMMITTED),
			//.sha1 = hash.c_str(),
			.fileName = dirName + currBlob.getName()

		};
		ii.mtreeCount++;
		ii.mindexEntries.push_back(currEntry);
	}
	for (auto& currTree : mtrees) {
		currTree.createNewIndex(ii, dirName + currTree.myName + "/");
	}
}


std::string Tree::getHash() { return mhash; };

std::string Tree::getHashOfFile(const std::string& fileName)const {
	auto blob = std::find_if(mblobs.begin(), mblobs.end(), [fileName](auto curr) {
		return curr.getName() == fileName;
		});
	if (blob != mblobs.end()) {
		return blob->getHash();
	}
	for (auto currTree : mtrees) {
		currTree.getHashOfFile(fileName);
	}
	return "";//if filenot found

}


Tree operator-(const Tree& tree1, const Tree& tree2) {
	Tree difftree{ "" };

	auto matchflag = false;
	for (auto blob1 : tree1.mblobs) {
		for (auto blob2 : tree2.mblobs) {
			if (blob1.getName() == blob2.getName()) {
				matchflag = true;
				break;
			}
		}
		if (!matchflag) {
			difftree.mblobs.push_back(blob1);
		}
		matchflag = false;
	}
	for (auto t1 : tree1.mtrees) {
		for (auto t2 : tree2.mtrees) {
			if (t1.myName == t2.myName) {
				Tree tempdiff = t1 - t2;
				tempdiff.myName = t1.myName;
				difftree.mtrees.push_back(tempdiff);
				matchflag = true;
				break;
			}
		}
		if (!matchflag) {
			difftree.mtrees.push_back(t1);
			matchflag = false;
		}
	}
	return difftree;
	/*merge this fucntion into merge Tree : leave for now*/
}

/*
For each existing file in both branches:
	- if file is not changed in both branch, nothing
	- if any file is changed only in current branch, don't touch it.
	- if any file is changed only in branch to merge, update the file in current branch.
	- if file is changed in both branch, asks the user which file he / she wants to keep.

for new file in current branch :
	don't touch it

for new file in branchtomerge :
	bring the file in current branch

for new file in both branch with same name:
	ask the user which file he / she wants to keep

for deleted file in currbranch????

same applies for tree
*/

void Tree::mergeTree(const Tree& treeToMerge, const Tree& commonAncestor, const fs::path& rootPath) {

	using enum utils::Color;
	utils::ConsoleHandler handler;

	auto userChoice = [&](const std::string& currblob, bool newfile)->int {
		handler.setColor(BRIGHT_GREEN);
		cout << "'" << currblob << "'";
		handler.setColor(BRIGHT_YELLOW);
		cout << (!newfile ? "is changed in both branches. " : "is created new in both branch") << endl;
		handler.resetColor();
		cout << "Please select the branch you want to keep change of: " << endl;
		handler.setColor(BRIGHT_GREEN);

		//		cout << "1. " << this->myName << endl;
		//		cout << "2. " << treeToMerge.myName << endl;

		cout << "1. " << "current branch" << endl;
		cout << "2. " << "treeToMerge.myName " << endl;

		handler.setColor(BRIGHT_YELLOW);
		std::cout << "Choose branch: (1/2)";

		int ch;
		std::cin >> ch;
		while (ch != 1 && ch != 2) {
			cout << "Please choose corrent branch. (1/2): ";
			std::cin >> ch;
		}
		return ch;
	};

	for (auto& currblob : this->mblobs) {
		for (const auto& blobtoMerge : treeToMerge.mblobs) {
			if (currblob.getName() == blobtoMerge.getName()) {
				/* for file in both branches */
				bool foundinCommon = false;
				for (const auto& commonblob : commonAncestor.mblobs) {
					if (currblob.getName() == commonblob.getName()) {
						foundinCommon = true;
						const auto currhash = currblob.getHash();
						const auto hashtoMerge = blobtoMerge.getHash();
						const auto commonhash = commonblob.getHash();
						if (hashtoMerge == commonhash) {
							/*File is not changed in branchtomerge i.e file is only changed in currbranch or file is not changed at all*/
							/*do nothing*/
						}
						else if (currhash == commonhash) {
							/*File is changed in brnachTomerge only*/
							currblob.updateHash(hashtoMerge);
						}
						else {
							/* file is changed in both branch*/
							if (userChoice(currblob.getName(), false) == 2)
								currblob.updateHash(hashtoMerge);
						}
					}
				}
				/* a new file with same name on both tree*/
				if (!foundinCommon && userChoice(currblob.getName(), false) == 2)
					currblob.updateHash(blobtoMerge.getHash());
			}
			else {
				/* new file in currbranch : don't touch*/
			}
		}
	}

	for (auto& currtree : this->mtrees) {
		const auto currName = currtree.myName;
		for (const auto& childTreetoMerge : treeToMerge.mtrees) {
			if (currName == childTreetoMerge.myName) {
				if (currtree.mhash == childTreetoMerge.mhash) {
					/*  trees are same in both branch*/
					break;
				}
				/*tree is found in both*/
				auto commonTree = std::find_if(commonAncestor.mtrees.begin(), commonAncestor.mtrees.end(), [currName](const Tree& tree) {
					return tree.myName == currName;
					});

				if (commonTree != commonAncestor.mtrees.end()) {
					/*tree is in common ancestor too*/
					currtree.mergeTree(childTreetoMerge, *commonTree, rootPath);
				}
				else {
					/*tree is new in both branches*/
					currtree.mergeTree(childTreetoMerge, Tree{ "" }, rootPath);
				}
			}
			else {
				/* new tree in current branch : don't touch*/
			}
		}
	}

}


void Tree::mergeDiffTree(const Tree& newTree) {
	for (auto blob : newTree.mblobs) {
		this->mblobs.push_back(std::move(blob));
	}
	for (auto newtree : newTree.mtrees) {
		bool found = false;
		for (auto& existingtree : this->mtrees) {
			if (newtree.myName == existingtree.myName) {
				existingtree.mergeDiffTree(newtree);
				found = true;
				break;
			}
		}
		if (!found) {
			this->mtrees.push_back(newtree);
		}
	}
}
