
#pragma once
#include "command.h"

class CommitAction :public Command {
private:

public:
	CommitAction(int argc, char* argv[]) :
		Command(argc, argv) {}
	void runCommand()override;
};
