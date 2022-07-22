#include "index.h"
#include "sha1.h"

//namespace fs = std::filesystem;
using std::cerr, std::cout;
void Index::fetchFromIndex() {
	if (fs::is_empty(mindexPath)) {
		return;
	}
	std::ifstream indexiptr{ mindexPath };
	if (!indexiptr) {
		cerr << "Error: " << "cannot open index file" << endl;
		exit(EXIT_FAILURE);
	}
	indexiptr >> mtreeCount;
	indexiptr.ignore(1);//ignore a new line
	mindexEntries.resize(mtreeCount);
	for (int i = 0; i < mtreeCount; i++) {
		struct index::indexEntry curr;
		readFromFile(indexiptr, curr);
		//mindexEntries.push_back(curr);
		mindexEntries[i] = curr;
	}
}

index::FileStatus Index::getFileStatus(const fs::path& filePath, bool updateIndex) {
	std::error_code ec;
	for (auto& entry : mindexEntries) {
		if (fs::equivalent(mrefToRootPath / entry.fileName, filePath, ec)) {
			//file exists in index file
			struct _stat filestat;
			bool _temp = _stat(filePath.string().c_str(), &filestat);//do what with bool 
			if (entry.modifiedTime == filestat.st_mtime) {
				//TODO: check for staged or commited
				if (static_cast<index::IndexFileStatus>(entry.flag) == index::IndexFileStatus::STAGED) {
					return index::FileStatus::STAGED;
				}
				return index::FileStatus::COMMITTED;
			}
			else {
				//in index but modified date donot match
				//so file is changed
				//and hence change the modifed date index too and update the index
				if (updateIndex == true) {
					SHA1 sha1;
					std::string hash{ sha1.from_file(filePath.string()) };
					const fs::path _hashFilePath{ mrefToRootPath / ".dose/objects" / hash.substr(0,2) / hash.substr(2,40 - 2) };

					std::error_code ec;
					fs::copy_file(filePath, _hashFilePath, fs::copy_options::skip_existing, ec); //no encryption now maybe later
					if (ec) {
						cout << "Error: " << ec.message() << endl;
						cout << "Error: " << "cannot perform the required action" << endl;
						exit(EXIT_FAILURE);
					}

					entry.modifiedTime = filestat.st_mtime;
					entry.sha1 = hash;
				}
				return  index::FileStatus::MODIFIED;
			}
		}
	}
	return index::FileStatus::UNTRACKED;

}

Index::Index() = default;

Index::Index(const fs::path & rootPath)
	:mtreeCount{ 0 },
	mrefToRootPath{ rootPath },
	mindexPath{ rootPath / ".dose/index" }{
	if (!exists(mindexPath)) {
		std::ofstream _indexfptr{ mindexPath };
	}
}


void Index::writeToFile(const index::indexEntry * entry) {
	//entry=nullptr(def val)
	std::ofstream fileoptr{ mindexPath };
	fileoptr << mtreeCount << '\n';
	for (auto entry : mindexEntries) {
		fileoptr << entry.createdTime << ' '
			<< entry.modifiedTime << ' '
			<< entry.sd_dev << ' '
			<< entry.sd_ino << ' '
			<< entry.mode << ' '
			<< entry.sd_uid << ' '
			<< entry.sd_gid << ' '
			<< entry.flag << ' '
			<< entry.sd_size << ' '
			<< entry.sha1 << ' '
			<< entry.fileName.c_str() << '\n';
	}
}

void Index::readFromFile(std::ifstream & fileiptr, index::indexEntry & entry) {
	std::string hash;
	fileiptr >> entry.createdTime;
	fileiptr >> entry.modifiedTime;
	fileiptr >> entry.sd_dev;
	fileiptr >> entry.sd_ino;
	fileiptr >> entry.mode;
	fileiptr >> entry.sd_uid;
	fileiptr >> entry.sd_gid;
	fileiptr >> entry.flag;
	fileiptr >> entry.sd_size;
	//fileiptr.get(static_cast<unsigned char*>(entry.sha1), 20);
	fileiptr >> entry.sha1;
	fileiptr >> entry.fileName;
	//fileiptr.ignore(1);//new line
}

bool Index::add(const fs::path & filePath, const std::string & hash) {
	using enum index::FileStatus;
	//latestFetch = true;
	index::FileStatus fileStatus = getFileStatus(filePath, true);
	if (fileStatus == MODIFIED) {
		//The modiifeid date inindex entries is updated
		writeToFile();
		return true;
	}
	if (fileStatus == COMMITTED) {
		cout << "nothing to add in file " << filePath << endl;
		return false;
	}
	if (fileStatus == STAGED) {
		cout << "file: " << filePath << " is already staged" << endl;
		return false;
	}
	struct _stat stat;
	bool _temp = _stat(filePath.string().c_str(), &stat);//_bool ?
	std::error_code ec;
	fs::path relativePath = fs::relative(filePath, mrefToRootPath, ec);
	if (ec) {
		std::cerr << "Error: cannot stage " << filePath.string() << endl;
		exit(EXIT_FAILURE);
		return false;
	}
	std::string str = relativePath.string();
	index::indexEntry currEntry = {
		.createdTime = static_cast<unsigned int>(stat.st_ctime),
		.modifiedTime = static_cast<unsigned int>(stat.st_mtime),
		.sd_dev = stat.st_dev,
		.sd_ino = stat.st_ino,
		.mode = stat.st_mode,
		.sd_uid = static_cast<unsigned  int>(stat.st_uid),
		.sd_gid = static_cast<unsigned  int>(stat.st_gid),
		.sd_size = static_cast<unsigned  int>(stat.st_size),
		.sha1 = hash,
		.flag = static_cast<unsigned int>(index::IndexFileStatus::STAGED),
		//.sha1 = hash.c_str(),
		.fileName = str
	};
	//utils::toHexEncoding(hash, currEntry.sha1);

	this->mindexEntries.push_back(currEntry);
	mtreeCount++;
	writeToFile();
	//indexfptr.write((char*)&currEntry, sizeof(index::indexEntry));
	//indexfptr.close();
	return true;
}

bool Index::hasFileChanged(const fs::path & filepath) {
	for (const auto& entry : mindexEntries) {
		if (fs::equivalent(filepath, mrefToRootPath / entry.fileName)) {
			struct _stat stat;
			bool _temp = _stat(entry.fileName.c_str(), &stat);
			if (stat.st_mtime == entry.modifiedTime) {
				return false;
			}
			return true;
		}
	}
	return false;
}

void Index::checkcout(Index & newIndex) {
	newIndex.writeToFile();//update index

	//update directory
	for (auto& newEntry : newIndex.mindexEntries) {
		fs::path fromP{ mrefToRootPath / ".dose/objects/" / newEntry.sha1.substr(0,2) / newEntry.sha1.substr(2,40 - 2) };
		fs::path toP{ mrefToRootPath / newEntry.fileName };
		std::ifstream tp{ toP };
		cout << tp.rdbuf();
		tp.close();
		std::error_code ec;
		bool _tb = fs::copy_file(fromP, toP, fs::copy_options::overwrite_existing, ec);
		cout << ec.message() << ec.value() << endl;
	}
	//delete files that are only in oldindex

	for (auto& oldEntry : this->mindexEntries) {
		bool foundMatch = false;
		fs::path filePath{ oldEntry.fileName };
		for (auto& newEntry : newIndex.mindexEntries) {
			if (filePath.compare(fs::path{ newEntry.fileName }) == 0) {
				foundMatch = true;
				break;
			}
		}
		if (foundMatch)continue;
		else {
			std::error_code ec;
			if (fs::remove(fs::current_path() / oldEntry.fileName, ec)) {
			}
			if (ec) {
				cerr << "Error: " << ec.message() << endl;
			}
		}
	}
}



//TODO: work on getFileStatus function
//TODO: work on git status function
//TODO: work on git commit function
