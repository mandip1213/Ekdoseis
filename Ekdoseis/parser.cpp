#include"parser.h"
using namespace parser;
bool parser::hasDoubleHyphen(const char* token) {
	//maybe BUG: only one hyphen in currtoken and next token starts with - too | maybe 
	if (*token == '-' && *(token + 1) != '-') {
		return true;
	}
	return false;
}

bool parser::isCommand(const char* token) {
	const char* tokens[] = { "init","add","commit" };
	for (const char* command : tokens) {
		if (strcmp(command, token) == 0) {
			return true;
		}
	}
	return false;
}

void parser::parseCommand(int argc, char* argv[], std::vector<struct parser::Token> tokenList) {
	int commandStart = 2;
	int tokenListCounter{};
	if (hasDoubleHyphen(argv[commandStart])) {
		const char* _token = argv[commandStart] + 2;
		struct Token token { std::string{ _token }, COMMAND_ARG };
		tokenList[tokenListCounter++] = token;
		return;
	}
	else if (isCommand(argv[commandStart])) {
		struct Token token { std::string{ argv[commandStart] }, COMMAND };
		tokenList[tokenListCounter++] = token;
		for (int i = commandStart + 1; i < argc; i++) {
			if (hasDoubleHyphen(argv[i])) {
				const char* _token = argv[commandStart] + 2;
				struct Token token { std::string{ _token }, COMMAND_ARG };
				tokenList[tokenListCounter++] = token;
				break;
			}
			else if (*(argv[i]) == '-') {//equivalent ot startswithhyphen/


			}


		}
	}
}

