#include <iostream>
#include <vector>
#include <cmath>

using std::vector;

struct Point {
	Point(double x = 0, double y = 0, double z = 0) :
		x(x), y(y), z(z) {}

	//координаты точки
	double x;
	double y;
	double z;
};

//квадрат расстояния между точками
double dist(Point p1, Point p2) {
	return (p1.x - p2.x)* (p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)* (p1.z - p2.z);
}

double distance(const vector<Point>& pointers) {
	double eps = 0.00000000001;
	Point v1(pointers[1].x - pointers[0].x, pointers[1].y - pointers[0].y, pointers[1].z - pointers[0].z);
	Point v2(pointers[3].x - pointers[2].x, pointers[3].y - pointers[2].y, pointers[3].z - pointers[2].z);
	Point p_1 = pointers[0];
	Point p_2 = pointers[2];
	Point np_1 = pointers[0];
	Point np_2 = pointers[2];
	Point p1;
	Point p2;
	vector<double> d;
	d.push_back(dist(pointers[0], pointers[2]));
	d.push_back(dist(pointers[0], pointers[3]));
	d.push_back(dist(pointers[1], pointers[3]));
	d.push_back(dist(pointers[1], pointers[2]));

	for (int i = 0; i < 10000; ++i) {
		double t_l = 0;
		double t_r = 1;
		p_1 = np_1;
		p_2 = np_2;

		while (t_r - t_l > eps) {
			double m1 = t_l + (t_r - t_l) / 3;
			double m2 = t_r - (t_r - t_l) / 3;
			p1.x = pointers[0].x + m1 * v1.x;
			p1.y = pointers[0].y + m1 * v1.y;
			p1.z = pointers[0].z + m1 * v1.z;

			p2.x = pointers[0].x + m2 * v1.x;
			p2.y = pointers[0].y + m2 * v1.y;
			p2.z = pointers[0].z + m2 * v1.z;

			if (dist(p1, p_2) > dist(p2, p_2)) {
				t_l = m1;
			} else {	
				t_r = m2;
			}
		}

		np_1 = p1;
		double s_l = 0;
		double s_r = 1;

		while (s_r - s_l > eps) {
			double m1 = s_l + (s_r - s_l) / 3;
			double m2 = s_r - (s_r - s_l) / 3;

			p1.x = pointers[2].x + m1 * v2.x;
			p1.y = pointers[2].y + m1 * v2.y;
			p1.z = pointers[2].z + m1 * v2.z;

			p2.x = pointers[2].x + m2 * v2.x;
			p2.y = pointers[2].y + m2 * v2.y;
			p2.z = pointers[2].z + m2 * v2.z;
			if (dist(p1, p_1) > dist(p2, p_1)) {
				s_l = m1;
			} else {
				s_r = m2;
			}
		}

		np_2 = p1;
	}
	
	d.push_back(dist(p_1, p_2));

	double ans = d[0];

	for (int i = 0; i < d.size(); ++i) {
		if (ans > d[i]) {
			ans = d[i];
		}
	}
	return sqrt(ans);
}

int main() {
	double x, y, z;//координаты
	vector<Point> pointers;

	for (int i = 0; i < 4; ++i) {
		std::cin >> x >> y >> z;
		Point temp(x, y, z);
		pointers.push_back(temp);
	}
	
	printf("%0.10f", distance(pointers));
	
	return 0;
}