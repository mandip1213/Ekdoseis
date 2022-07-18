#include<chrono>
#include<ctime>
#include "utils.h"
#define endl '\n'

int utils::getHex(char c) {
	if (isdigit(c))return  c - '0';
	else	if (c >= 'a' && c <= 'f')return  c - 'a' + 10;
	else {
		//cout << "error: not a hex digit" << endl;
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
	//cout << hash.size() << endl;
	//cout << hash << endl;

	return hash;
}


ReturnFlag utils::createObjectDir(const fs::path& newDir, CreateFlag flag) {
	if (fs::exists(newDir)) {
		if (flag == NO_OVERRIDE) {
			return ALREADY_EXISTS;
		}
		else if (flag == OVERRIDE_IF_EXISTS) {
			if (!fs::remove(newDir)) {
				return OVERRIDE_FAILURE;
			}//todo : create a new won now
			return OVERRIDE_SUCCESS;
		}
	}
	bool _success = fs::create_directory(newDir);
	if (flag == NO_OVERRIDE) {
		return (_success ? CREATE_SUCCESS : CREATE_SUCCESS);
	}
	else	if (flag == OVERRIDE_IF_EXISTS) {
		return (_success ? OVERRIDE_SUCCESS : OVERRIDE_SUCCESS);
	}
	return CREATE_SUCCESS;

}

bool utils::validateHash(const std::string& hash) {
	if (hash.length() != 40) {
		std::cerr << "Invalid Hash" << endl;
		exit(EXIT_FAILURE);
		//return false;
	}
	for (const auto& curr : hash) {
		if (!((curr >= 'a' && curr <= 'f') || isdigit(curr))) {
			std::cerr << "Invalid Hash" << endl;
			exit(EXIT_FAILURE);
			//return false;
		}
	}
	return true;
}

uint64_t utils::getTime() {
	using namespace std::chrono;
	uint64_t ms = duration_cast<seconds>(
		system_clock::now().time_since_epoch()).count();

	std::cout << "ms  " << ms << endl;
	return ms;
}

void utils::printDate(uint64_t time) {
	//auto currTime = std::chrono::system_clock::now();
	//const std::time_t t_c = std::chrono::system_clock::to_time_t(currTime);
	const std::time_t t_c{ static_cast<long long>(time) };
	std::tm t;
	//gmtime_s(&t, &t_c); for gmtime
	localtime_s(&t, &t_c);
	//LINK:https://en.cppreference.com/w/cpp/io/manip/put_time
	std::cout << std::put_time(&t, "%a %b %d %Y %H:%M:%S %z %Z");

}

