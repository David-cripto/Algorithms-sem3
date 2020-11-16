#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using std::string;
using std::vector;

const int alphabet = 27;

void stepZero(int len, const string& text, vector<int>& suff_array, vector<vector<int>>& c, int& clas_num) {
	vector<int> count(alphabet, 0);

	for (int i = 0; i < len; ++i) {
		if (text[i] == '$') {
			++count[0];
			continue;
		}

		++count[text[i] - 'a' + 1];
	}

	for (int i = 1; i < alphabet; ++i) {
		count[i] += count[i - 1];
	}

	for (int i = 0; i < len; ++i) {
		if (text[i] == '$') {
			suff_array[0] = i;
			continue;
		}

		suff_array[--count[text[i] - 'a' + 1]] = i;
	}

	for (int i = 1; i < len; ++i) {
		if (text[suff_array[i]] != text[suff_array[i - 1]]) {
			++clas_num;
		}

		c[0][suff_array[i]] = clas_num - 1;
	}

}

void stepFromKToKPlusOne(int len, vector<int>& suff_array, vector<vector<int>>& c, int& clas_num) {
	vector<int> pn(len);
	int counter = 0;

	for (int i = 1; i < len; i *= 2) {

		for (int j = 0; j < len; ++j) {
			pn[j] = suff_array[j] - (2 << counter);
			if (pn[j] < 0) {
				pn[j] += len;
			}
		}

		vector<int> coun(clas_num, 0);

		for (int j = 0; j < len; ++j) {
			++coun[c[counter][pn[j]]];
		}

		for (int j = 1; j < static_cast<int>(coun.size()); ++j) {
			coun[j] += coun[j - 1];
		}

		for (int j = len - 1; j >= 0; --j) {
			suff_array[--coun[c[counter][pn[j]]]] = pn[j];
		}

		vector<int> clas_num_vec(len, 0);
		c.push_back(clas_num_vec);

		clas_num = 1;

		for (int j = 1; j < len; ++j) {
			int mid1 = (suff_array[j] + (2 << counter)) % len;
			int mid2 = (suff_array[j - 1] + (2 << counter)) % len;

			if (c[counter][suff_array[j]] != c[counter][suff_array[j - 1]] || c[counter][mid1] != c[counter][mid2]) {
				++clas_num;
			}
			c[counter + 1][suff_array[j]] = clas_num - 1;
		}

		counter++;
	}
}

vector<int> lcpArr(const vector<int>& suff_array, const string& str) {
	int n = static_cast<int>(str.length());

	vector<int> lcp(n - 1);
	vector<int> pos(n - 1);

	for (int i = 0; i < n - 1; ++i) {
		pos[suff_array[i]] = i;
	}

	int k = 0;

	for (int i = 0; i < n - 1; ++i) {
		if (k > 0) {
			k--;
		}
		if (pos[i] == n - 1) {
			lcp[n - 1] = -1;
			k = 0;
			continue;
		} else {
			int j = suff_array[pos[i] + 1];
			while (std::max(i + k, j + k) < n && str[i + k] == str[j + k]) {
				k++;
			}
			lcp[pos[i]] = k;
		}
	}

	return lcp;
}

vector<int> buildSuffArray(const string& str) {
	int len = static_cast<int>(str.length());
	vector<vector<int>> c(1, vector<int>(len));

	int clas_num = 1;
	vector<int> suff_array(len);

	stepZero(len, str, suff_array, c, clas_num);
	stepFromKToKPlusOne(len, suff_array, c, clas_num);

	return suff_array;
}

int getAnswer(const string& text) {
	string str = text;
	str.push_back('$');
	int len = static_cast<int>(str.length());
	int ans = 0;

	vector<int> suff_array = buildSuffArray(str);
	vector<int> lcp = lcpArr(suff_array, str);

	for (int i = 1; i < len; i++) {
		ans += (len - 1) - suff_array[i];
	}

	for (int i = 0; i < len - 2; i++) {
		ans -= lcp[i];
	}

	return ans;
}

int main() {
	string str;
	std::cin >> str;
	std::cout << getAnswer(str);
	return 0;
}