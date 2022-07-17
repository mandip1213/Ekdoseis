#pragma once
#include <string>
#include <filesystem >

#include"tree.h"

namespace fs = std::filesystem;

namespace commit {

}

class Commit {
private:
	bool minitialCommit{ false };
	Tree mtree{""};
	std::string mhash;
	std::string mparentHash;
	std::string mauthor;
	fs::path mrootPath;
	fs::path mrefPath;
	void createLogFiles();
	//timestamp
	//gmt ahead before 
public:
	Commit(const fs::path& rootPath);
	void createTree();//create a tree object from index entries 
	void updateHead();
	void  compareTreeHash();
	void fetchParentHash();
	void updateLogs();
	void createCommit();


};
