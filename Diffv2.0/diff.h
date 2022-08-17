#pragma once
class Diff {
private:
	int* index;
	int size;
public:
	Diff() = default;
	Diff(int Max) :size{ Max }, index{ new int[2 * size + 1] }{}
	int& operator [] (int i) {
		return index[i + size];
	}
	~Diff() {
		delete[](index);
	}
};
