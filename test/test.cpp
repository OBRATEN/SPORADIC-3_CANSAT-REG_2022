#include <iostream>
#include <string.h>
#include <vector>
#include <map>

using namespace std;

struct dictTypes {
	long int ints    = 1;
	long int strings = 1;
	long int chars   = 1;
	long int bools   = 1;
};

class Dict {
public:
	Dict(dictTypes dt) {
		m_dt = dt;
		charKeys   -> resize(dt.chars);
		charVals   -> resize(dt.chars);
		intKeys    -> resize(dt.ints);
		intVals    -> resize(dt.ints);
		boolKeys   -> resize(dt.bools);
		boolVals   -> resize(dt.bools);
		stringKeys -> resize(dt.strings);
		stringVals -> resize(dt.strings);
	}

	~Dict() {
		delete [] charVals;
		delete [] charKeys;
		delete [] boolVals;
		delete [] boolKeys;
		delete [] stringVals;
		delete [] stringKeys;
		delete [] intVals;
		delete [] intKeys;
	}

	void resizeInts(long int dif) {
		m_dt.ints += dif;
		intKeys -> resize(m_dt.ints);
		intVals -> resize(m_dt.ints);
	}

	void resizeChars(long int dif) {
		m_dt.chars += dif;
		charKeys -> resize(m_dt.chars);
		charVals -> resize(m_dt.chars);
	}

	void resizeStrings(long int dif) {
		m_dt.strings += dif;
		stringKeys -> resize(m_dt.strings);
		stringVals -> resize(m_dt.strings);
	}

	void resizeBools(long int dif) {
		m_dt.bools += dif;
		boolKeys -> resize(m_dt.bools);
		boolVals -> resize(m_dt.bools);
	}

	void pushInt(int d1, int d2) {
		intKeys -> push_back(d1);
		intVals -> push_back(d2);
	}

	int getInt(int idx) {
		return intVals->at(idx);
	}

	void printInts() {
		for (int i = 0; i < intKeys->size(); i++)
		if (intKeys->at(i) != 0)
		std::cout << intKeys->at(i) << " : " << intVals->at(i) << std::endl;
	}

	void pushChar(char d1, char d2) {
		charKeys -> push_back(d1);
		charVals -> push_back(d2);
	}

	char getChar(int idx) {
		return charVals->at(idx);
	}

	void printChars() {
		for (int i = 0; i < charKeys->size(); i++)
		if (charKeys->at(i) != 0)
		std::cout << charKeys->at(i) << " : " << charVals->at(i) << std::endl;
	}

	void pushString(string d1, string d2) {
		stringKeys -> push_back(d1);
		stringVals -> push_back(d2);
	}

	string getString(int idx) {
		return stringVals->at(idx);
	}

	void printStrings() {
		for (int i = 0; i < stringKeys->size(); i++)
		if (stringKeys->at(i).length() != 0)
		std::cout << stringKeys->at(i) << " : " << stringVals->at(i) << std::endl;
	}

	void pushBool(bool d1, bool d2) {
		boolKeys -> push_back(d1);
		boolVals -> push_back(d2);
	}

	bool getBool(int idx) {
		return boolVals->at(idx);
	}

	void printBools() {
		for (int i = 0; i < boolKeys->size(); i++)
		if (boolKeys->at(i))
		std::cout << ((boolKeys->at(i)) ? "true" : "false") << " : " << ((boolVals->at(i)) ? "true" : "false")  << std::endl;
	}

private:
	std::vector<char>   *charKeys   = new std::vector<char>(1);
	std::vector<int>    *intKeys    = new std::vector<int>(1);
	std::vector<bool>   *boolKeys   = new std::vector<bool>(1);
	std::vector<string> *stringKeys = new std::vector<string>(1);

	std::vector<char>   *charVals   = new std::vector<char>(1);
	std::vector<int>    *intVals    = new std::vector<int>(1);
	std::vector<bool>   *boolVals   = new std::vector<bool>(1);
	std::vector<string> *stringVals = new std::vector<string>(1);

	std::map <int, int> idxMap;
	dictTypes m_dt;
};

int main(void) {
	dictTypes a;
	a.ints = 150;
	a.bools = 200;
	a.strings = 30;
	a.chars = 20;
	Dict dict(a);
	dict.pushInt(1, 2);
	dict.pushChar('k', 'v');
	dict.pushString("asd", "dsa");
	dict.pushBool(1, 1);
	dict.pushBool(1, 0);
	dict.printInts();
	dict.printChars();
	dict.printStrings();
	dict.printBools();
}