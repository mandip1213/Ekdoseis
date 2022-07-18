#pragma once
#include<cstdint>
#include<string>
#include<vector>
#include<filesystem>
#include<fstream>
#include<sys/stat.h>
#include<iostream>
#include"utils.h"
#include"commit.h"
#define endl '\n'

namespace fs = std::filesystem;

/*
The index file consists of:
* 32 bit number of index entries
* Index entry consists of:
	* 32+32 bit created time( second and nano second)
	* 32+32 bit modified time( second and nano second)

*/
namespace index {
	enum class IndexFileStatus {
		COMMITTED = 1,// can be in index
		STAGED = 2,//can be in index
	};
	enum class FileStatus {
		COMMITTED,// can be in index
		STAGED,//can be in index
		MODIFIED,
		UNTRACKED,
	};
	/*	struct time {
			uint32_t sec;
			uint32_t nsec;
		};
	*/
	/*	struct statData {
			//struct time createdTime;
			//struct time modifiedTime;
			unsigned int createdTime;
			unsigned int modifiedTime;
			unsigned int sd_dev;//metadata
			unsigned int sd_ino;//metadata
			unsigned int mode;//permissoins -only this is used in windows
			unsigned int sd_uid;//metadata
			unsigned int sd_gid;//metadata
			unsigned int sd_size;//filesize
		};
	*/


	struct indexEntry {
		unsigned int createdTime;
		unsigned int modifiedTime;
		unsigned int sd_dev;//metadata
		unsigned int sd_ino;//metadata
		unsigned int mode;//permissoins -only this is used in windows
		unsigned int sd_uid;//metadata
		unsigned int sd_gid;//metadata
		unsigned int sd_size;//filesize
		std::string   sha1;//what is signed and unsigned char 
		unsigned int flag;
		std::string fileName;//variable length null terminated

	};
};
class Index {
private:
	int mtreeCount;//number of index entry
	fs::path mrefToRootPath;
	fs::path mindexPath;
	std::vector<index::indexEntry> mindexEntries;
	bool latestFetch{ false };
	void fetchFromIndex();
	void writeToFile(const index::indexEntry* entry = nullptr);
	void readFromFile(std::ifstream& fileiptr, index::indexEntry& entry);
	index::FileStatus getFileStatus(const fs::path& filePath, bool upDateIndex = false);
public:
	Index();
	Index(const fs::path& rootPath);
	bool add(const fs::path& filePath, const std::string& hash);
	bool hasFileChanged(const fs::path& filepath);
	friend void Commit::createTree();
};


