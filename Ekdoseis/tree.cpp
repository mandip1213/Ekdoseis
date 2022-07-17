#pragma once
#include<sstream>
#include<fstream>
#include"tree.h"
#include"blob.h"
#include"sha1.h"
#include"utils.h"
using std::cout;
#define endl '\n'

Tree::Tree(const std::string& name) :myName{ name } {

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
		streambuf << 100644 << " " << "tree" << " " << currTree.myName << " " << currTree.mhash << '\n';
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

