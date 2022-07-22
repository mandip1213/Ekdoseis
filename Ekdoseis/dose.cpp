#include<iostream>
#include<fstream>
#include<windows.h>//for fileapi.h
#include<fileapi.h>
#include"dose.h"
#include"commit.h"
#include"utils.h"
#define endl '\n'
//0- root command name//dose 
//1- location from which comman is called
//2-main command (add, init,commit ..)

using std::cout;
Dose::Dose(int n, const char* ptr[]) :margc{ n }, margv{ ptr } {

}
std::ostream& operator<<(std::ostream& out, const Dose& dose) {
	out << " command 'dose " << dose.margv[2] << "' ";

	return out;
}
#include "SHA1.h"
Dose& Dose::parseRootCommand() {
	const char* arg_path = margv[1];
	const char* arg_cmd = margv[2];
	mrootPath = fs::path{ arg_path };
	if (strcmp(arg_cmd, "init") == 0) {
		mcommand = INIT;
	}
	else if (strcmp(arg_cmd, "add") == 0) {
		mcommand = ADD;
	}
	else if (strcmp(arg_cmd, "commit") == 0) {
		mcommand = COMMIT;
	}
	else if (strcmp(arg_cmd, "status") == 0) {
		mcommand = STATUS;
	}
	else if (strcmp(arg_cmd, "log") == 0) {
		mcommand = LOG;
	}
	else if (strcmp(arg_cmd, "checkout") == 0) {
		mcommand = CHECKOUT;
	}
	else {
		cout << "Error:" << *this << "doesnot exist" << endl;
		exit(EXIT_FAILURE);
	}
	fs::current_path(arg_path);//change current path to arg_path
	return *this;
}
Dose& Dose::execCommand() {
	switch (mcommand) {
	case INIT:init(); break;
	case ADD:add(); break;
	case COMMIT: commit(); break;
	case STATUS: status(); break;
	case LOG: log(); break;
	case CHECKOUT: checkout(); break;
	}
	return *this;
}
ReturnFlag Dose::createDirectory(const std::string_view& dirName, CreateFlag flags) {
	fs::path newDir = mrootPath / dirName;//bug?
	if (fs::exists(newDir)) {
		if (flags == NO_OVERRIDE) {
			return ALREADY_EXISTS;
		}
		else if (flags == OVERRIDE_IF_EXISTS) {
			if (!remove(newDir)) {
				return OVERRIDE_FAILURE;
			}
		}
	}
	bool _success = create_directory(newDir);
	if (flags == NO_OVERRIDE) {
		return (_success ? CREATE_SUCCESS : CREATE_SUCCESS);
	}
	else	if (flags == OVERRIDE_IF_EXISTS) {
		return (_success ? OVERRIDE_SUCCESS : OVERRIDE_SUCCESS);
	}
	return CREATE_SUCCESS;
}
void Dose::errorExit() {
	cout << *this << "An error occured" << endl;
	exit(EXIT_FAILURE);
}

Dose& Dose::init() {
	//LINK:https://git-scm.com/docs/git-init
	cout << "initializing empty ekdoseis repo in " << mrootPath << endl;
	ReturnFlag _rflag = createDirectory(".dose");
	if (_rflag == ALREADY_EXISTS) {
		cout << "Ekdoseis is already initialized in " << mrootPath << endl;
		return *this;
	}
	if (_rflag == CREATE_FAILURE) {
		cout << "Error: couldnot initialize Ekdoseis repo on " << mrootPath << endl;
		exit(EXIT_FAILURE);
	}
	bool _s = SetFileAttributesA((mrootPath / ".dose").string().c_str(), FILE_ATTRIBUTE_HIDDEN);//for hiding the folder
	ReturnFlag _r1 = createDirectory(".dose/objects");
	if (_r1 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r2 = createDirectory(".dose/refs");
	if (_r2 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r3 = createDirectory(".dose/refs/heads");
	if (_r3 != CREATE_SUCCESS)errorExit();
	std::ofstream _headf{ (mrootPath / ".dose/refs/heads/main") };
	if (!(_headf))errorExit();
	std::ofstream headptr{ mrootPath / ".dose/HEAD" };
	if (!headptr)errorExit();
	headptr << "ref: refs/heads/main" << endl;
	_headf.close();
	headptr.close();
	cout << "Successfully Initialized empty repo in" << *this << endl;

	return *this;
}
Dose& Dose::commit() {


	if (margc >= 5) {
		if (!strcmp(margv[4], "-m")) {
			std::cerr << "Error: Invalid Command" << endl;
		}
	}
	else {
		std::cerr << "Error: Not enough arguments" << endl;
	}
	Commit commit{ mrootPath };
	commit.setMessage(margv[4]);
	commit.createTree();
	commit.fetchParentHash();
	commit.createCommit();
	//	commit.compareTreeHash();//todo: instead of this  check the flag of every file of index
	std::error_code ec;
	return *this;
}
Dose& Dose::status() {
	using iterator = fs::recursive_directory_iterator;
	//for (auto& curr : fs::recursive_directory_iterator(p)) {
		//cout << curr << endl;
	//}
	doseIgnore = DoseIgnore(mrootPath);//we only need it now
	mindex = Index(mrootPath);
	mindex.fetchFromIndex();
	std::vector<fs::path> modified;
	std::vector<fs::path>untracked;
	std::string symbol("|___");
	for (iterator i = fs::recursive_directory_iterator(mrootPath);
		i != fs::recursive_directory_iterator();
		++i) {
		fs::path p{ i->path() };
		if (doseIgnore.has(p)) {
			i.disable_recursion_pending();
			continue;
		}
		if (fs::equivalent(p, mrootPath / ".dose")) {
			//cout << "Dose found" << endl;
			i.disable_recursion_pending();
			continue;
		}
		if (fs::equivalent(p, mrootPath / ".git")) {
			//cout << "Dose found" << endl;
			i.disable_recursion_pending();
			continue;
		}

		if (fs::is_directory(p)) {
			continue;
		}

		using namespace index;
		index::FileStatus status = mindex.getFileStatus(i->path());
		if (status == FileStatus::STAGED) {
		}
		if (status == FileStatus::UNTRACKED) {
			untracked.push_back(i->path());
		}
		if (status == FileStatus::MODIFIED) {
			untracked.push_back(i->path());
		};

		if (i.depth() != 0) {
			std::cout << std::string((i.depth() - 1) * 4, ' ');
			std::cout << symbol;
		}
		std::cout << i->path().filename() << "    " << endl;
		//cout << i->path()<<endl;
	}
	cout << endl << endl;
	cout << "modified: " << endl;
	for (auto it : modified) {
		std::cout << it.filename() << "    " << endl;
	}
	cout << endl << endl;
	cout << "untracked: " << endl;
	for (auto it : untracked) {
		std::cout << it.filename() << "    " << endl;
	}
	return *this;
}
Dose& Dose::log() {
	cout << "Logging: " << endl;

	std::ifstream logFile{ mrootPath / ".dose/logs/refs/main" };
	if (!logFile) {
		cout << "NO logs";
		return *this;
	}

	std::string line;
	std::string hash, author, message;
	uint64_t time;//time are stroed in 
	while (std::getline(logFile, line)) {
		std::istringstream stream{ line };
		stream.ignore(41, ' ');
		stream >> hash >> time >> author >> message;
		cout << "Commit\t" << hash << endl;
		cout << "Author:\t" << author << endl;
		cout << "Date: ";
		utils::printDate(time);
		cout << endl;
		cout << "Message:\t" << message << endl;
	}
	return *this;
}
void Dose::addFile(const fs::path& currPath) {
	if (!exists(currPath)) {
		cout << "Error: " << currPath.string() << " doesnot exists." << endl;
		return;
		//exit(EXIT_FAILURE);
	}
	if (doseIgnore.has(currPath)) {
		cout << " debug Error: " << currPath << " already exists in the doseignore." << endl;
		return;
		//exit(EXIT_FAILURE);
	}
	if (fs::is_directory(currPath)) {
		using iterator = fs::recursive_directory_iterator;
		for (iterator i = fs::recursive_directory_iterator(currPath);
			i != fs::recursive_directory_iterator();
			++i) {
			fs::path p{ i->path() };
			if (doseIgnore.has(p)) {
				i.disable_recursion_pending();
				continue;
			}
			if (fs::is_directory(p)) {
				continue;
			}
			addFile(p);
			//using namespace index;
			//index::FileStatus status = mindex.getFileStatus(i->path());
			//std::cout << i->path().filename() << "    " << endl;
			//cout << i->path()<<endl;
		}
		cout << endl << endl;

	}
	else {
		SHA1 fileSHA;
		const fs::path objectPath = mrootPath / ".dose/objects";
		std::string hash{ fileSHA.from_file(currPath.string()) };
		size_t   hashLength{ hash.length() };//read: size_t
		if (!(hashLength == 40)) {
			std::cerr << "Error: " << "Unwanted hash length" << endl;
		}

		const fs::path hashDirPath = objectPath / hash.substr(0, 2);
		ReturnFlag _rf1 = createDirectory(hashDirPath.string());
		if (!(_rf1 == CREATE_SUCCESS || _rf1 == ALREADY_EXISTS)) {
			cout << "Error: " << "cannot perform the required action" << endl;
			exit(EXIT_FAILURE);
		}

		const fs::path _hashFilePath{ hashDirPath / hash.substr(2,hashLength - 2) };
		std::error_code ec;

		fs::copy_file(currPath, _hashFilePath, fs::copy_options::skip_existing, ec); //no encryption now maybe later
		if (ec) {
			cout << "Error: " << ec.message() << endl;
			cout << "Error: " << "cannot perform the required action" << endl;
			exit(EXIT_FAILURE);
		}
		bool badd = mindex.add(currPath, hash);
		if (badd) {
			cout << "File: " << currPath << " staged successfully" << endl;
		}
	}
}

Dose& Dose::add() {
	//check for git init
	if (!exists(mrootPath / ".dose/index")) {
		std::ofstream _headptr{ mrootPath / ".dose/index" };
		_headptr.close();
	}
	doseIgnore = DoseIgnore(mrootPath);//we only need it now
	mindex = Index(mrootPath);
	mindex.fetchFromIndex();

	for (int i = 3; i < margc; i++) {
		const fs::path _path = mrootPath / margv[i];
		//if(isFile)//todo
		addFile(_path);
		cout << _path << endl;
	}
	return *this;
}
Dose& Dose::checkout() {

	std::string checkoutPoint{ margv[3] };

	mindex = Index{ mrootPath };
	mindex.fetchFromIndex();
	if (isBranch(checkoutPoint)) {
		//implement if implemented branching
	}
	else if (isValidCommit(checkoutPoint)) {
		const fs::path  objPath = mrootPath / ".dose/objects" / checkoutPoint.substr(0, 2) / checkoutPoint.substr(2, 40 - 2);
		std::ifstream commitiptr{ objPath };
		std::string treehash;
		commitiptr >> treehash >> treehash;
		commitiptr.close();
		const fs::path treePath{ mrootPath / ".dose/obejcts" / treehash.substr(0,2) / treehash.substr(2,40 - 2) };
		Tree newTree{ "",treehash };
		newTree.createTreeFromObject();
		Index newIndex{ mrootPath };
		newTree.createNewIndex(newIndex);//passed as reference
		this->mindex.checkcout(newIndex);

		//for (auto& entries : newIndex.mindexEntries)
	}



	return *this;
}
bool Dose::isValidCommit(const std::string& ch_point) {
	const fs::path dirPath = mrootPath / ".dose/objects" / ch_point.substr(0, 2);
	const fs::path requiredPath = dirPath / ch_point.substr(2, 40 - 2);
	using iterator = fs::recursive_directory_iterator;
	for (iterator i = fs::recursive_directory_iterator(mrootPath / ".dose/objects");
		i != fs::recursive_directory_iterator();
		++i) {
		//refactor into normal iterator
		fs::path tmp = i->path();
		std::error_code ec;
		if (!fs::equivalent(i->path(), dirPath, ec)) {
			i.disable_recursion_pending();
		}
		if (fs::equivalent(i->path(), requiredPath, ec)) {
			cout << "found the checkout point";
			//sutffs
			return true;
			break;
		}
	}
	return false;
}

bool Dose::isBranch(const std::string& ch_point) {
	//for (auto const& currBranch : fs::directory_iterator{ mrootPath / ".dose/refs/heads" }) {
		//cout << currBranch.file_name<<endl;
		//cout << "df";
	//}
	return false;
}

//TODO:
//show corresponding commit message if the file is up to date
//set file attribute while copying to hashed objects and vice versa
//cannot add multiple files at once//work on deleting and creating dir while checking out
//


