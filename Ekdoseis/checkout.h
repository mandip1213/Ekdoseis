#pragma once
#include "command.h"

class Checkout :public Command {
private:

public:
	Checkout (int argc,char* argv[]):
		Command(argc,argv){}

	void runCommand()override;
};
