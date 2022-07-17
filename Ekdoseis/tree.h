#pragma once
#include<vector>
#include<string>
#include<filesystem>
#include"blob.h"

namespace fs = std::filesystem;
class Tree {
private:
	std::string mhash;
	std::string myName;
	std::vector<Tree> mtrees;
	std::vector<Blob> mblobs;
public:
	Tree(const std::string& name);
	bool insertBlob(const std::string& hash,const fs::path& dirname);
	void createTreeObjects();
	std::string getHash() { return mhash; };//remove this and use friend function of class

};



