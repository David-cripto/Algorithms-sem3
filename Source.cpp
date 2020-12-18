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
	vector<int> ind_of_pat;
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

	AhoCorasik(const std::string& str, const vector<std::pair<int, int>>& pos);

	vector<int> getEntries(const std::string& text);

	
private:
	string str_;
	vector<std::pair<int, int>> pos_;
	vector<Vertex> trie_;

	int fast_(int ind, char c);

	int suf_(int ind);

	void findTermVer_(int ind);
};

AhoCorasik::AhoCorasik(const std::string& str, const vector<std::pair<int, int>>& pos)
	:str_(str), trie_(1, Vertex(0, -1)), pos_(pos) 
{
	char first_char = 'a';
	trie_[0].suffix = 0;

	//////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////

	for (int i = 0; i < static_cast<int>(pos.size()); ++i) {
		int cur_ver = 0;
		for (int j = pos[i].first; j <= pos[i].second; ++j) {
			char temp = str[j] - first_char;
			if (trie_[cur_ver].edges[temp] == -1) {
				trie_.push_back(Vertex(cur_ver, temp));
				trie_[cur_ver].edges[temp] = static_cast<int>(trie_.size()) - 1;
			}
			cur_ver = trie_[cur_ver].edges[temp];
		}
		trie_[cur_ver].pat = true;
		trie_[cur_ver].ind_of_pat.push_back(i);
	}
}

vector<int> AhoCorasik::getEntries(const std::string& text) {
	int l = static_cast<int>(text.length());
	vector<int> entries(l, 0);
	int cur_ver = 0;
	int temp = static_cast<int>(str_.length());
	char first_char = 'a';

	if (pos_.size() == 0) {
		for (int i = 0; i < l - temp + 1; ++i) {
			std::cout << i << " ";
		}
		return vector<int>();
	}

	for (int i = 0; i < l; ++i) {
		cur_ver = fast_(cur_ver, text[i] - first_char);
		int suf_ver = cur_ver;

		while (suf_ver != 0) {
			if (trie_[suf_ver].pat) {
				for (int j = 0; j < trie_[suf_ver].ind_of_pat.size(); ++j) {
					int right = pos_[trie_[suf_ver].ind_of_pat[j]].second;

					int st = i - right;

					int right_border = st + static_cast<int>(str_.length()) - 1;

					if (st >= 0 && right_border < l) {
						++entries[st];
					}
				}
			}

			suf_ver = suf_(suf_ver);
		} 
	}

	return entries;
}

int AhoCorasik::fast_(int ind, char c) {
	int temp = trie_[ind].edges[c];

	if (trie_[ind].move[c] == -1) {
		if (temp != -1) {
			trie_[ind].move[c] = temp;
		} else if (ind == 0) {
			trie_[ind].move[c] = 0;
		} else {
			trie_[ind].move[c] = fast_(suf_(ind), c);
		}
	}

	return trie_[ind].move[c];
}

int AhoCorasik::suf_(int ind) {
	if (trie_[ind].suffix == -1) {
		if (trie_[ind].parent != 0) {
			trie_[ind].suffix = fast_(suf_(trie_[ind].parent), trie_[ind].parCh);
		} else {
			trie_[ind].suffix = 0;
		}
	}

	return trie_[ind].suffix;
}

void AhoCorasik::findTermVer_(int ind) {
	int cur_ind = ind;

	while (!trie_[cur_ind].term.size() && cur_ind != 0) {
		if (trie_[cur_ind].pat && ind != 0) {
			trie_[ind].term.push_back(cur_ind);
		}
		ind = suf_(ind);
	}

	for (int i = 0; i < trie_[cur_ind].term.size(); ++i) {
		trie_[ind].term.push_back(trie_[cur_ind].term[i]);
	}
}

vector<std::pair<int, int>> parseString(const std::string& str) {
	vector<std::pair<int, int>> pos;
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

	return pos;
}

vector<int> solve(const string& pattern, const string& text) {
	vector<std::pair<int, int>> pos = parseString(pattern);

	AhoCorasik temp(pattern, pos);

	vector<int> entries = temp.getEntries(text);

	int n = static_cast<int>(entries.size());
	vector<int> ans;

	for (int i = 0; i < n; i++) {
		if (entries[i] == static_cast<int>(pos.size())) {
			ans.push_back(i);
		}
	}

	return ans;
}

int main() {
	string pattern, text;
	std::cin >> pattern >> text;

	vector<int> ans = solve(pattern, text);

	for (int i = 0; i < static_cast<int>(ans.size()); i++) {
		std::cout << ans[i] << " ";
	}
	return 0;
}