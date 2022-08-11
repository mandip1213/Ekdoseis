#pragma once

#ifndef INDEX_H
#define INDEX_H
#include<cstdint>
#include<string>
#include<vector>
#include<filesystem>
#include<fstream>
#include<sys/stat.h>
#include<iostream>
#include"enums.h"
#include"commit.h"
#include"tree.h"

#define endl '\n'

namespace fs = std::filesystem;

/*
The index file consists of:
* 32 bit number of index entries
* Index entry consists of:
	* 32+32 bit created time( second and nano second)
	* 32+32 bit modified time( second and nano second)

*/

class Index {
private:
	int mtreeCount;//number of index entry
	fs::path mrefToRootPath;
	fs::path mindexPath;
	std::vector<index::indexEntry> mindexEntries;
	void writeToFile(const index::indexEntry* entry = nullptr);
	void readFromFile(std::ifstream& fileiptr, index::indexEntry& entry);
public:
	Index();
	Index(const fs::path& rootPath);
	void fetchFromIndex();
	bool add(const fs::path& filePath, const std::string& hash);
	index::FileStatus getFileStatus(const fs::path& filePath, bool upDateIndex = false);
	bool hasFileChanged(const fs::path& filepath);
	void checkcout( Index& newIndex);
	friend void Commit::createTree();
	friend void Tree::createNewIndex(Index& ii,std::string dir);
	void restoreFile(const fs::path& path,bool staged=true);
};
#endif
