#pragma once
#include<vector>
#include<filesystem>
namespace fs = std::filesystem;
class DoseIgnore {
private:
	std::vector<std::string> ignorePaths;
	 fs::path refToRootPath; //error using const reference
	template<typename T>
	bool pHas(const T& path)const {
		for (std::string current : ignorePaths) {
			std::error_code ec;
			if (fs::equivalent(path,refToRootPath/current, ec)) {
				return true;
			}
		}
		return false;
	}
public:
	DoseIgnore();
	DoseIgnore(const fs::path& rootPath);
	bool has(const fs::path& path)const;
	bool has(const std::string& path)const;
	bool has(const char* path)const;
};
