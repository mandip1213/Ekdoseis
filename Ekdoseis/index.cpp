#include "index.h"

//namespace fs = std::filesystem;
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

index::FileStatus Index::getFileStatus(const fs::path& filePath) {
	std::error_code ec;
	for (struct index::indexEntry entry : mindexEntries) {
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

void Index::writeToFile(std::ofstream & fileoptr, const index::indexEntry & entry) {
	for (auto entry:mindexEntries) {
	fileoptr.flush();//debug
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
	fileoptr.flush();//debug
	}
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
	++mtreeCount;
	fileoptr.flush();//debug
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
	fileiptr >> entry.sha1;
	fileiptr >> entry.fileName;
	//fileiptr.ignore(1);//new line
}

bool Index::add(const fs::path & filePath, const std::string & hash) {
	using enum index::FileStatus;
	//latestFetch = true;
	if (!latestFetch) {
		fetchFromIndex();
	}
	index::FileStatus fileStatus = getFileStatus(filePath);
	if (!(fileStatus == MODIFIED || fileStatus == UNTRACKED)) {
		return true;//return file is uptodate(staged or committed)
	}
	std::ofstream indexfptr{ mindexPath };
	cout << endl << indexfptr.tellp() << endl;
	struct _stat stat;
	bool _temp = _stat(filePath.string().c_str(), &stat);//_bool ?
	std::error_code ec;
	fs::path relativePath = fs::relative(filePath, mrefToRootPath, ec);
	if (ec) {
		std::cerr << "Error: cannot stage " << filePath.string() << endl;
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
		.flag = static_cast<int>(index::IndexFileStatus::STAGED),
		.fileName = str
	};
	utils::toHexEncoding(hash, currEntry.sha1);

	indexfptr << mtreeCount + 1 << '\n';
	indexfptr.flush();//debug
	writeToFile(indexfptr, currEntry);
	//indexfptr.write((char*)&currEntry, sizeof(index::indexEntry));
	indexfptr.close();
	return true;
}

//TODO: work on getFileStatus function
//TODO: work on git status function
//TODO: work on git commit function
//TODO: solve bug on hexa encoded hash string while reading from file  (may be use binary format
