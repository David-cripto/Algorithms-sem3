#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>

using std::string;
using std::vector;

const int alphabet = 26;

struct Vertex {
	int left;
	int right;
	int suf;
	int parent;
	std::map<char, int> edges;

	Vertex(int left = -1, int right = -1, int parent = -1) :
		left(left), right(right), parent(parent), suf(0)
	{}
};

struct Suftree {
	Suftree(string& text, int len_of_first_str);

	void print();

private:
	void DFS_(int ver, int& i);

	void algorithmOfUkkonen_(char ch);

	int cur_v_;
	int cur_pos_;
	int cur_pos_str_;
	int len_of_first_str_;
	string text_;
	vector<Vertex> tree_;
	vector<int> re_num_ver_;
};


Suftree::Suftree(string& text, int len_of_first_str) :
	cur_v_(0), cur_pos_(0), cur_pos_str_(0),
	len_of_first_str_(len_of_first_str),
	text_(text), tree_(2, Vertex())
{
	const char first_char = 'a';
	int len = static_cast<int>(text.length());

	for (int i = 0; i < alphabet; ++i) { // 1 -- Джокер вершина. Мацкевич так называл вершину.
		char temp = first_char + i;
		tree_[1].edges[temp] = 0;
	}

	tree_[1].edges['#'] = 0;
	tree_[1].edges['$'] = 0;
	tree_[0].suf = 1;

	for (int i = 0; i < len; ++i) {
		char ch = text[i];
		algorithmOfUkkonen_(ch);
		++cur_pos_str_;
	}
}

void Suftree::print() {
	int i = 1;
	int len = static_cast<int>(tree_.size());
	re_num_ver_.resize(len, 0);//выстраиваю вершины в правльном порядке

	std::cout << len - 1 << "\n";

	DFS_(0, i);
}

void Suftree::DFS_(int ver, int& i) {

	if (ver != 0) {
		re_num_ver_[ver] = i++;
		std::cout << re_num_ver_[tree_[ver].parent] << " ";
		if (tree_[ver].left >= len_of_first_str_) {
			std::cout << "1" << " " << tree_[ver].left - len_of_first_str_ << " " << tree_[ver].right - len_of_first_str_ + 1 << "\n";
		}
		else if (tree_[ver].left < len_of_first_str_ && tree_[ver].right < len_of_first_str_) {
			std::cout << "0" << " " << tree_[ver].left << " " << tree_[ver].right + 1 << "\n";
		}
		else {
			std::cout << "0" << " " << tree_[ver].left << " " << len_of_first_str_ << "\n";
		}
	}

	for (auto it = tree_[ver].edges.begin(); it != tree_[ver].edges.end(); ++it) {
		DFS_(it->second, i);//DFS по значениям
	}
}

void Suftree::algorithmOfUkkonen_(char ch) {
	int len = static_cast<int>(text_.length());

	if (tree_[cur_v_].right < cur_pos_) { // вышли за ребро, тогда переходим к следующему ребенку
		if (tree_[cur_v_].edges.find(ch) == tree_[cur_v_].edges.end()) { // нет ребёнка по данному символу, делаем лист. Пункт 2а
			tree_.push_back(Vertex(cur_pos_str_, len - 1, cur_v_));
			tree_[cur_v_].edges[ch] = static_cast<int>(tree_.size()) - 1;
			cur_v_ = tree_[cur_v_].suf;
			cur_pos_ = tree_[cur_v_].right + 1;
			algorithmOfUkkonen_(ch);
			return;
		}

		cur_v_ = tree_[cur_v_].edges[ch];
		cur_pos_ = tree_[cur_v_].left;
	}

	if (cur_pos_ == -1 || ch == text_[cur_pos_]) {
		++cur_pos_; // совпадает пункт 3. Просто идём дальше
		return;
	}
	// пункт 2б разделяем ребро. Ох, как не хочу это делать :)
	tree_.push_back(Vertex(tree_[cur_v_].left, cur_pos_ - 1, tree_[cur_v_].parent));
	int n_ver = static_cast<int>(tree_.size()) - 1;
	tree_[n_ver].edges[text_[cur_pos_]] = cur_v_;
	tree_[cur_v_].left = cur_pos_;
	tree_[tree_[cur_v_].parent].edges[text_[tree_[n_ver].left]] = n_ver;
	tree_[cur_v_].parent = n_ver;

	tree_.push_back(Vertex(cur_pos_str_, len - 1, n_ver));
	int n_ver_2 = static_cast<int>(tree_.size()) - 1;
	tree_[n_ver].edges[text_[cur_pos_str_]] = n_ver_2;

	cur_v_ = tree_[tree_[n_ver].parent].suf;
	cur_pos_ = tree_[n_ver].left;

	while (cur_pos_ <= tree_[n_ver].right) { // находим конец суфф. 
		cur_v_ = tree_[cur_v_].edges[text_[cur_pos_]];
		cur_pos_ += 1 + tree_[cur_v_].right - tree_[cur_v_].left;
	}

	if (cur_pos_ == tree_[n_ver].right + 1) {// вершина
		tree_[n_ver].suf = cur_v_;
	}
	else {// не вершина
		tree_[n_ver].suf = n_ver_2 + 1;
	}

	cur_pos_ = tree_[cur_v_].right - (cur_pos_ - tree_[n_ver].right) + 2;// текущая позиция на новом ребре

	algorithmOfUkkonen_(ch);
}

int main() {
	string s, t;
	std::cin >> s >> t;
	string temp = s + t;
	int len = static_cast<int>(s.length());

	Suftree tree(temp, len);

	tree.print();

	return 0;
}