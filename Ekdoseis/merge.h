#pragma once
#include<filesystem>
#include<string>

namespace fs = std::filesystem;

class Merge {
private:
	std::string mcurrBranch;
	std::string mbranchToMerge;
	std::string mancestor;
	fs::path mrootPath;

public:
	Merge(const fs::path& rpath, const std::string& btom);
	std::string getHeadReference();
	std::string findCommonAcestor();
	void merge();

};







