#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <set>
#include <map>
#include <iomanip>
#include <string>

using std::vector;
using std::string;

struct state {
	//t -- terminator
	//f -- fugitive
	int t, f;
	int step;

	state() = default;

	state(int t, int f, int step) :
		t(t), f(f), step(step) {}
};

const char wall = '1';

//number of strings
const int n = 9;

//number of columns
const int m = 10;

//right diag
const int right_diag = 11;

//left diag
const int left_diag = 9;

vector<state> g[n * m][n * m][2];
int win[n * m][n * m][2];
int loose[n * m][n * m][2];
int used[n * m][n * m][2];
int degree[n * m][n * m][2];

bool checkKill(int same, int other1, int other2, const vector<string>& str, bool flag) {
	bool is_kill = true;

	int min_s = std::min(other1, other2);
	int max_s = std::max(other1, other2);

	//if wall then not kill
	for (int k = min_s; k <= max_s; ++k) {
		if (flag && str[k][same] == wall || !flag && str[same][k] == wall) {
			is_kill = false;
			break;
		}
	}

	return is_kill;
}

bool checkDiag(int s_t, int c_t, int s_f, int c_f, const vector<string>& str, int flag) {
	bool is_kill = true;

	int min_s = std::min(s_t, s_f);
	int max_s = std::max(s_t, s_f);
	int min_c = std::min(c_t, c_f);
	int max_c = std::max(c_t, c_f);

	int i;

	if (flag == 1) {
		i = min_c;
	} else {
		i = max_c;
	}
	//if wall then not kill
	for (int k = min_s; k <= max_s; ++k, i += flag) {
		if (str[k][i] == wall) {
			is_kill = false;
			break;
		}
	}

	return is_kill;
}

void markTerminatorWin(const vector<string>& str) {
	//take a pos of fugitive
	for (int i = 0; i < n * m; ++i) {
		//pos string of fugitive in table
		int s_f = i / m;

		//pos column of fugitive in table
		int c_f = i % m;

		if (str[s_f][c_f] == wall) {
			continue;
		}

		//take pos of terminator
		for (int j = 0; j < n * m; ++j) {
			//the same with terminator
			int s_t = j / m;
			int c_t = j % m;

			if (str[s_t][c_t] == wall) {
				continue;
			}

			//in the same column or string
			if (c_f == c_t && checkKill(c_f, s_f, s_t, str, true) || s_f == s_t && checkKill(s_f, c_f, c_t, str, false)) {
				win[i][j][0] = 1;
				loose[i][j][1] = 1;
			}
			else if ((std::abs(s_t - s_f) == std::abs(c_t - c_f)) && std::abs(i - j) % right_diag == 0 && checkDiag(s_t, c_t, s_f, c_f, str, 1)) {
				//right diag
				win[i][j][0] = 1;
				loose[i][j][1] = 1;
			}
			else if ((std::abs(s_t - s_f) == std::abs(c_t - c_f)) && std::abs(i - j) % left_diag == 0 && checkDiag(s_t, c_t, s_f, c_f, str, -1)) {
				//left diag
				win[i][j][0] = 1;
				loose[i][j][1] = 1;
			}
		}
	}
}

void markFigutiveWin(const vector<string>& str) {
	for (int i = 0; i < m; ++i) {
		if (str[n - 1][i] == '1') {
			continue;
		}

		for (int j = 0; j < n * m; ++j) {
			if (str[j / m][j % m] == '1') {
				continue;
			}

			if (!loose[(n - 1) * m + i][j][1]) {
				win[(n - 1) * m + i][j][1] = 1;
				//loose[(n - 1) * m + i][j][0] = 1;
			}
		}
	}
}

void markVertexes(const vector<string>& str) {
	markTerminatorWin(str);
	markFigutiveWin(str);
}

void makeEdges(const vector<string>& str) {
	//take a pos of fugitive
	for (int i = 0; i < n * m; ++i) {
		//pos string of fugitive in table
		int s_f = i / m;

		//pos column of fugitive in table
		int c_f = i % m;

		if (str[s_f][c_f] == wall) {
			continue;
		}

		//take pos of terminator
		for (int j = 0; j < n * m; ++j) {
			//the same with terminator
			int s_t = j / m;
			int c_t = j % m;

			if (str[s_t][c_t] == wall) {
				continue;
			}

			//step
			for (int st = 0; st <= 1; ++st) {
				const int dx[] = { -1, 0, 1, 0,   -1, -1, 1, 1 };
				const int dy[] = { 0, 1, 0, -1,   -1, 1, -1, 1 };

				if (win[i][j][st] == 1) {
					continue;
				}

				state stat(j, i, st);

				for (int d = 0; d < 8; ++d) {
					int ssf = s_f;
					int sst = s_t;
					int ccf = c_f;
					int cct = c_t;

					//terminator step
					if (st == 0) {
						sst += dy[d];
						cct += dx[d];
					}
					else {//fl step
						ssf += dy[d];
						ccf += dx[d];
					}

					if (ssf < n && str[ssf][ccf] != '1' && sst < n && str[sst][cct] != '1') {
						g[ssf * m + ccf][sst * m + cct][!st].push_back(stat);
						++degree[i][j][st];
					}
				}
			}
		}
	}
}

void builtGraph(const vector<string>& str) {
	//mark vertexes
	markVertexes(str);

	//make edges
	makeEdges(str);
}

vector<string> newStr(const vector<string>& str) {
	//make boarder
	vector<string> new_str;

	string temp(m, '1');
	string help = "1";

	new_str.push_back(temp);

	for (int i = 1; i < n; ++i) {
		new_str.push_back(help + str[i - 1] + help);
	}

	return new_str;
}

void dfs(int f, int t, int step) {
	used[f][t][step] = 1;
	for (state st : g[f][t][step]) {
		if (!used[st.f][st.t][st.step]) {
			if (loose[f][t][step]) {
				win[st.f][st.t][st.step] = 1;
			} else if (--degree[st.f][st.t][st.step] == 0) {
				loose[st.f][st.t][st.step] = 1;
			} else {
				continue;
			}
			dfs(st.f, st.t, st.step);
		}
	}
}

int main() {
	vector<string> str(n - 1);

	for (int i = 0; i < n - 1; ++i) {
		std::cin >> str[i];
	}

	vector<string> new_str = newStr(str);

	builtGraph(new_str);

	for (int i = 0; i < n * m; ++i) {
		//pos string of fugitive in table
		int s_f = i / m;

		//pos column of fugitive in table
		int c_f = i % m;

		if (new_str[s_f][c_f] == wall) {
			continue;
		}

		for (int j = 0; j < n * m; ++j) {
			//the same with terminator
			int s_t = j / m;
			int c_t = j % m;

			if (new_str[s_t][c_t] == wall) {
				continue;
			}

			for (int s = 0; s <= 1; ++s) {
				if ((win[i][j][s] || loose[i][j][s]) && !used[i][j][s]) {
					dfs(i, j, s);
				}
			}
		}
	}

	int pos_f;
	int pos_t;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			if (new_str[i][j] == '2') {
				pos_f = i * m + j;
			}
			if (new_str[i][j] == '3') {
				pos_t = i * m + j;
			}
		}
	}

	std::cout << (win[pos_f][pos_t][true] == 1 ? 1 : -1);

	return 0;
}