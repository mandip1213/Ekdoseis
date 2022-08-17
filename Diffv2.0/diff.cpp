#include"diff.h"
#include<iostream>
#include<cstring>
#include<tuple>
#include<fstream>
/* It gives the length of shortest way possible
int frontTrack(const char* a, size_t n, const char* b, size_t m) {
	int max = n + m;
	Diff v{ max };
	v[1] = 0;
	int x{}, y{};
	for (int d{ 0 }; d <= max; d++) {
		for (int k{ -d }; k <= d; k += 2) {
			if (k == -d || (k != d && v[k - 1] < v[k + 1])) {
				x = v[k + 1];
			}
			else {
				x = v[k - 1] + 1;
			}
			y = x - k;
			while (x < n && y < m && a[x] == b[y]) {
				x += 1;
				y += 1;
			}
			v[k] = x;
			if (x >= n && y >= m) {
				return d;
			}
		}
	}
}
*/
//Finding middle snake 
std::tuple<int, int, int, int, int> FindMiddleSnake(const char* a, size_t n, const char* b, size_t m) {
	int delta = n - m;
	static Diff fv(n+m);
	static Diff rv(n+m);
	int x{}, y{};
	fv[1] = 0;
	rv[delta + 1] = n + 1;
	for (int d{ 0 }; d <= std::ceil((n+m) / 2.0); d++) {
		for (int k{ -d }; k <= d; k += 2) {
			if (k == -d || (k != d && fv[k - 1] < fv[k + 1])) {
				x = fv[k + 1];
			}
			else {
				x = fv[k - 1] + 1;
			}
			y = x - k;
			while (x < n && y < m && a[x] == b[y]) {
				x += 1;
				y += 1;
			}
			fv[k] = x;
			if (delta % 2 != 0 && k >= delta - (d - 1) && k <= delta + (d - 1)) {
				if (fv[k] == rv[k]) {
					return std::make_tuple(rv[k], rv[k] - k, x, y, 2 * d - 1);
				}
			}
		}
		for (int k{ -d }; k <= d ; k += 2) {
			if (k == -d + delta || (k != d + delta && rv[k - 1] >= rv[k + 1])) {
				x = rv[k + 1] - 1;
			}
			else {
				x = rv[k - 1];
			}
			y = x - k;
			while (x > 0 && y > 0 && a[x - 1] == b[y - 1]) {
				x -= 1;
				y -= 1;
			}
			rv[k] = x;
			if (delta % 2 == 0 && k >= (- d - delta) && k <= (d-delta)) {
				if (fv[k] == rv[k]) {
					return std::make_tuple(x, y, fv[k], fv[k] - k, 2 * d);
				}
			}
		}
	}
	return {};
}
//To print output
void Output(const char* str, int start, int end) {
	for (int i = start; i <= end; i++) {
		std::cout << str[i];
	}
}
//To find insertion and deletion
void LCS(const char* a, size_t n, const char* b, size_t m) {
	if (n > 0 && m > 0) {
		int x, y, u, v, d;
		std::tie(x, y, u, v, d) = FindMiddleSnake(a, n, b, m);
		if (d > 1) {
			LCS(a, x, b, y);
			Output(a, x , u-1);
			LCS(a+u, n - u, b+v, m - v);
		}
		else if (m > n) {
			Output(a,0,n-1);
		}
		else {
			Output(b,0,m-1);
		}
	}
}
///*
//SES to find insertion and deletion
void SES(const char* a, size_t n, const char* b, size_t m) {
	while (*a == *b && n > 0 && m > 0) {
		++a;
		++b;
		--n;
		--m;
	}
	while (*(a + n - 1) == *(b + m - 1) && n > 0 && m > 0) {
		--n;
		--m;
	}
	if (n > 0 && m > 0) {
		int x{}, y{}, u{}, v{}, d{};
		std::tie(x, y, u, v, d) = FindMiddleSnake(a, n, b, m);
		SES(a, x, b, y);
		SES(a + u, n - u, b + v, m - v);
	}
	else if (n > 0) {
		std::cout << "- ";
		Output(a, 0, n-1);
	}
	else if (m > 0) {
		std::cout << "+ ";
		Output(b, 0, m - 1);
	}
}
/*//yo chaii link"https://www.codeproject.com/Articles/42280/Investigating-Myers-Diff-Algorithm-Part-2-of-2" ko ma vako pseudo code implement garna khojeko but got stuck
void SES(const char* a, size_t n, const char* b, size_t m) {
	if (m == 0 && n > 0) {
		std::cout << "- ";
		Output(a, 0, n - 1);
	}
	if (n == 0 && m > 0) {
		std::cout << "+ ";
		Output(b, 0, m - 1);
	} 
	int x, y, u, v, d;
	std::tie(x, y, u, v, d) = FindMiddleSnake(a, n, b, m);
	if (d > 1) {

	}
	else if (d == 1) {

	}
	else if (d == 0) {

	}
}
*/
int main() { //	char a[] = "abcabba";
//	char b[] = "cbabac";
//	std::cout << frontTrack(a, strlen(a), b, strlen(b));//Gives shortest way to the solution

	//Read from file [tala ko complete comment haneko code ma without junk character at end aauney garera gareko xa]
	std::ifstream file1("a.txt", std::ios::ate);
	size_t sizeofa = file1.tellg();
	char* a = new char[sizeofa];
	file1.seekg(0);
	file1.read((a), sizeofa);
	file1.close();
	std::ifstream file2("b.txt", std::ios::ate);
	size_t sizeofb = file2.tellg();
	char* b = new char[sizeofb];
	file2.seekg(0);
	file2.read(b, sizeofb);
	file2.close();
	std::cout << a << "\n\n" << b << "\n\n";

	//Passing to LCS [to find common lines]
	LCS(a, sizeofa, b, sizeofb);
	//Passing to SES [to find insertion and deletion]
	SES(a, sizeofa, b, sizeofb);
	delete[] a;
	delete[] b;
	return 0;
}


//decomposing into two dimensional
/*
#include<iostream>
#include<fstream>
#include<cstring>
#include<tuple>
#include"diff.h"
size_t fileSize(const char* fileName) {
	std::ifstream file(fileName, std::ios::ate);
	return file.tellg();
}
std::string readFromFile(const char* fileName) {
	std::ifstream file(fileName);
	return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}
std::tuple<size_t, size_t>Counter(const char* str, size_t size) {
	size_t lineCounter{ 1 }, charCounter;
	for (int i{ 0 }; i < size; i++) {
		if (str[i] == '\n')
			lineCounter++;
	}
	int* character = new int[lineCounter];
	int head{ 0 };
	for (int i{ 0 }; i < lineCounter; i++) {
		character[i] = 0;
		int j{ 0 };
		while (str[head] != '\n') {
			if (str[head] == '\0')
				break;
			character[i]++;
			j++;
			head++;
		}
		head++;
	}
	charCounter = character[0];
	for (int i{ 0 }; i < lineCounter; i++) {
		for (int j{ i + 1 }; j < lineCounter; j++) {
			if (charCounter < character[j])
				charCounter = character[j];
		}
	}
	delete[] character;
	return std::make_tuple(lineCounter, charCounter);
}
void decompose(char** decomposed, const char* str, size_t lines, size_t words) {
	int head{ 0 };
	for (int i{ 0 }; i < lines; i++) {
		int j{ 0 };
		while (str[head] != '\n') {
			if (str[head] == '\0') {
				decomposed[i][j] = '\0';
				break;
			}
			decomposed[i][j] = str[head];
			j++;
			head++;
		}
		head++;
	}
}
void compareAndOutput(const char* A, const char* B) {
	bool result = true ? (strcmp(A, B) == 0) : false;
	if (result)
		std::cout << "  " << A << std::endl;
	else {
		std::cout << "- " << A << std::endl;
		std::cout << "+ " << B << std::endl;
	}
}

std::tuple<int, int, int, int, int> FindMiddleSnake(const char* a, size_t n, const char* b, size_t m) {
	int delta = n - m;
	static Diff fv(n+m);
	static Diff rv(n+m);
	int x{}, y{};
	fv[1] = 0;
	rv[delta + 1] = n + 1;
	for (int d{ 0 }; d <= std::ceil((n+m) / 2.0); d++) {
		for (int k{ -d }; k <= d; k += 2) {
			if (k == -d || (k != d && fv[k - 1] < fv[k + 1])) {
				x = fv[k + 1];
			}
			else {
				x = fv[k - 1] + 1;
			}
			y = x - k;
			while (x < n && y < m && a[x] == b[y]) {
				x += 1;
				y += 1;
			}
			fv[k] = x;
			if (delta % 2 != 0 && k >= delta - (d - 1) && k <= delta + (d - 1)) {
				if (fv[k] == rv[k]) {
					return std::make_tuple(rv[k], rv[k] - k, x, y, 2 * d - 1);
				}
			}
		}
		for (int k{ -d }; k <= d ; k += 2) {
			if (k == -d + delta || (k != d + delta && rv[k - 1] >= rv[k + 1])) {
				x = rv[k + 1] - 1;
			}
			else {
				x = rv[k - 1];
			}
			y = x - k;
			while (x > 0 && y > 0 && a[x - 1] == b[y - 1]) {
				x -= 1;
				y -= 1;
			}
			rv[k] = x;
			if (delta % 2 == 0 && k >= (- d - delta) && k <= (d-delta)) {
				if (fv[k] == rv[k]) {
					return std::make_tuple(x, y, fv[k], fv[k] - k, 2 * d);
				}
			}
		}
	}
	return {};
}
void Output(const char* str, int start, int end) {
	for (int i = start; i <= end; i++) {
		std::cout << str[i];
	}
}
void LCS(const char* a, size_t n, const char* b, size_t m) {
	if (n > 0 && m > 0) {
		int x, y, u, v, d;
		std::tie(x, y, u, v, d) = FindMiddleSnake(a, n, b, m);
		if (d > 1) {
			LCS(a, x, b, y);
			Output(a, x , u-1);
			LCS(a+u, n - u, b+v, m - v);
		}
		else if (m > n) {
			Output(a,0,n-1);
		}
		else {
			Output(b,0,m-1);
		}
	}
}
void SES(const char* a, size_t n, const char* b, size_t m) {
	while (*a == *b && n > 0 && m > 0) {
		--n;
		--m;
	}
	while (*(a + n - 1) == *(b + m - 1) && n > 0 && m > 0) {
		--n;
		--m;
	}
	if (n > 0 && m > 0) {
		int x{}, y{}, u{}, v{}, d{};
		std::tie(x, y, u, v, d) = FindMiddleSnake(a, n, b, m);
		SES(a, x, b, y);
		SES(a + u, n - u, b + v, m - v);
	}
	else if (n > 0) {
		std::cout << "- ";
		Output(a, 0, n-1);
	}
	else if (m > 0) {
		std::cout << "+ ";
		Output(b, 0, m - 1);
	}
}
int main() {
	size_t sizeofA = fileSize("a.txt");
	size_t sizeofB = fileSize("b.txt");
	char* A = new char[sizeofA];
	char* B = new char[sizeofB];
	std::string fromFileA = readFromFile("a.txt");
	std::string fromFileB = readFromFile("b.txt");
	memcpy(A, fromFileA.c_str(), sizeofA);
	memcpy(B, fromFileB.c_str(), sizeofB);

//Finding number of lines and maximum number of characters in a line
	size_t lineInA, charInA, lineInB, charInB;
	std::tie(lineInA, charInA) = Counter(A, sizeofA);
	std::tie(lineInB, charInB) = Counter(B, sizeofB);

//Decomposing one dimensional into two dimensional
	char** twoDA = new char* [lineInA]{NULL};
	for (int i{ 0 }; i < lineInA; i++)
		twoDA[i] = new char[charInA]{NULL};
	decompose(twoDA, A, lineInA, charInA);
	char** twoDB = new char* [lineInB]{NULL};
	for (int i{ 0 }; i < lineInB; i++)
		twoDB[i] = new char[charInB]{NULL};
	decompose(twoDB, B, lineInB, charInB);

//Printing two dimensional
	for (int i{ 0 }; i < lineInA; i++) {
		for (int j{ 0 }; j < charInA; j++)
			std::cout << twoDA[i][j];
		std::cout << "\n";
	}
	for (int i{ 0 }; i < lineInB; i++) {
		for (int j{ 0 }; j < charInB; j++) 
			std::cout << twoDB[i][j];
		std::cout << "\n";
	}

//Printing the diff output
	int higher;
	(higher = lineInA) ? (lineInA > lineInB) : (higher = lineInB) ? (lineInB > lineInA) : (higher = lineInA);
	for (int i{ 0 }; i < higher; i++) {
		LCS(twoDA[i], strlen(twoDA[i]), twoDB[i], strlen(twoDB[i]));
		SES(twoDA[i], strlen(twoDA[i]), twoDB[i], strlen(twoDB[i]));
	}

//Deleting
	for (int i{ 0 }; i < lineInA; i++)
		delete[] twoDA[i];
	delete[] twoDA;
	for (int i{ 0 }; i < lineInB; i++)
		delete[] twoDB[i];
	delete[] twoDB;
	delete[] A;
	delete[] B;

	return 0;
}
*/
