#include<string>
#include"blob.h"
Blob::Blob(const std::string& hash, const std::string& name) 
	:myName{ name }, mhash{ hash }{
}

const std::string& Blob::getName() {
	return myName;
}
	std::string Blob::getHash() { return mhash; };
