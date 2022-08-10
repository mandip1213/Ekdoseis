#include<iostream>
#include<fstream>
#include<chrono>


#include"sha1.h"

#include"index.h"
#include"commit.h"
#include"utils.h"

#define endl '\n'
using std::cerr, std::cout;

Commit::Commit(const fs::path& rootPath)
	:mrootPath{ rootPath }, mtree{ "" }
{
	std::ifstream headptr{ mrootPath / ".dose/HEAD" };
	std::string _tempstr, refpath;
	headptr >> _tempstr >> refpath;
	mrefPath = mrootPath / ".dose" / refpath;
}

void Commit::compareTreeHash() {
	//if(string.compare())
	const std::string currHash{ mtree.getHash() };
	std::string prevHash;
	fs::path prevPath{ mrootPath / ".dose/objects" / mparentHash.substr(0,2) / mparentHash.substr(2,40 - 2) };
	std::ifstream prevCommit{ prevPath };
	prevCommit >> prevHash;//"tree"
	prevCommit >> prevHash;//tree hash
	if (currHash.compare(prevHash) == 0) {
		//bug: this runs when already tracked files are committed
		cout << "Nothinig to commit" << endl;
		cout << "Please add files before committing" << endl;
		exit(EXIT_SUCCESS);
	}
	return;
}
void Commit::createCommit() {
	compareTreeHash();//if same program will exit

	std::stringstream streambuf;
	mcoommitTime = utils::getTime();
	//todo: add author committer and time
	//commit message
	streambuf << "tree " << mtree.getHash() << '\n';
	streambuf << "parent " << mparentHash << '\n';
	//streambuf << "author " << "ShakesPeare" << "time "<<"timezone"<<'\n';
	streambuf << "committer " << "ShakesPeare" << mcoommitTime << '\n';
	streambuf << mmessage << '\n';

	SHA1 hash;
	hash.update(streambuf);
	mhash = hash.final();

	fs::path commitObj{ fs::current_path() / ".dose/objects" / mhash.substr(0,2) };
	ReturnFlag _rf = utils::createObjectDir(commitObj);
	if (!(_rf == CREATE_SUCCESS || _rf == ALREADY_EXISTS)) {
		cerr << "Error: " << "cannot perform the required action" << endl;
		exit(EXIT_FAILURE);
	}

	std::ofstream ofptr{ commitObj / mhash.substr(2,40 - 2) };
	ofptr << streambuf.str();
	ofptr.close();

	if (minitialCommit) {
		createLogFiles();
	}
	updateLogs();
	updateHead();
}
void Commit::createLogFiles() {
	ReturnFlag _rf = utils::createObjectDir(mrootPath / ".dose/logs");
	if (_rf == CREATE_FAILURE) {
		cerr << "Error: cannot update logs";
		exit(EXIT_FAILURE);
	}
	_rf = utils::createObjectDir(mrootPath / ".dose/logs/refs");
	if (_rf == CREATE_FAILURE) {
		cerr << "Error: cannot update logs";
		exit(EXIT_FAILURE);
		std::ofstream newFile{ mrootPath / ".dose/logs/refs/main" };
	}
}
void Commit::updateHead() {
	std::ofstream headptr{ mrefPath };
	headptr << mhash;
	cout << "log:  " << mhash << " ref updated" << endl;
}
void Commit::createTree() {
	Index index{ mrootPath };
	index.fetchFromIndex();
	std::error_code ec;

	bool isallCommited = !std::any_of(index.mindexEntries.begin(), index.mindexEntries.end(), [](auto curr) {
		return static_cast<index::IndexFileStatus>(curr.flag) == index::IndexFileStatus::STAGED;
		});
	if (isallCommited) {
		cout << "Nothing to commit. All files up to date" << endl;
		cout << "Please add files with `dose add [files]` before commiting" << endl;
		exit(EXIT_SUCCESS);
	}
	for (auto& entry : index.mindexEntries) {
		fs::path relativePath = entry.fileName;
		if (ec) {
			std::cerr << "Error: cannot commit " << endl;
			exit(EXIT_FAILURE);
		}
		mtree.insertBlob(entry.sha1, relativePath);//insert the blob into appropriate location in the tree
		entry.flag = static_cast<unsigned int>(index::IndexFileStatus::COMMITTED);
	}
	index.writeToFile();//updating index with flag set to commited
	//create a tree object according to the tree data strucutre and 	index.writeToFile();
	mtree.createTreeObjects();
}


void Commit::fetchParentHash() {
	std::ifstream logFile{ mrootPath / ".dose/logs/refs/main" };
	if (!logFile) {
		if (fs::exists(mrootPath / ".dose/logs/refs/main")) {
			cerr << "Error: cannot commit" << endl;
			exit(EXIT_FAILURE);
		}
		minitialCommit = true;
		mparentHash = "0000000000000000000000000000000000000000";//todo: do it better
		return;
	}
	std::string line;
	while (std::getline(logFile, line)) {
		//extracts line till it reach the end of file
		//The line before eof will be the end of line 
		if (logFile.peek() == EOF) {
			break;
		}
	};
	std::istringstream sstream{ line };
	sstream >> mparentHash;//parent of parent hash
	sstream >> mparentHash;///required parent 
	utils::validateHash(mparentHash);
}

void tempexit() {
	cerr << "Error: cannot update llogs" << endl;
	exit(EXIT_FAILURE);
}
void Commit::updateLogs() {
	//replace: main with branch name
	std::ofstream logFile{ mrootPath / ".dose/logs/refs/main",std::ios::app };
	if (!logFile) {
		cerr << "Error: couldnot open log files" << endl;
		tempexit();
	}
	logFile << mparentHash << " " << mhash << " " << mcoommitTime << " author " << mmessage << endl;//add time, author and message
}


bool Commit::loadFromCommitHash(const std::string& hash) {
	if (hash.empty()) {
		/*load from latest commit*/
		std::ifstream refstream{ mrefPath };
		refstream >> mparentHash;
		if (mparentHash.empty()) {
			//no commit has been made yet so its empty
			return false;
		}
	}
	else {
		mparentHash = hash;
	}
	//create the tree recursively 
	std::ifstream commitptr{ mrootPath / ".dose/objects" / mparentHash.substr(0,2) / mparentHash.substr(2,40 - 2) };
	std::string treehash;
	commitptr >> treehash >> treehash;
	mtree = Tree("", treehash);
	mtree.createTreeFromObject();
	return true;
}

