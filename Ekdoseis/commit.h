#pragma once
#ifndef COMMIT_H
#define COMMIT_H
#include <string>
#include <filesystem >
#include"tree.h"



namespace fs = std::filesystem;

class Commit {
private:
	bool minitialCommit{ false };
	Tree mtree;
	std::string mhash;
	std::string mparentHash;
	std::string mauthor;
	std::string mmessage;
	fs::path mrootPath;
	fs::path mrefPath;
	uint64_t mcoommitTime;
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
	void setMessage(std::string message) { mmessage = message; }
	bool loadFromCommitHash(const std::string& hash = "");
	const Tree& getTree()const { return mtree; }


};
#endif
