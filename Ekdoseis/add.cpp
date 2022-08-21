#include<filesystem>
#include<string>
#include"add.h"

#include"sha1.h"

#define endl '\n'
namespace fs = std::filesystem;


void Add::runCommand() {
	//check for git init
	if (!fs::exists(mrootPath / ".dose/index")) {
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
	}
}



void Add::addFile(const fs::path& currPath) {

	if (!exists(currPath)) {
		const std::array<utils::StringColorPair, 3> arr{ {
		{"ERROR:", utils::Color::BRIGHT_RED},
		{" '" + currPath.string() + "' ",utils::Color::BRIGHT_GREEN},
		{"doesnot exists\n", utils::Color::BRIGHT_RED}
		} };
		utils::printColorful(arr);
		return;
		//exit(EXIT_SUCCESS);
	}
	if (doseIgnore.has(currPath)) {
		const std::array<utils::StringColorPair, 3> arr{ {
		{"ERROR:", utils::Color::BRIGHT_RED},
		{" '" + currPath.string() + "' ",utils::Color::BRIGHT_GREEN},
		{"exists in the doseignore.", utils::Color::BRIGHT_RED}
		} };
		utils::printColorful(arr);
		return;
		//exit(EXIT_SUCCESS);
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
			utils::printError("Error: cannot perform the required action\n");
			//exit(EXIT_SUCCESS);
			return;
		}

		const fs::path _hashFilePath{ hashDirPath / hash.substr(2,hashLength - 2) };
		std::error_code ec;

		fs::copy_file(currPath, _hashFilePath, fs::copy_options::skip_existing, ec); //no encryption now maybe later
		if (ec) {
			const std::array<utils::StringColorPair, 3> arr{ {
			{"Error: ", utils::Color::BRIGHT_RED},
			{ec.message(),utils::Color::BRIGHT_GREEN},
			{"\nError: couldnot perform the required action\n", utils::Color::BRIGHT_YELLOW}

			} };
			utils::printColorful(arr);
			return;
			//exit(EXIT_SUCCESS);
		}
		bool badd = mindex.add(currPath, hash);
		if (badd) {
			const std::array<utils::StringColorPair, 3> arr{ {
			{"File:", utils::Color::BRIGHT_YELLOW},
			{" '" + currPath.string() + "' ",utils::Color::BRIGHT_GREEN},
			{"staged successfully.\n", utils::Color::BRIGHT_YELLOW}

			} };
			utils::printColorful(arr);
		}
	}
}
