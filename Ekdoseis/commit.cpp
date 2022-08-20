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
	if (!fs::exists(mrefPath)) {
		if (_tempstr.size() == 40) {
			utils::printError("Cannot create a commit in detach mode\n");
			cout << "Please switch to specific branch to create a commit\n";
			cout << "You can create a branch at current commmit with: ";
			cout << "`dose branch <branch-name>\n";
		}
		else {
			utils::printError("Illegal reference in HEAD");
		}
		exit(EXIT_FAILURE);

	}
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

		utils::ConsoleHandler handler;
		handler.setColor(utils::Color::BRIGHT_YELLOW);
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
	streambuf << "committer " << "ShakesPeare " << mcoommitTime << '\n';
	streambuf << mmessage << '\n';

	SHA1 hash;
	hash.update(streambuf);
	mhash = hash.final();

	fs::path commitObj{ fs::current_path() / ".dose/objects" / mhash.substr(0,2) };
	ReturnFlag _rf = utils::createObjectDir(commitObj);
	if (!(_rf == CREATE_SUCCESS || _rf == ALREADY_EXISTS)) {
		std::stringstream errmsg;
		errmsg << "Error: " << "cannot perform the required action" << endl;
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
	}

	std::ofstream ofptr{ commitObj / mhash.substr(2,40 - 2) };
	ofptr << streambuf.str();
	ofptr.close();

	if (minitialCommit) {
		createLogFiles();
	}
	updateLogs();
	updateHeadRef();
}

void Commit::createLogFiles() {
	ReturnFlag _rf = utils::createObjectDir(mrootPath / ".dose/logs");
	if (_rf == CREATE_FAILURE) {
		std::stringstream errmsg;
		errmsg << "Error: cannot update logs";
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
	}
	_rf = utils::createObjectDir(mrootPath / ".dose/logs/refs");
	if (_rf == CREATE_FAILURE) {
		std::stringstream errmsg;
		errmsg << "Error: cannot update logs";
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
		std::ofstream newFile{ mrootPath / ".dose/logs/refs" / mrefPath.filename() };
	}
}

void Commit::updateHeadRef() {
	std::ofstream headptr{ mrefPath };
	cout << "updatin head " << mrefPath << endl;
	headptr << mhash;
	//cout << "log:  " << mhash << " ref updated" << endl;
}

void Commit::createTree() {
	Index index{ mrootPath };
	index.fetchFromIndex();
	std::error_code ec;

	bool isallCommited = !std::any_of(index.mindexEntries.begin(), index.mindexEntries.end(), [](auto curr) {
		return static_cast<index::IndexFileStatus>(curr.flag) == index::IndexFileStatus::STAGED;
		});
	if (isallCommited) {
		utils::ConsoleHandler handler;
		handler.setColor(utils::Color::BRIGHT_RED);
		cout << "Nothing to commit. All files up to date" << endl;
		cout << "Please add files with `dose add [files]` before commiting" << endl;
		handler.resetColor();
		exit(EXIT_SUCCESS);
	}
	for (auto& entry : index.mindexEntries) {
		fs::path relativePath = entry.fileName;
		if (ec) {
			std::stringstream errmsg;
			errmsg << "Error: cannot commit " << endl;
			utils::printError(errmsg.str());
			exit(EXIT_SUCCESS);
		}
		mtree.insertBlob(entry.sha1, relativePath);//insert the blob into appropriate location in the tree
		entry.flag = static_cast<unsigned int>(index::IndexFileStatus::COMMITTED);
	}
	index.writeToFile();//updating index with flag set to commited
	//create a tree object according to the tree data strucutre and 	index.writeToFile();
	mtree.createTreeObjects();
}


void Commit::fetchParentHash() {
	std::ifstream logFile{ mrootPath / ".dose/logs/refs" / mrefPath.filename() };
	if (!logFile) {
		if (fs::exists(mrootPath / ".dose/logs/refs" / mrefPath.filename())) {
			std::stringstream errmsg;
			errmsg << "Error: cannot commit" << endl;
			utils::printError(errmsg.str());
			exit(EXIT_SUCCESS);
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
	std::stringstream errmsg;
	errmsg << "Error: cannot update llogs" << endl;
	utils::printError(errmsg.str());
	exit(EXIT_SUCCESS);
}

void Commit::updateLogs() {
	//replace: main with branch name
	/* TODO MUST : refactor : in logfile object*/
	std::ofstream logFile{ mrootPath / ".dose/logs/refs" / mrefPath.filename(),std::ios::app };
	cout << "updating log" << mrefPath.filename() << endl;
	if (!logFile) {
		std::stringstream errmsg;
		errmsg << "Error: couldnot open log files" << endl;
		utils::printError(errmsg.str());
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


void Commit::setParenthash(const std::string& hash) {
	mparentHash = hash;
}

void Commit::setTree(const Tree& tree) {
	mtree = std::move(tree);
}
