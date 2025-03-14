#include <filesystem>
#include <iostream>

int main() {
	cout << "Current working directory: " << filesystem::current_path() << endl;
	return 0;
}