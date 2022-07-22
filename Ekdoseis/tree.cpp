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
	//hostel net sucks and so do no access to stackoverflow
	//std::basic_stringstream stream 
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
		cout << "Error: " << "cannot perform the required action" << endl;
		exit(EXIT_FAILURE);
	}
	std::ofstream ofptr{ objPath / mhash.substr(2,40 - 2) };
	//ofptr << streambuf.rdbuf();
	ofptr << streambuf.str();
	ofptr.close();
}

void Tree::createTreeFromObject() {
	const fs::path treeObj{ fs::current_path() / ".dose/objects" / mhash.substr(0,2) / mhash.substr(2,40 - 2) };
	std::ifstream treeiptr{ treeObj };
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

void Tree::createNewIndex(Index& ii,std::string dirName) {
	//fs::path currPath = fs::current_path();
	//fs::path dirPath = fs::current_path() / dirName;
	for (auto& currBlob : mblobs) {
		fs::path filePath = fs::current_path()/dirName / currBlob.getName();
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
			.flag = static_cast<unsigned int>(index::IndexFileStatus::STAGED),
			//.sha1 = hash.c_str(),
			.fileName = dirName + currBlob.getName()
			
		};
		ii.mtreeCount++;
		ii.mindexEntries.push_back(currEntry);
	}
	for (auto& currTree : mtrees) {
		currTree.createNewIndex(ii,dirName+currTree.myName+"/");
	}
}


std::string Tree::getHash() { return mhash; };
