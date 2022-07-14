#pragma once
#ifndef _PARSER_H
#define _PARSER_H


#include<string>
#include<vector>
namespace parser {

	enum TokenType {
		COMMAND_ARG,
		COMMAND,
		ATTRIBUTE,
		EXPRESSION,
		CHARACTER
	};
	enum TokenState {
		DATA,
		COMMAND_NAME,
	};
	struct Token {
		std::string token;
		TokenType type;
	};
	bool hasDoubleHyphen(const char* token);
	bool isCommand(const char* token);

	void parseCommand(int argc, char* argv[], std::vector<struct parser::Token> tokenList);

	//namespace closing brace
}

#endif // !_PARSER_H
