#pragma once
#include<vector>
#include<string>
#include<filesystem>
#include"blob.h"

class Index;//yo line le 3 ghanta insane banayo
namespace fs = std::filesystem;
class Tree {
private:
	std::string mhash;
	std::string myName;
	std::vector<Tree> mtrees;
	std::vector<Blob> mblobs;
public:

	Tree(const std::string& name, const std::string& hash = "");
	//Tree(const std::string& name,const std::string& mhash);
	bool insertBlob(const std::string& hash, const fs::path& dirname);//insert blob to the tree object
	void createTreeObjects();//physically create objects a/c to tree lists
	std::string getHash();//remove this and use friend function of class
	void createTreeFromObject();
	void createNewIndex(Index& ii, std::string dir = "");
	std::string getHashOfFile(const std::string& fileName)const;
	friend Tree operator-(const Tree& tree1, const Tree& tree2);
	void mergeTree(const Tree& treeToMerge,const Tree& commonAncestor, const fs::path& rootPath);
	void mergeDiffTree(const Tree& newTree);
};



