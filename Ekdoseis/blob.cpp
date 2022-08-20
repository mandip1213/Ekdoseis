#pragma once
#include<string>
#include"blob.h"

Blob::Blob(const std::string& hash, const std::string& name)
	:myName{ name }, mhash{ hash }{
}

const std::string& Blob::getName() const {
	return myName;
}
std::string Blob::getHash() const { return mhash; };

void Blob::updateHash(const std::string& hash) {
	mhash = hash;
}
