#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using std::vector;

struct Point {
	Point(double x = 0, double y = 0, double z = 0) :
		x(x), y(y), z(z) {}

	Point(const Point& other) :
		x(other.x), y(other.y), z(other.z) {}

	//координаты точки
	double x;
	double y;
	double z;

	Point& operator += (const Point& other);
	Point& operator -= (const Point& other);
};

Point operator + (const Point& other1, const Point& other2);
Point operator - (const Point& other1, const Point& other2);
Point operator * (double number, const Point& other);
Point operator * (const Point& other, double number);

//квадрат расстояния между точками
double dist(Point p1, Point p2) {
	return (p1.x - p2.x)* (p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)* (p1.z - p2.z);
}

double ternarySearch(const Point& point1, const Point& point2, const Point& point3, const Point& point4) {
	double eps = 0.00000000001;
	Point first_segment(point2 - point1);
	Point second_segment(point4 - point3);
	Point fixed_point_in_first_segment(point1);
	Point fixed_point_in_second_segment(point3);
	Point new_change_point_for_fixed_point_in_first_segment(point1);
	Point new_change_point_for_fixed_point_in_second_segment(point3);
	Point change_point_in_first_segment;
	Point change_point_in_second_segment;
	vector<double> arr_of_distance;

	arr_of_distance.push_back(dist(point1, point3));
	arr_of_distance.push_back(dist(point1, point4));
	arr_of_distance.push_back(dist(point2, point4));
	arr_of_distance.push_back(dist(point2, point3));

	for (int i = 0; i < 10000; ++i) {
		double t_l = 0;
		double t_r = 1;
		fixed_point_in_first_segment = new_change_point_for_fixed_point_in_first_segment;
		fixed_point_in_second_segment = new_change_point_for_fixed_point_in_second_segment;

		while (t_r - t_l > eps) {
			double m1 = t_l + (t_r - t_l) / 3;
			double m2 = t_r - (t_r - t_l) / 3;

			change_point_in_first_segment = point1 + m1 * first_segment;
			change_point_in_second_segment = point1 + m2 * first_segment;

			if (dist(change_point_in_first_segment, fixed_point_in_second_segment) > dist(change_point_in_second_segment, fixed_point_in_second_segment)) {
				t_l = m1;
			} else {	
				t_r = m2;
			}
		}

		new_change_point_for_fixed_point_in_first_segment = change_point_in_first_segment;
		double s_l = 0;
		double s_r = 1;

		while (s_r - s_l > eps) {
			double m1 = s_l + (s_r - s_l) / 3;
			double m2 = s_r - (s_r - s_l) / 3;

			change_point_in_first_segment = point3 + m1 * second_segment;
			change_point_in_second_segment = point3 + m2 * second_segment;

			if (dist(change_point_in_first_segment, fixed_point_in_first_segment) > dist(change_point_in_second_segment, fixed_point_in_first_segment)) {
				s_l = m1;
			} else {
				s_r = m2;
			}
		}

		new_change_point_for_fixed_point_in_second_segment = change_point_in_first_segment;
	}
	
	arr_of_distance.push_back(dist(fixed_point_in_first_segment, fixed_point_in_second_segment));

	double ans = arr_of_distance[0];

	for (int i = 0; i < arr_of_distance.size(); ++i) {
		if (ans > arr_of_distance[i]) {
			ans = arr_of_distance[i];
		}
	}
	return sqrt(ans);
}

std::istream& operator >> (std::istream& in, Point& temp) {
	in >> temp.x >> temp.y >> temp.z;
	return in;
}

int main() {
	Point point1, point2, point3, point4;
	
	std::cin >> point1 >> point2 >> point3 >> point4;

	std::cout.precision(10);
	std::cout << std::fixed << std::setprecision(10) << ternarySearch(point1, point2, point3, point4);

	return 0;
}

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

