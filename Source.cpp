//Весь алгоритм взят из оригинальной статьи Чана по ссылке
//http ://tmc.web.engr.illinois.edu/ch3d/ch3d.pdf
//В этом коде будет очень много отсылок к этой статье :)
//Я не настолько ебн... мазахист, чтобы писать этот код
//Начал делать gift-wrapping, но потом друг сказал, что F и E
//можно решить одним алгоритмом -- Chan's algorithm...
//Ненавижу своих друзей(Штука :))

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <set>
#include <map>
#include <iomanip>

using std::vector;
using std::pair;

//вводится условное число для описания времени от -беск до +беск
const double infinity = 1e99;
struct  Point;
struct  Face;

void rotate(Point& p, double angle);
vector<Point*> downHull(std::vector<Point>& points, int left, int right);
void findBridge(Point*& u, Point*& v);
void findEvents(vector<Point*>& ans, vector<Point*>& rec1, vector<Point*>& rec2, Point*& u, Point*& v);
double crossPorduct(const Point* p1, const Point* p2, const Point* p3);
double calculateTime(const Point* p1, const Point* p2, const Point* p3);
void buildFace(vector<Face>& ans, vector<Point*>& points, int number_of_case);

enum Cases {
	FIRST_CASE,
	SECOND_CASE,
	THIRD_CASE,
	FOURTH_CASE,
	FIFTH_CASE,
	SIXTH_CASE,
};

struct Point {
	double x;
	double y;
	double z;
	int number;// номер точки по подаче значений для вывода соответствующему условие задачи
	Point* next;//вершинка с индексом +, как в статье
	Point* prev;//вершинка с индексом -, как в статье
	bool use;

	Point(double x = 0, double y = 0, double z = 0, int number = -1)
		: x(x), y(y), z(z)
		, number(number)
		, next(nullptr)
		, prev(nullptr)
		, use(false) {}

	// функция совершения события, т.е. либо добавление точки, либо удаление
	//P.S. вначале сделал как в статье void, но в функции algChan 
	//понадобилось возвращать bool
	bool doEvent() {
		// добавление
		if (prev->next != this) {
			prev->next = this;
			next->prev = this;
			return true;
		}
		else {//удаление
			prev->next = next;
			next->prev = prev;
			return false;
		}
	}

	Point& operator += (const Point& other);
	Point& operator -= (const Point& other);
};

std::istream& operator >> (std::istream& in, Point& temp) {
	in >> temp.y >> temp.x;
	temp.z = temp.x * temp.x + temp.y * temp.y;
	return in;
}

Point operator + (const Point& other1, const Point& other2);
Point operator - (const Point& other1, const Point& other2);
Point operator * (double number, const Point& other);
Point operator * (const Point& other, double number);

Point& Point::operator+=(const Point& other) {
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;

	return *this;
}

Point& Point::operator-=(const Point& other) {
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;

	return *this;
}

Point operator-(const Point& other1, const Point& other2) {
	Point temp(other1);

	temp -= other2;

	return temp;
}

Point operator*(double number, const Point& other) {
	Point temp(other);
	temp.x *= number;
	temp.y *= number;
	temp.z *= number;

	return temp;
}

Point operator*(const Point& other, double number) {
	Point temp(other);
	temp.x *= number;
	temp.y *= number;
	temp.z *= number;

	return temp;
}

Point operator+(const Point& other1, const Point& other2) {
	Point temp(other1);

	temp += other2;

	return temp;
}

//грани многоугольника
struct Face {

	//точки из которых состоит грань
	int f_point;
	int s_point;
	int th_point;

	Face(int first, int second, int third)
		: f_point(first)
		, s_point(second)
		, th_point(third) {}

	Face() = default;

	void sortVertex() {
		if (s_point < f_point && s_point < th_point) {
			std::swap(f_point, s_point);
			std::swap(s_point, th_point);
		}
		else if (th_point < f_point && th_point < s_point) {
			std::swap(s_point, th_point);
			std::swap(f_point, s_point);
		}
	}
};

//компаратор для сортировки по х, применяем когда проецируем
struct CompPoint {
	bool operator()(const Point& p1, const Point& p2) {
		return p1.x < p2.x;
	}
}cmp_point;

vector<Face>algChan(vector<Point>& all_points) {
	int n = static_cast<int>(all_points.size());
	//сортируем точки по х
	//Для чего?
	//Ответ с отсылкой на статью: "Sort the points pˆ1, . . . , pˆn in increasing x-coordinates (recalling
	//that points only move vertically).We use divide - and -conquer to solve the kinetic 2 - d problem."
	std::sort(all_points.begin(), all_points.end(), cmp_point);

	//самая интересная часть алогритма
	//Идея:
	//Строим down hull и upper hull(Они строятся симметрично, поэтому
	//хватит одной функции -- downHull)
	//потом склеиваем их и получаем нужную нам оболочку
	vector<Point*> points = downHull(all_points, 0, n);
	vector<Face> ans;

	buildFace(ans, points, 1);//строим грани из точек нах в down hull

	//переставляем точки для upper hull
	for (int i = 0; i < n; ++i) {
		all_points[i].next = nullptr;
		all_points[i].prev = nullptr;
		all_points[i].z = -all_points[i].z;
	}
	points = downHull(all_points, 0, n);

	buildFace(ans, points, 2);//строим грани из точек нах в upper hull

	return ans;
}

//Ищем event(один из 6-ти случаев в статье) рекурсивно с помощью divide - and -conquer 
vector<Point*> downHull(std::vector<Point>& points, int left, int right) {
	//осталась одна точка возвращаем пустой вектор
	if (right - left == 1) {
		vector<Point*> a;
		return a;
	}

	int med = (left + right) / 2;

	vector<Point*> rec1 = downHull(points, left, med);
	vector<Point*> rec2 = downHull(points, med, right);

	Point* u = &points[med - 1];
	Point* v = &points[med];

	//находим мост
	findBridge(u, v);

	vector<Point*> ans;

	//пробегаем все события времени
	findEvents(ans, rec1, rec2, u, v);

	//Как написано в статье "now go back in time to update pointers"
	u->next = v;
	v->prev = u;
	int n_ans = static_cast<int>(ans.size());

	for (int i = n_ans - 1; i >= 0; --i) {
		Point* cur_ver = ans[i];

		//левее u в L, либо правее v в R по x, то совершаем событие
		if (cur_ver->x <= u->x || cur_ver->x >= v->x) {
			cur_ver->doEvent();
			if (cur_ver == u) {
				u = u->prev;
			}
			else if (cur_ver == v) {
				v = v->next;
			}
		}
		else {
			v->prev = cur_ver;
			u->next = cur_ver;
			cur_ver->prev = u;
			cur_ver->next = v;
			if (cur_ver->x <= points[med - 1].x) {
				u = cur_ver;
			}
			else {
				v = cur_ver;
			}
		}
	}

	return ans;
}

//функция поиска bridge
void findBridge(Point*& u, Point*& v) {
	//Ищем bridge начиная с "starts with u = pˆbn/2c and 
	//v = pˆbn/2c+1 and repeatedly advances u leftward on L 
	//and v rightward on R, until u−uv and uvv+ both form 
	//counterclockwise turns"
	while (true) {
		if (crossPorduct(u, v, v->next) < 0) {
			v = v->next;
		}
		else if (crossPorduct(u->prev, u, v) < 0) {
			u = u->prev;
		}
		else {
			break;
		}
	}
}

void findEvents(vector<Point*>& ans, vector<Point*>& rec1, vector<Point*>& rec2, Point*& u, Point*& v) {
	//ставим время на -беск и будем искать время, где случаются
	//одно из шести событий, если несколько, то берем мин из них
	double cur_t = -infinity;

	int temp1 = 0;//чекаем точки в L
	int temp2 = 0;//чекаем точки в R
	while (true) {
		Point* l = nullptr;
		Point* r = nullptr;
		//массив времни случаев каждая ячейка -- определенный случай
		vector<double> time(6, infinity);

		//Далее на каждый случай цитата со статьи, описывающая его

		//Случай, когда мы смотрим на левую точку и ищем момент для неё 
		int n_r1 = static_cast<int>(rec1.size());
		if (temp1 < n_r1) {// смотрим, что не вышли за L
			l = rec1[temp1];
			time[0] = calculateTime(l->prev, l, l->next);
		}
		int n_r2 = static_cast<int>(rec2.size());
		//Аналогичный случай, только для правой
		if (temp2 < n_r2) {// смотрим, что не вышли за R
			r = rec2[temp2];
			time[1] = calculateTime(r->prev, r, r->next);
		}

		//When uvv+ turns clockwise, uv+ becomes the new bridge.
		//Then A undergoes deletion of v(between u and v+ ).
		time[2] = calculateTime(u, v, v->next);

		//When uv−v turns counterclockwise, uv− becomes the new bridge.
		//Then A undergoes insertion of v− between u and v
		time[3] = calculateTime(u, v->prev, v);

		//When u−uv turns clockwise, u−v becomes the new bridge.
		//Then A undergoes deletion of u(between u− and v).
		time[4] = calculateTime(u->prev, u, v);

		//When uu+v turns counterclockwise, u+v becomes the new bridge.
		//Then A undergoes insertion of u+ between u and v.
		time[5] = calculateTime(u, u->next, v);

		int min_ind = -1;
		double min_time = infinity;
		for (int i = 0; i < 6; i++) {
			if (time[i] > cur_t&& time[i] < min_time) {
				min_time = time[i];
				min_ind = i;
			}
		}

		if (min_time >= infinity) {//не нашли время 
			break;
		}

		//перебор случаев
		if (min_ind == FIRST_CASE) {
			// левее u в L
			if (l->x < u->x) {
				ans.push_back(l);
			}
			l->doEvent();
			temp1++;
		}
		else if (min_ind == SECOND_CASE) {
			// левее r в R
			if (r->x > v->x) {
				ans.push_back(r);
			}
			r->doEvent();
			temp2++;
		}
		else if (min_ind == THIRD_CASE) {
			ans.push_back(v);
			v = v->next;
		}
		else if (min_ind == FOURTH_CASE) {
			v = v->prev;
			ans.push_back(v);
		}
		else if (min_ind == FIFTH_CASE) {
			ans.push_back(u);
			u = u->prev;
		}
		else if (min_ind == SIXTH_CASE) {
			u = u->next;
			ans.push_back(u);
		}

		cur_t = min_time;
	}
}

double calculateTime(const Point* p1, const Point* p2, const Point* p3) {
	// event произойдет на +беск(т.е. не произойдет :))
	if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
		return infinity;
	}
	return ((p2->x - p1->x) * (p3->z - p2->z) - (p2->z - p1->z) * (p3->x - p2->x)) / crossPorduct(p1, p2, p3);
}

double crossPorduct(const Point* p1, const Point* p2, const Point* p3) {
	if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
		return true;
	}
	return (p2->x - p1->x) * (p3->y - p2->y) - (p2->y - p1->y) * (p3->x - p2->x);
}

void buildFace(vector<Face>& ans, vector<Point*>& points, int number_of_case) {
	int n_p = static_cast<int>(points.size());

	//после downHull в points лежат точки, которые образуют грани
	for (int i = 0; i < n_p; ++i) {
		//строим грань из points[i]
		Face cur_face(points[i]->prev->number, points[i]->number, points[i]->next->number);

		//меняем точки внутр грани
		if (number_of_case == 1 && !points[i]->doEvent()) {//заходит, если точка была удалена в 1-ом случае
			std::swap(cur_face.f_point, cur_face.s_point);
		}
		else if (number_of_case == 2 && points[i]->doEvent()) {//заходит, если точка была добавлена во 2-ом случае
			std::swap(cur_face.f_point, cur_face.s_point);
		}
		ans.push_back(cur_face);
	}
}

//компаратор для сортировки граней
struct CompFaces {
	//вроде понятая штука
	//смотри по первой точке, потом второй и наконец третьей
	bool operator()(const Face& f1, const Face& f2) {
		if (f1.f_point < f2.f_point) {
			return true;
		}
		else if (f1.f_point > f2.f_point) {
			return false;
		}
		else if (f1.s_point < f2.s_point) {
			return true;
		}
		else if (f1.s_point > f2.s_point) {
			return false;
		}
		else if (f1.th_point < f2.th_point) {
			return true;
		}
		else {
			return false;
		}
	}
}cmp_face;

//это функцию я подсмотрел кое-где. 
//Дело в том, что типикал Чан не заходит,
//и я правда очень дого провозился с этим
//Даже первый тест не заходил просто тупил и заходит в тл
//1
//4
//0 0 0
//1 0 0
//0 1 0
//0 0 1
//А дело в том, что 2 точки лежт по одной координате x
//и при проецировании соответственно получается лажа.
//Эта функция поворачитвает точки с помощью "замены координат"
//как при повороте базисных векторов на угол angle относительно 
//соответствующей оси
void rotate(Point& p, double angle) {
	//новые координаты по x, y, z
	double new_x;
	double new_y;
	double new_z;

	//Поворачиваем относительно оси Y и видим поворот плоскости OZX на угол angle
	new_x = p.x * cos(angle) + p.z * sin(angle);
	new_z = -p.x * sin(angle) + p.z * cos(angle);
	p.x = new_x;
	p.z = new_z;

	//Поворачиваем относительно оси X и видим поворот плоскости OZX на угол angle
	new_z = p.z * cos(angle) + p.y * sin(angle);
	new_y = -p.z * sin(angle) + p.y * cos(angle);
	p.z = new_z;
	p.y = new_y;

	//Поворачиваем относительно оси Z и видим поворот плоскости OZX на угол angle
	new_x = p.x * cos(angle) + p.y * sin(angle);
	new_y = -p.x * sin(angle) + p.y * cos(angle);
	p.x = new_x;
	p.y = new_y;
}


////////////////////////////////////////////////////////////////////////////////////////
//void findEdgesInFaces(std::map<pair<int, int>, int>& edges, int a, int b) {
//	pair<int, int> edge = std::make_pair(a, b);
//
//	if (edge.first > edge.second) {
//		std::swap(edge.first, edge.second);
//	}
//
//	if (edges.find(edge) == edges.end()) {
//		edges[edge] = 1;
//	}
//	else {
//		++edges[edge];
//	}
//}

void findEdgesInFaces(std::set<pair<int, int>>& edges, int a, int b) {
	pair<int, int> edge = std::make_pair(a, b);

	if (edge.first > edge.second) {
		std::swap(edge.first, edge.second);
	}
	edges.insert(edge);
}


void findPointsPl(vector<Point>& points, vector<Point>& planar_hull) {
	for (const Point& point : points) {

		while (planar_hull.size() >= 2) {
			if (crossPorduct(&planar_hull[planar_hull.size() - 2], &planar_hull.back(), &point) > 0) {
				break;
			}
			planar_hull.pop_back();
		}

		planar_hull.push_back(point);
	}
}

void revPl(vector<Point>& pointers, vector<Point>& planar_hull) {
	int n_p_h = static_cast<int>(planar_hull.size());

	for (int i = pointers.size() - 2; i >= 0; --i) {
		while (static_cast<int>(planar_hull.size()) > n_p_h) {
			if (crossPorduct(&planar_hull[planar_hull.size() - 2], &planar_hull.back(), &pointers[i]) > 0) {
				break;
			}
			planar_hull.pop_back();
		}
		planar_hull.push_back(pointers[i]);
	}
}

//Код написанный выше полнстью взять из решения С1
//Идея:
//Проецируем точки на парабалоид и строим 
//convex hull, которая выдаст нам триангулляцю 
//из триангуляции получаем диагрмму Вороного
double findAlmostAllNumbersOfEdgesOfVoronoiDiagrams(vector<Point>& points) {
	//vector<Face> convex_hull = algChan(pointers);

	//int n = static_cast<int>(pointers.size());

	//std::map<pair<int, int>, int>edges;
	//int n_c_h = static_cast<int>(convex_hull.size());
	//int del_edges = 0;
	//int count_of_non_close_polygon = 0;
	//vector<int> edge_from_site(n, 0);
	//vector<bool> is_in_ceil(n, true);

	////сопоставляю каждому ребру его количество входов в грань
	//for (int i = 0; i < n_c_h; ++i) {
	//	findEdgesInFaces(edges, convex_hull[i].f_point, convex_hull[i].s_point);
	//	findEdgesInFaces(edges, convex_hull[i].s_point, convex_hull[i].th_point);
	//	findEdgesInFaces(edges, convex_hull[i].th_point, convex_hull[i].f_point);
	//}

	//int n_m = static_cast<int>(edges.size());

	////количество ребер выоходящей с каждой вершины
	//for (const auto& elem : edges) {
	//	++edge_from_site[elem.first.first];
	//	++edge_from_site[elem.first.second];
	//	//на оболочке
	//	if (elem.second == 1) { //ребро вошло один раз в грань
	//		is_in_ceil[elem.first.first] = false;
	//		is_in_ceil[elem.first.second] = false;

	//	}
	//}

	////количество многоугольников
	//for (int i = 0; i < n; ++i) {
	//	count_of_non_close_polygon += is_in_ceil[i];
	//}

	////если ребро вошло вдве грани => считаем кол-во ребер из сайта и складываем в ответ
	//for (int i = 0; i < n; ++i) {
	//	if (is_in_ceil[i]) {
	//		del_edges += edge_from_site[i];
	//	}
	//}

	//if (count_of_non_close_polygon == 0) {
	//	std::cout.precision(6);
	//	std::cout << std::fixed << 0;
	//} else {
	//	std::cout.precision(6);
	//	std::cout << std::fixed << std::setprecision(6) << static_cast<double>(del_edges / count_of_non_close_polygon);
	//}

	vector<Face> convex_hull = algChan(points);
	std::set<pair<int, int>> edges;
	vector<int> edges_number(points.size());
	int n_c_h = static_cast<int>(convex_hull.size());
	vector<Point> planar_hull;//оболочка на плоскости
	vector<bool> is_in_ceil(points.size(), false);

	for (int i = 0; i < n_c_h; ++i) {
		findEdgesInFaces(edges, convex_hull[i].f_point, convex_hull[i].s_point);
		findEdgesInFaces(edges, convex_hull[i].s_point, convex_hull[i].th_point);
		findEdgesInFaces(edges, convex_hull[i].th_point, convex_hull[i].f_point);
	}

	for (const pair<int, int>& edge : edges) {
		edges_number[edge.first]++;
		edges_number[edge.second]++;
	}

	findPointsPl(points, planar_hull);

	revPl(points, planar_hull);

	for (Point& i : planar_hull) {
		is_in_ceil[i.number] = true;
	}

	int count_of_non_close_polygon = 0;
	int del_edges = 0;
	for (int i = 0; i < points.size(); i++) {
		if (!is_in_ceil[i]) {
			del_edges += edges_number[i];
			count_of_non_close_polygon++;
		}
	}

	if (count_of_non_close_polygon == 0) {
		return 0;
	}
	else {
		return static_cast<double>(del_edges) / count_of_non_close_polygon;
	}
}

int main() {
	vector<Point> points;
	double x, y;
	int ind = 0;
	Point temp;//проецируем на парабалоид

	while (std::cin >> temp) {
		temp.number = ind;
		rotate(temp, 0.000001);
		points.push_back(temp);
		ind++;
	}

	std::cout.precision(6);
	std::cout << std::fixed << findAlmostAllNumbersOfEdgesOfVoronoiDiagrams(points);

	return 0;
}