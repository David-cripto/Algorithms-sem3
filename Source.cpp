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
using std::set;

struct Ans {
	int is_win;
	vector<int> who_killes;
};

set <int> findSetStatusOfChildes(int count_of_people, const vector<int>& previous_nims) {
	set<int> ans;

	for (int i = 1; i <= count_of_people; ++i) {
		//ищем с помощью теоремы Бутона, а именно XOR-сумма размеров кучек
		//предыдушие значения Шпрага-Гранди, подсчитаны на предыдущих уровнях рекурсии
		ans.insert(previous_nims[i - 1] ^ previous_nims[count_of_people - i]);
	}

	return ans;
}

int findMex(const set <int>& status_of_child_vertexes) {
	int ans = 0;
	int min = *(status_of_child_vertexes.begin());
	int max = *(status_of_child_vertexes.begin());

	for (set<int>::iterator it = status_of_child_vertexes.begin(); it != status_of_child_vertexes.end(); ++it) {
		if (min < *it) {
			min = *it;
		} 

		if (max > * it) {
			max = *it;
		}
	}
	
	while (true) {
		if (status_of_child_vertexes.find(ans) == status_of_child_vertexes.end()) {
			break;
		}
		else {
			++ans;
		}
	}

	return ans;
}

//функция для понимания, кто выиграл
//усли Мюллер, то false
//если Штирлиц, то return true + массив значений кого надо убивать с помощью прохождений по уровням реккурсии
Ans isSchtirlitzWin(int n) {
	Ans ans;

	vector<int> previous_nims(n + 1);

	//i -- уровень(глубина) погружения в рекурсию, 
	//по простому количество человек в очереди,  начиная с 2 до n
	for (int i = 2; i <= n; ++i) {
		//find set of cild's status and after we'll find mex 
		set <int> status_of_child_vertexes = findSetStatusOfChildes(i, previous_nims);

		previous_nims[i] = findMex(status_of_child_vertexes);
	}

	//если 0, то Штирлиц проиграл(по теореме Бутона), т.к. он всегда ходит первый
	//иначе выиграл Мюллер
	ans.is_win = previous_nims[n];

	if (ans.is_win) {
		//проходимся по уровням рекурсии
		//находим, кого убивать
		for (int i = 1; i <= n; ++i) {
			//т.к. из выигрышной вершины всегда идет ребро в проигрышную
			//поэтому находим проигрышные
			if (!(previous_nims[i - 1] ^ previous_nims[n - i])) {
				ans.who_killes.push_back(i);
			}

		}
	}

	return ans;
}

int main() {
	int n;
	std::cin >> n;
	
	Ans ans = isSchtirlitzWin(n);

	if (ans.is_win) {
		std::cout << "Schtirlitz" << std::endl;

		for (int i = 0; i < ans.who_killes.size(); ++i) {
			std::cout << ans.who_killes[i] << std::endl;
		}
	} else {
		std::cout << "Mueller" << std::endl;
	}

	return 0;
}