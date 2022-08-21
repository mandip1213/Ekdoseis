#pragma once
#include "command.h"

class Add :public Command {
private:

public:
	Add(int argc, char* argv[]) :
		Command(argc, argv) {}

	void runCommand()override;
	void addFile(const fs::path& filePath);

};

