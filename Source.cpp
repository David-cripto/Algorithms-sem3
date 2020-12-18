#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<int> findEntries(const std::string& pattern, const std::string& str) {
	std::string s = pattern + "#" + str;
	int n = static_cast<int>(s.length());//len of s
	int p = static_cast<int>(pattern.length());
	std::vector<int> pref(p);// prefix-fun
	std::vector<int> positions;
	int k;

	for (int i = 1; i < n; ++i) {
		if (i < p) {
			k = pref[i - 1];
		} else if (i == p) {
			k = 0;
		}

		while (k > 0 && s[i] != s[k]) {//count k
			k = pref[k - 1];
		}

		if (s[i] == s[k]) {
			k++;
		}

		if (i >= p && k == p) {
			positions.push_back(i - p - k);
			k = pref[k - 1];
		}
		if (i < p) {
			pref[i] = k;
		}
	}

	return positions;
}

int main() {
	std::string pattern;
	std::string str;
	std::cin >> pattern >> str;
	std::vector<int> p = findEntries(pattern, str);
	for (int i = 0; i < p.size(); ++i) {
		std::cout << p[i] << " ";
	}
	return 0;
}