#include<iostream>

using namespace std;

struct Test {
	int value;
};

int main() {
	cout << alignof(Test) << endl;	

	return 0;
}
