#pragma once
#ifndef BLOB_H
#define BLOB_H
#include<string>

class Blob {
private:
	std::string mhash;
	std::string myName;
public:
	Blob(const std::string& hash, const std::string& name);
	const std::string& getName()const;
	std::string getHash()const;
};
#endif
