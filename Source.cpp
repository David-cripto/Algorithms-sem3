#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using std::string;
using std::vector;

const int alphabet = 27;

void stepZero(int len, const string& text, vector<int>& suff_array, vector<vector<int>>& arr_of_classes, int& class_num) {
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
			++class_num;
		}

		arr_of_classes[0][suff_array[i]] = class_num - 1;
	}

}

void stepFromKToKPlusOne(int len, vector<int>& suff_array, vector<vector<int>>& arr_of_classes, int& class_num) {
	vector<int> pn(len);
	int counter = 0;

	for (int i = 1; i < len; i *= 2) {

		for (int j = 0; j < len; ++j) {
			pn[j] = suff_array[j] - (1 << counter);
			if (pn[j] < 0) {
				pn[j] += len;
			}
		}

		vector<int> count(class_num, 0);

		for (int j = 0; j < len; ++j) {
			++count[arr_of_classes[counter][pn[j]]];
		}

		for (int j = 1; j < static_cast<int>(count.size()); ++j) {
			count[j] += count[j - 1];
		}

		for (int j = len - 1; j >= 0; --j) {
			suff_array[--count[arr_of_classes[counter][pn[j]]]] = pn[j];
		}

		vector<int> class_num_vec(len, 0);
		arr_of_classes.push_back(class_num_vec);

		class_num = 1;

		for (int j = 1; j < len; ++j) {
			int mid1 = (suff_array[j] + (1 << counter)) % len;
			int mid2 = (suff_array[j - 1] + (1 << counter)) % len;

			if (arr_of_classes[counter][suff_array[j]] != arr_of_classes[counter][suff_array[j - 1]] || arr_of_classes[counter][mid1] != arr_of_classes[counter][mid2]) {
				++class_num;
			}
			arr_of_classes[counter + 1][suff_array[j]] = class_num - 1;
		}

		counter++;
	}
}

vector<int> lcpArr(const vector<int>& suff_array, const string& str) {
	int n = static_cast<int>(str.length());

	vector<int> lcp(n);
	vector<int> pos(n);

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
	vector<vector<int>> arr_of_classes(1, vector<int>(len));

	int class_num = 1;
	vector<int> suff_array(len);

	stepZero(len, str, suff_array, arr_of_classes, class_num);
	stepFromKToKPlusOne(len, suff_array, arr_of_classes, class_num);

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

	for (int i = 0; i < static_cast<int>(lcp.size()); i++) {
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