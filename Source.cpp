#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


//1 -- ind = 0, 2 -- ind = 1 
void helpFunction(const std::string& s, std::vector<long long>& arr, long long& l, long long& r, long long ind, long long i, long long n) {

	if (i <= r) {//take the min for don't go outside the border
		arr[i] = std::min(r - i + ind, arr[r - i + l + ind]);
	}

	while (i + arr[i] + 1 - ind <= n && i - arr[i] - 1 >= 0 && s[i + arr[i] + 1 - ind] == s[i - arr[i] - 1]) {//count z-fun
		++arr[i];
	}

	if (i + arr[i] - ind > r) {//move the right border
		l = i - arr[i];
		r = i + arr[i] - ind;
	}

}

long long algorithmOfManacher(const std::string& s) {
	long long n = static_cast<long long>(s.length());//len of s
	std::vector<long long> recursive_arr1(n);// z-fun
	std::vector<long long> recursive_arr2(n);// z-fun
	long long ans = 0;
	long long l_0 = 0;
	long long r_0 = 0;
	long long l_1 = 0;
	long long r_1 = 0;
	for (long long i = 1; i < n; ++i) {
		helpFunction(s, recursive_arr1, l_0, r_0, 0, i, n);
		helpFunction(s, recursive_arr2, l_1, r_1, 1, i, n);
	}

	for (long long i = 0; i < n; ++i) {
		ans += recursive_arr1[i] + recursive_arr2[i];
	}

	return ans;
}

int main() {
	std::string str;
	std::cin >> str;
	std::cout << algorithmOfManacher(str);
	return 0;
}