#include "splay_tree.h"
#include "testrunner.h"
#include "profile.h"


#include <ctime>
#include <set>
#include <string>


const int INF = static_cast<int>(1e9);


const int number_of_operations = static_cast<int>(1e6);
const int max_number = static_cast<int>(1e5);
const double insert_probability = 0.7;


int Rand(int maxNumber = INF) {
	return (rand() % 1000 * 1000 * 1000 +
			rand() % 1000 * 1000 +
			rand() % 1000) % max_number;
}


double rand0to1 () {
	return static_cast<double>(Rand()) /
			static_cast<int>(INF);
}


template<typename T>
void checkEqual(T& t, std::set<int>& s) {
	while (s.size() != 0) {
		int a1 = t.min();
		int a2 = *s.lower_bound(-INF);
		AssertEqual(a1, a2, "different min values");
		t.erase(a1);
		s.erase(a2);
	}
	AssertEqual(t.empty(), true, "container must be empty in the end");
}


template<typename T>
void correctnessTest() {
	T container;
	std::set<int> checker;
	for (int i = 0; i < number_of_operations; ++i) {
		int a = Rand(max_number);
		if (rand0to1() <= insert_probability) {
			container.insert(a);
			checker.insert(a);
		} else {
			container.erase(a);
			checker.erase(a);
		}
	}
	checkEqual(container, checker);
}


std::string toString(int number) {
	if (number == 0) {
		return "0";
	} else {
		std::string result;
		while (number > 0) {
			result = static_cast<char>(number % 10 + '0') + result;
			number /= 10;
		}
		return result;
	}
}


template<typename Container>
void speedTest() {
	Container c;
	for (int i = 0; i < number_of_operations; ++i) {
		c.insert(Rand(max_number));
	}
}


void TestAll() {
	TestRunner tr;
	tr.RunTest(correctnessTest<SplayTree>, "checking Splay-Tree correctness");
	{ LOG_DURATION(toString(number_of_operations) + " insert operations Splay-Tree test");
		speedTest<SplayTree>();
	}
	{ LOG_DURATION(toString(number_of_operations) + " insert operations std::set test");
		speedTest<std::set<int>>();
	}
}


int main () {
	srand(time(0));
	TestAll();
	return 0;
}
