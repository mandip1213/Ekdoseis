#include<fstream>
#include"doseignore.h"


DoseIgnore::DoseIgnore() = default;
DoseIgnore::DoseIgnore(const fs::path & rootPath)
	:refToRootPath{ rootPath } {
	const fs::path ignoreFile = rootPath / ".doseIgnore";
	if (fs::exists(ignoreFile)) {
		std::ifstream ignoreptr{ ignoreFile };
		std::string _templine;
		size_t i{ 0 };
		while (std::getline(ignoreptr, _templine)) {
			if (_templine.length() != 0) {
				//ignorePaths[i++] = _templine;
				ignorePaths.push_back(_templine);
			}
		}
	}
}

bool DoseIgnore::has(const fs::path & path)const {
	return pHas(path);
}
bool DoseIgnore::has(const std::string & pathstr)const {
	return pHas(fs::path{ pathstr });
}
bool DoseIgnore::has(const char* pathstr)const {
	return pHas(fs::path{ pathstr });
}

