#pragma once
#include<string>

class Blob {
private:
	std::string mhash;
	std::string myName;
public:
	Blob(const std::string& hash, const std::string& name);
	const std::string& getName() {
		return myName;
	}
	std::string getHash() { return mhash; };
};
