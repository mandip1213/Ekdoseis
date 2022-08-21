#pragma once
#include"commitaction.h"

void CommitAction::runCommand() {

	if (margc >= 5) {
		if (!strcmp(margv[4].c_str(), "-m")) {
			utils::printError("Error: Invalid Command\n");
		}
	}
	else {
		std::cerr << "Error: Not enough arguments" << endl;
	}
	Commit commit{ mrootPath };
	commit.setMessage(margv[4]);
	commit.createTree();
	commit.fetchParentHash();
	commit.getAuthor();

	commit.createCommit();
	const std::array<utils::StringColorPair, 2> arr{ {
			{"Changes commited successfully with commit hash ", utils::Color::BRIGHT_YELLOW},
			{" '" + commit.getHash() + "' ",utils::Color::BRIGHT_GREEN},
			} };
	utils::printColorful(arr);
	//	commit.compareTreeHash();//todo: instead of this  check the flag of every file of index
}
