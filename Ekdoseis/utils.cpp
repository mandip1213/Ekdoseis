#include "utils.h"


int utils::getHex(char c) {
	if (isdigit(c))return  c - '0';
	else	if (c >= 'a' && c <= 'f')return  c - 'a' + 10;
	else {
		cout << "error: not a hex digit" << endl;
		return 0;
	}
}

void utils::toHexEncoding(std::string hash, unsigned char* ptr, int len) {
	if (hash.size() != 40) {
		exit(EXIT_FAILURE);
		return;
	}
	uint8_t byte{};
	int count{};
	for (int i = 0; i < 40; i++) {
		byte = utils::getHex(hash[i]);
		byte = byte << 4;
		++i;
		byte += utils::getHex(hash[i]);
		ptr[i / 2] = byte;
	}
}

std::string utils::toAsciiEncoding(const std::string& hash) {
	cout << hash.size() << endl;
	cout << hash<<endl;

	return hash;
}
