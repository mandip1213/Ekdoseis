#include<iostream>
#include<fstream>
#include<sstream>
#include<windows.h>//for fileapi.h
#include<fileapi.h>
#include<algorithm>
#include"dose.h"
#include"commit.h"
#include"utils.h"
#include"branch.h"
#include"merge.h"
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
	else if (strcmp(arg_cmd, "restore") == 0) {
		mcommand = RESTORE;
	}
	else if (strcmp(arg_cmd, "log") == 0) {
		mcommand = LOG;
	}
	else if (strcmp(arg_cmd, "checkout") == 0) {
		mcommand = CHECKOUT;
	}
	else if (strcmp(arg_cmd, "branch") == 0) {
		mcommand = BRANCH;
	}
	else if (strcmp(arg_cmd, "merge") == 0) {
		mcommand = MERGE;
	}
	else {
		std::stringstream errmsg;
		errmsg << "Error:" << *this << "doesnot exist" << endl;
		utils::printError(errmsg.str());
		exit(EXIT_SUCCESS);
	}
	fs::current_path(arg_path);//change current path to arg_path
	return *this;
}
Dose& Dose::execCommand() {
	switch (mcommand) {
	case NOCOMMAND:break;
	case INIT:init(); break;
	case ADD:add(); break;
	case COMMIT: commit(); break;
	case STATUS: status(); break;
	case LOG: log(); break;
	case CHECKOUT: checkout(); break;
	case RESTORE:restore(); break;
	case BRANCH:branch(); break;
	case MERGE: merge(); break;
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
	exit(EXIT_SUCCESS);
}

Dose& Dose::init() {
	const std::string defaultBranchName = "main";
	//LINK:https://git-scm.com/docs/git-init

	utils::ConsoleHandler handler;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "initializing empty ekdoseis repo in " << mrootPath << endl;
	ReturnFlag _rflag = createDirectory(".dose");
	if (_rflag == ALREADY_EXISTS) {
		cout << "Ekdoseis is already initialized in " << mrootPath << endl;
		return *this;
	}
	if (_rflag == CREATE_FAILURE) {
		cout << "Error: couldnot initialize Ekdoseis repo on " << mrootPath << endl;
		exit(EXIT_SUCCESS);
	}

	handler.resetColor();
	bool _s = SetFileAttributesA((mrootPath / ".dose").string().c_str(), FILE_ATTRIBUTE_HIDDEN);//for hiding the folder
	ReturnFlag _r1 = createDirectory(".dose/objects");
	if (_r1 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r2 = createDirectory(".dose/refs");
	if (_r2 != CREATE_SUCCESS)errorExit();
	ReturnFlag _r3 = createDirectory(".dose/refs/heads");
	if (_r3 != CREATE_SUCCESS)errorExit();
	std::ofstream _headf{ (mrootPath / ".dose/refs/heads" / defaultBranchName) };
	if (!(_headf))errorExit();
	std::ofstream headptr{ mrootPath / ".dose/HEAD" };
	if (!headptr)errorExit();
	headptr << "ref: refs/heads/" << defaultBranchName << endl;
	_headf.close();
	headptr.close();
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "Successfully Initialized empty repo in" << *this << endl;
	handler.resetColor();
	return *this;
}

Dose& Dose::commit() {

	utils::ConsoleHandler handler;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	if (margc >= 5) {
		if (!strcmp(margv[4], "-m")) {
			std::cerr << "Error: Invalid Command" << endl;
		}
	}
	else {
		std::cerr << "Error: Not enough arguments" << endl;
	}
	handler.resetColor();
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
	std::vector<fs::path> staged;
	std::vector<fs::path>untracked;
	std::vector<fs::path>committed;
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
		if (fs::exists(mrootPath / ".git")) {

			if (fs::equivalent(p, mrootPath / ".git")) {
				//cout << "Dose found" << endl;
				i.disable_recursion_pending();
				continue;
			}
		}
		if (fs::is_directory(p)) {
			continue;
		}

		using namespace index;
		index::FileStatus status = mindex.getFileStatus(i->path());
		if (status == FileStatus::STAGED) {
			staged.push_back(fs::relative(p, mrootPath));
		}
		else if (status == FileStatus::COMMITTED) {
			committed.push_back(fs::relative(p, mrootPath));
		}
		else if (status == FileStatus::UNTRACKED) {
			untracked.push_back(fs::relative(p, mrootPath));
		}
		else if (status == FileStatus::MODIFIED) {
			modified.push_back(fs::relative(p, mrootPath));
		};

		if (i.depth() != 0) {
			//std::cout << std::string((i.depth() - 1) * 4, ' ');
			// std::cout << symbol;
		}
		//std::cout << p.filename() << "    " << endl;
		//cout << i->path()<<endl;
	}
	utils::ConsoleHandler handler;
	cout << endl << endl;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "committed: " << endl;
	handler.setColor(utils::Color::BRIGHT_YELLOW);
	for (auto it : committed) {
		std::cout << it << "    " << endl;
	}
	cout << endl << endl;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "staged: " << endl;
	handler.setColor(utils::Color::BRIGHT_YELLOW);
	for (auto it : staged) {
		std::cout << it << "    " << endl;
	}
	cout << endl << endl;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "modified: " << endl;
	handler.setColor(utils::Color::BRIGHT_YELLOW);
	for (auto it : modified) {
		std::cout << it << "    " << endl;
	}
	cout << endl << endl;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	cout << "untracked: " << endl;
	handler.setColor(utils::Color::BRIGHT_YELLOW);
	for (auto it : untracked) {
		std::cout << it << "    " << endl;
	}
	handler.resetColor();
	return *this;
}
Dose& Dose::log() {

	/*
	- todo: implement for detached head state
	*/

	std::ifstream headfptr{ mrootPath / ".dose/HEAD" };
	std::string reference;
	std::getline(headfptr, reference);
	if (reference.starts_with("ref: refs/heads/")) {
		reference.erase(0, std::string("ref: refs/heads/").size());
	}
	else {
		/* HEAD is in detahced state */
		cout << "HEAD is in detached state " << endl;
		/*MUST change the way of logging*/
	}
	std::ifstream logFile{ mrootPath / ".dose/logs/refs" / reference };
	if (!logFile) {
		cout << "NO logs";
		return *this;
	}

	std::string line;
	std::string hash, author, message;
	uint64_t time;//time are stroed in 

	utils::ConsoleHandler handler;
	while (std::getline(logFile, line)) {
		std::istringstream stream{ line };
		stream.ignore(41, ' ');
		stream >> hash >> time >> author;
		std::getline(stream >> std::ws, message);

		handler.setColor(utils::Color::BRIGHT_YELLOW);
		cout << std::left << std::setw(10) << "Commit:";
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << hash << endl;

		handler.setColor(utils::Color::BRIGHT_YELLOW);
		cout << std::left << std::setw(10) << "Author:";
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << author << endl;

		handler.setColor(utils::Color::BRIGHT_YELLOW);
		cout << std::left << std::setw(10) << "Date:";
		handler.setColor(utils::Color::BRIGHT_GREEN);
		utils::printDate(time);
		cout << endl;

		handler.setColor(utils::Color::BRIGHT_YELLOW);
		cout << std::left << std::setw(10) << "Message:";
		handler.setColor(utils::Color::BRIGHT_GREEN);
		cout << message << endl;
		cout << endl << endl;
	}
	handler.resetColor();
	return *this;
}
void Dose::addFile(const fs::path& currPath) {

	utils::ConsoleHandler handler;
	handler.setColor(utils::Color::BRIGHT_GREEN);
	if (!exists(currPath)) {
		cout << "Error: " << currPath.string() << " doesnot exists." << endl;
		return;
		//exit(EXIT_SUCCESS);
	}
	if (doseIgnore.has(currPath)) {
		cout << " debug Error: " << currPath << " already exists in the doseignore." << endl;
		return;
		//exit(EXIT_SUCCESS);
	}

	handler.resetColor();
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
				//skip (files inside it will be handled by recursive iteration)
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
			exit(EXIT_SUCCESS);
		}

		const fs::path _hashFilePath{ hashDirPath / hash.substr(2,hashLength - 2) };
		std::error_code ec;

		fs::copy_file(currPath, _hashFilePath, fs::copy_options::skip_existing, ec); //no encryption now maybe later
		if (ec) {
			cout << "Error: " << ec.message() << endl;
			cout << "Error: " << "cannot perform the required action" << endl;
			exit(EXIT_SUCCESS);
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
		this->addFile(_path);
		cout << _path << endl;
	}
	return *this;
}

Dose& Dose::checkout() {

	/*
	if the checkout point is branch
	if the checkout pointis branchless commit f
	To checkout:
	-- create a tree corresponding to given commit
	-- update the index and update the working directroy
	-- update the HEAD
	*/

	const 	std::string checkoutPoint{ margv[3] };
	std::string checkoutCommit;

	mindex = Index{ mrootPath };
	mindex.fetchFromIndex();
	cout << checkoutPoint << " c" << endl;
	if (Branch::isBranch(mrootPath, checkoutPoint)) {
		//cout << "YEs man this is my branch" << endl;
		std::ifstream branchfptr{ mrootPath / ".dose/refs/heads" / checkoutPoint };
		branchfptr >> checkoutCommit;
		//TODO BRANCH
		//implement if implemented branching
	}
	else {
		checkoutCommit = checkoutPoint;
	}

	if (isValidCommit(checkoutCommit)) {
		const fs::path  objPath = mrootPath / ".dose/objects" / checkoutCommit.substr(0, 2) / checkoutCommit.substr(2, 40 - 2);
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
		Dose::updateHead(mrootPath, checkoutPoint);
		//for (auto& entries : newIndex.mindexEntries)
	}

	return *this;
}
bool Dose::isValidCommit(const std::string& ch_point) {
	if (ch_point.size() != 40)
		return false;
	const fs::path dirPath = mrootPath / ".dose/objects" / ch_point.substr(0, 2);
	const fs::path requiredPath = dirPath / ch_point.substr(2, 40 - 2);
	if (fs::exists(requiredPath)) {
		return true;
	}
	/*
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
		}
	}
	*/
	return false;
}

Dose& Dose::restore() {
	//0 command 1 location 2 root command

	if (strcmp(margv[3], "--staged") == 0) {
		//unstage file from staging area
		mindex = Index(mrootPath);
		mindex.fetchFromIndex();
		for (int i = 4; i < margc; i++) {
			const fs::path _path = mrootPath / margv[i];
			mindex.restoreFile(mrootPath / margv[i], true);
		}
	}
	else {
		//restore the modified content of the file to that of latest commit
		//TODO
		mindex = Index(mrootPath);
		mindex.fetchFromIndex();
		for (int i = 3; i < margc; i++) {
			mindex.restoreFile(mrootPath / margv[i], false);
		}


	}

	return *this;
}

void Dose::updateHead(const fs::path& rootPath, const std::string& reference) {
	std::ofstream headfptr{ rootPath / ".dose/HEAD" };
	if (!headfptr) {
		utils::printError("Head donot exist");
		exit(EXIT_SUCCESS);
		//TODO: MUST if head donot exist undo all the actions and exit 
	}
	if (Branch::isBranch(rootPath, reference))
	{
		headfptr << "ref: refs/heads/" << reference;
		return;
	}
	headfptr << reference;
}

Dose& Dose::branch() {
	//if(margc>5)
	if (margc == 3 || strcmp(margv[3], "--list") == 0 || strcmp(margv[3], "-l") == 0 && margc < 5) {
		Branch::listBranch(mrootPath);

	}
	else if (strcmp(margv[3], "--delete") == 0 || strcmp(margv[3], "-d") == 0) {
		if (margc == 4) {
			utils::printError("ERROR: branch name is required\n");
		}
		for (int i = 4; i < margc; i++) {
			Branch branch{ mrootPath,margv[i] };
			branch.deleteBranch();
		}
	}
	else if (*margv[3] != '-') {
		if (margc > 5) {
			//exit
			exit(EXIT_SUCCESS);
		}
		const std::string branchName = margv[3];
		const bool isValidBranchName = std::all_of(branchName.begin(), branchName.end(), [](char c) {
			return std::isalpha(c);
			});
		if (!isValidBranchName) {
			std::stringstream errmsg;
			errmsg << "'" << branchName << "'" << " is not a valid branch Name" << endl;
			utils::printError(errmsg.str());
			exit(EXIT_SUCCESS);
		}
		Branch branch{ mrootPath, branchName };
		if (margc == 4)
			branch.createBranch();
		else  if (margc == 5)
			branch.createBranch(margv[4]);
		else {

		}
	}
	else {
		const std::array<utils::StringColorPair, 2> arr{ {
			{"ERROR: Illegal Argument: ", utils::Color::BRIGHT_RED},
			{std::string{"'"} + margv[3] + "'",utils::Color::BRIGHT_YELLOW},
			} };
		utils::printColorful(arr);
	}
	return *this;
}

Dose& Dose::merge() {
	if (margc != 4) {
		utils::printError("Invalid args\n");
		exit(EXIT_SUCCESS);
	}
	if (Branch::isBranch(mrootPath, margv[3])) {
		const  std::string commitObj = Branch::getCommitFromBranch(mrootPath, margv[3]);
		if (isValidCommit(commitObj)) {
			Merge merge{ mrootPath,commitObj };
			merge.merge();
		}
	}
	else if (this->isValidCommit(margv[3])) {
		Merge merge{ mrootPath,margv[3] };
		merge.merge();
	}
	else {
		const std::array<utils::StringColorPair, 3> arr{ {
			{"ERROR: cannot merge ", utils::Color::BRIGHT_RED},
			{std::string{"'"} + margv[3] + "'. ",utils::Color::BRIGHT_YELLOW},
			{"Invalid Reference.", utils::Color::BRIGHT_RED}
			} };
		utils::printColorful(arr);
	}
	return *this;

}
//TODO:
//show corresponding commit message if the file is up to date
//set file attribute while copying to hashed objects and vice versa
//cannot add multiple files at once//work on deleting and creating dir while checking out
//add validation for commit message

//work with commited and staged flag in index file and show status accordingly
//validate rootPath

//see and read:
//locking file 
//handle adding existing directory as file
//handle adding existing file as directory
//handle files not having enough permissions
//when user add files outside of root directory
//


