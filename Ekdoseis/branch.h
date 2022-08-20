#pragma once
#include<filesystem>
#include<string>
#include<array>
#include<iostream>
#include<utility>
#include<fstream>


namespace fs = std::filesystem;


class Branch {
private:
	std::string mbranchName;
	fs::path mrootPath;
public:
	Branch(const fs::path& rpath, const std::string& name);
	Branch(const fs::path& rpath);
	void createBranch();
	static std::string  getCommitFromBranch(const fs::path& rootPath, const std::string& bName);
	void createBranch(const std::string& object);
	static bool isBranch(const fs::path& rootPath, const std::string& bName);
	static void listBranch(const fs::path& rootPath);
	void deleteBranch();
};


