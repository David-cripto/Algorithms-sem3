#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using std::string;
using std::vector;



struct Vertex {
	Vertex(int parent, char parCh, int alphabet);

	char parCh;
	int parent;
	int suffix;
	bool pat;
	vector<int> indOfPat;
	vector<int> edges;
	vector<int> move;
	vector<int> term;
	int alphabet;
};

Vertex::Vertex(int parent, char parCh, int alphabet = 26)
	:alphabet(alphabet), parent(parent), parCh(parCh), suffix(-1),
	pat(false), edges(alphabet, -1), move(alphabet, -1)
{}

struct AhoCorasik {

	AhoCorasik(const std::string& str, vector<std::pair<int, int>>& pos);

	vector<int> ans(const std::string& text);

	
private:
	string str;
	vector<std::pair<int, int>> pos;
	vector<Vertex> trie;

	int fast(int ind, char c);

	int suf(int ind);

	void findTermVer(int ind);
	const int alphabet = 26;

	char first_char = 'a';
	char sec_char = 'b';
	char question_char = '?';
	char last_char = 'z';
};

AhoCorasik::AhoCorasik(const std::string& str, vector<std::pair<int, int>>& pos)
	:str(str), trie(1, Vertex(0, -1)), pos(pos) 
{
	trie[0].suffix = 0;

	//////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////

	for (int i = 0; i < static_cast<int>(pos.size()); ++i) {
		int cur_ver = 0;
		for (int j = pos[i].first; j <= pos[i].second; ++j) {
			char temp = str[j] - first_char;
			if (trie[cur_ver].edges[temp] == -1) {
				trie.push_back(Vertex(cur_ver, temp));
				trie[cur_ver].edges[temp] = static_cast<int>(trie.size()) - 1;
			}
			cur_ver = trie[cur_ver].edges[temp];
		}
		trie[cur_ver].pat = true;
		trie[cur_ver].indOfPat.push_back(i);
	}
}

vector<int> AhoCorasik::ans(const std::string& text) {
	int l = static_cast<int>(text.length());
	vector<int> enter(l, 0);
	int cur_ver = 0;
	int temp = static_cast<int>(str.length());

	if (pos.size() == 0) {
		for (int i = 0; i < l - temp + 1; ++i) {
			std::cout << i << " ";
		}
		return vector<int>();
	}

	for (int i = 0; i < l; ++i) {
		cur_ver = fast(cur_ver, text[i] - first_char);
		int suf_ver = cur_ver;

		while (suf_ver != 0) {
			if (trie[suf_ver].pat) {
				for (int j = 0; j < trie[suf_ver].indOfPat.size(); ++j) {
					int right = pos[trie[suf_ver].indOfPat[j]].second;

					int st = i - right;

					int right_border = st + static_cast<int>(str.length()) - 1;

					if (st >= 0 && right_border < l) {
						++enter[st];
					}
				}
			}

			suf_ver = suf(suf_ver);
		} 
	}

	return enter;
}

int AhoCorasik::fast(int ind, char c) {
	int temp = trie[ind].edges[c];

	if (trie[ind].move[c] == -1) {
		if (temp != -1) {
			trie[ind].move[c] = temp;
		} else if (ind == 0) {
			trie[ind].move[c] = 0;
		} else {
			trie[ind].move[c] = fast(suf(ind), c);
		}
	}

	return trie[ind].move[c];
}

int AhoCorasik::suf(int ind) {
	if (trie[ind].suffix == -1) {
		if (trie[ind].parent != 0) {
			trie[ind].suffix = fast(suf(trie[ind].parent), trie[ind].parCh);
		} else {
			trie[ind].suffix = 0;
		}
	}

	return trie[ind].suffix;
}

void AhoCorasik::findTermVer(int ind) {
	int cur_ind = ind;

	while (!trie[cur_ind].term.size() && cur_ind != 0) {
		if (trie[cur_ind].pat && ind != 0) {
			trie[ind].term.push_back(cur_ind);
		}
		ind = suf(ind);
	}

	for (int i = 0; i < trie[cur_ind].term.size(); ++i) {
		trie[ind].term.push_back(trie[cur_ind].term[i]);
	}
}

void parsing(const std::string& str, vector<std::pair<int, int>>& pos) {
	std::pair<int, int> cur_pos;

	char first_char = 'a';
	char sec_char = 'b';
	char question_char = '?';
	char last_char = 'z';

	if (first_char <= str[0] && str[0] <= last_char) {
		cur_pos.first = 0;
		if (str[1] == question_char) {
			cur_pos.second = 0;
			pos.push_back(cur_pos);
		}
	}

	int l = static_cast<int>(str.length()) - 1;

	for (int i = 1; i < l; ++i) {
		if (str[i - 1] == question_char && first_char <= str[i] && str[i] <= last_char) {
			cur_pos.first = i;
		}
		if (str[i + 1] == question_char && first_char <= str[i] && str[i] <= last_char) {
			cur_pos.second = i;
			pos.push_back(cur_pos);
		}
	}

	if (str[l - 1] == question_char && first_char <= str[l] && str[l] <= sec_char) {
		cur_pos.first = l;
		cur_pos.second = l;
		pos.push_back(cur_pos);
	}
	else if (first_char <= str[l] && str[l] <= sec_char) {
		cur_pos.second = l;
		pos.push_back(cur_pos);
	}
}

void solve(const string& pattern, const string& text) {
	vector<std::pair<int, int>> pos;

	parsing(pattern, pos);

	AhoCorasik temp(pattern, pos);

	vector<int> ans = temp.ans(text);

	int n = static_cast<int>(ans.size());

	for (int i = 0; i < n; i++) {
		if (ans[i] == static_cast<int>(pos.size())) {
			std::cout << i << " ";
		}
	}
}

int main() {
	string pattern, text;
	std::cin >> pattern >> text;

	solve(pattern, text);

	return 0;
}