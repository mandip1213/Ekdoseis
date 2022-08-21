#include"checkout.h"



void Checkout::runCommand() {

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

	const bool isBranch = Branch::isBranch(mrootPath, checkoutPoint);
	mindex = Index{ mrootPath };
	mindex.fetchFromIndex();
	if (isBranch) {
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

		if (isBranch) {
			const std::array<utils::StringColorPair, 2> arr{ {
					{"Sucessfully switched to branch" , utils::Color::BRIGHT_YELLOW},
					{" '" + checkoutPoint + "' \n",utils::Color::BRIGHT_GREEN},
					} };
			utils::printColorful(arr);
		}
		else {
			const std::array<utils::StringColorPair, 3> arr{ {
					{"Sucessfully switched to commit" , utils::Color::BRIGHT_YELLOW},
					{" '" + checkoutPoint + "' \n",utils::Color::BRIGHT_GREEN},
					{"You are now in detached head state." , utils::Color::BRIGHT_YELLOW},
					} };
			utils::printColorful(arr);
		}
	}
	else {
		const std::array<utils::StringColorPair, 2> arr{ {
				{"You are trying to checkout to illegal reference:" , utils::Color::BRIGHT_RED},
				{" '" + checkoutPoint + "' \n",utils::Color::BRIGHT_GREEN},
				} };
		utils::printColorful(arr);
	}

}

