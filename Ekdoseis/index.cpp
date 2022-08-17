#include<algorithm>
#include "index.h"
#include "sha1.h"
#include "utils.h"

//namespace fs = std::filesystem;
using std::cerr, std::cout;
void Index::fetchFromIndex() {
	if (fs::is_empty(mindexPath)) {
		return;
	}
	std::ifstream indexiptr{ mindexPath };
	if (!indexiptr) {
		std::stringstream errmsg;
		errmsg << "Error: " << "cannot open index file" << endl;
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
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
		fs::path fileIndex{ mrefToRootPath / entry.fileName };
		bool match = fs::equivalent(mrefToRootPath / entry.fileName, filePath, ec);
		if (ec) {
			std::stringstream errmsg;
			errmsg<< "An error occured. " << ec.message();
			utils::printError(errmsg.str());
			exit(EXIT_SUCCESS);
		}
		if (match) {
			//file exists in index file
			struct _stat filestat;
			bool _temp = _stat(filePath.string().c_str(), &filestat);//do what with bool 
			if (entry.modifiedTime == filestat.st_mtime) {
				/*
				if the timestamp is same doesnot means the file is modified(Yeah my messed up design)
				Now get the hash of latest commit of the file
				and hash the content of current version
				if both hash are same then the file isnot modified
				else file is modified
				if updateindex=true
				update the index
				*/
				const std::string currHash = SHA1::from_file(filePath.string());
				const std::string prevHash = entry.sha1;
				if (currHash == prevHash) {
					//content is not modified
					if (static_cast<index::IndexFileStatus>(entry.flag) == index::IndexFileStatus::STAGED) {
						return index::FileStatus::STAGED;
					}
					return index::FileStatus::COMMITTED;
				}
				//if content is modified code below this blocck will be executed
				//TODO: check for staged or commited
			}
			//in index but modified
			//so file is changed
			//and hence change the modifed date index too and update the index
			if (updateIndex == true) {
				std::string hash{ SHA1::from_file(filePath.string()) };
				const fs::path _hashFilePath{ mrefToRootPath / ".dose/objects" / hash.substr(0,2) / hash.substr(2,40 - 2) };

				std::error_code ec;
				fs::copy_file(filePath, _hashFilePath, fs::copy_options::skip_existing, ec); //no encryption now maybe later
				if (ec) {
					std::stringstream msg;
					msg << "Error: " << ec.message() << endl;
					msg << "Error: " << "cannot perform the required action" << endl;
					utils::printError(msg.str());
					exit(EXIT_SUCCESS);
				}

				entry.modifiedTime = filestat.st_mtime;
				entry.sha1 = hash;
				entry.flag = static_cast<unsigned int>(index::IndexFileStatus::STAGED);
			}
			return  index::FileStatus::MODIFIED;
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
	//fileiptr >> entry.fileName;
	std::getline(fileiptr >> std::ws, entry.fileName);
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

		utils::ConsoleHandler handler;
		handler.setColor(utils::Color::BRIGHT_BLUE);
		cout << "nothing to add in file " << filePath << endl;
		return false;
	}
	if (fileStatus == STAGED) {
		utils::ConsoleHandler handler;
		handler.setColor(utils::Color::BRIGHT_BLUE);
		cout << "file: " << filePath << " is already staged" << endl;
		return false;
	}
	struct _stat stat;
	bool _temp = _stat(filePath.string().c_str(), &stat);//_bool ?
	std::error_code ec;
	fs::path relativePath = fs::relative(filePath, mrefToRootPath, ec);
	if (ec) {
		std::stringstream errmsg;
		errmsg<< "Error: cannot stage " << filePath.string() << endl;
		utils::printError(errmsg.str());
		

		(EXIT_SUCCESS);
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

void Index::restoreFile(const fs::path & path, bool staged) {

	if (!exists(path)) {
		cout << "Error: " << path.string() << " doesnot exists." << endl;
		return;
		//exit(EXIT_SUCCESS);
	}
	if (fs::is_directory(path)) {
		using iterator = fs::recursive_directory_iterator;
		for (iterator i = fs::recursive_directory_iterator(path);
			i != fs::recursive_directory_iterator();
			++i) {
			fs::path p{ i->path() };
			if (fs::is_directory(p)) {
				continue;
			}
			restoreFile(p);
			/*in the below else statement commit tree is created for everyfile to restore todo imporve*/
			//addFile(p);
		}
		cout << endl << endl;

	}
	else {
		/*
		if --staged
		if the file isnot staged simply return
		if the file is staged
		if the file was commited before update the index to point to commited hash
		else remove from index
		else if restore
		if the file is not staged or modified -> return
		if file is staged update index and restore file
		if file is modified restore file

		*/
		auto status = this->getFileStatus(path);
		if (staged == true && status != index::FileStatus::STAGED) {
			return;
		}
		if (staged == false && (status != index::FileStatus::STAGED && status != index::FileStatus::MODIFIED)) {
			return;
		}
		Commit commit{ this->mrefToRootPath };
		//load from latest commit
		if (!commit.loadFromCommitHash()) {
			/*
			if staged remove from Index
			if not same(nothiing to restore)
			*/
			size_t count = std::erase_if(this->mindexEntries, [path, this](auto entry) {
				return  fs::equivalent(this->mrefToRootPath / entry.fileName, path);
				});
			if (count == 1) {
				this->mtreeCount--;
				this->writeToFile();
				return;
			}
			else
				return;
		}
		else {
			//you are means the repo was commited previously 
			/*
			if staged flag:-> update index only
			----To update index:
			--------if the file is in prev commit change its hash to corr hash
			-------	if the file is not in prev commit, remove from index
			if not staged flag:-> update index and
			if the file is in prev commit , update index and its content to that of prev commit
			if the file is not in prev commit, remove from index, donot touch its content
			*/
			//the repo was commited before
			std::error_code ec;
			const fs::path relPath = fs::relative(path, mrefToRootPath, ec);
			std::string hash = commit.getTree().getHashOfFile(relPath.string());
			if (hash.empty()) {
				//file was not committed before
				std::erase_if(this->mindexEntries, [path, this](auto entry) {
					return  fs::equivalent(this->mrefToRootPath / entry.fileName, path);
					});
				this->mtreeCount--;
				this->writeToFile();
				return;
			}
			else {
				/*
				file was in prev commit
				*/
				auto found = std::find_if(mindexEntries.begin(), mindexEntries.end(), [path, this](auto entry) {
					return  fs::equivalent(this->mrefToRootPath / entry.fileName, path);
					});
				std::error_code ec;
				if (staged == false) {
					fs::copy_file(
						mrefToRootPath / ".dose/objects/" / hash.substr(0, 2) / hash.substr(2, 40 - 2),
						path,
						fs::copy_options::overwrite_existing,
						ec
					);
					if (ec) {
						cout << "An error occured while restoring the file " << ec.message() << endl;
						return;
					}
				}
				if (found != mindexEntries.end()) {
					found->sha1 = hash;
					found->flag = static_cast<int>(index::IndexFileStatus::COMMITTED);
					//MUST: The files mtime is last modified time and the file is not staged so while adding only the mtime is checked curretnly so a bug .
					// check the contents too
					//TODO: modified date
					this->writeToFile();
				}
			}
		}
	}
}

//TODO: work on getFileStatus function
//TODO: work on git status function
//TODO: work on git commit function
