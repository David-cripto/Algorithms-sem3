#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <iomanip>

using std::stack;
using std::vector;
using std::iterator;


struct Point {
    Point(long long x = 0, long long y = 0) :
        x(x), y(y) {}

    long long x;
    long long y;
};

std::istream& operator >> (std::istream& in, Point& temp) {
    in >> temp.x >> temp.y;
    return in;
}

//квадрат расстояния
long double squaredDist(const Point& p1, const Point& p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

//расстояние
long double dist(const Point& p1, const Point& p2) {
    return sqrt(squaredDist(p1, p2));
}

//векторное произведение для угла
long double crossPorduct(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
    return (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
}

//Сумма минковского
//Возвращает новую оболочку -- A+B(сумма Минковского)
vector<Point> sumOfMink(vector<Point>& A, vector<Point>& B) {
    vector<Point> ans;

    long long i = 0;//increment of A convex hull

    long long j = 0;//increment of B convex hull

    long long n = static_cast<long long>(A.size());
    long long m = static_cast<long long>(B.size());

    A.push_back(A[0]);
    A.push_back(A[1]);

    B.push_back(B[0]);
    B.push_back(B[1]);

    while (i < n || j < m) {
        ans.push_back(Point(A[i].x + B[j].x, A[i].y + B[j].y));
        if (crossPorduct(A[i], A[i + 1], B[j], B[j + 1]) > 0) {//берем строна из А
            ++i;
        } else if (crossPorduct(A[i], A[i + 1], B[j], B[j + 1]) < 0) {//берем строна из В
            ++j;
        } else {//берем две стороны
            ++i;
            ++j;
        }
    }

    return ans;
}

//считаем площадь многоугольника
//по формуле https ://ru.wikipedia.org/wiki/%D0%A4%D0%BE%D1%80%D0%BC%D1%83%D0%BB%D0%B0_%D0%BF%D0%BB%D0%BE%D1%89%D0%B0%D0%B4%D0%B8_%D0%93%D0%B0%D1%83%D1%81%D1%81%D0%B0
long double areaOfPolygon(const vector<Point>& A) {
    long double ans = 0;
    long long n = static_cast<long long>(A.size());
    for (long long i = 0; i < n - 1; ++i) {
        ans += A[i].x * A[i + 1].y;
        ans -= A[i + 1].x * A[i].y;
    }

    ans += A[n - 1].x * A[0].y;
    ans -= A[0].x * A[n - 1].y;

    ans = std::abs(ans);

    ans /= 2;

    return ans;
}

long long findMinYInd(const vector<Point>& A) {
    long long n = static_cast<long long>(A.size());
    long long ans = 0;

    for (long long i = 0; i < n; ++i) {
        if ((A[ans].y > A[i].y) || (A[ans].y == A[i].y && A[ans].x > A[i].x)) {
            ans = i;
        }
    }

    return ans;
}

long double findMixedArea(vector<Point>& A, vector<Point>& B) {
    long long ind_A = findMinYInd(A);
    long long ind_B = findMinYInd(B);

    long long n_A = static_cast<long long>(A.size());
    long long n_B = static_cast<long long>(B.size());

    //циклические сдвиги
    std::rotate(A.begin(), A.begin() + ind_A, A.end());
    std::rotate(B.begin(), B.begin() + ind_B, B.end());

    long double area_A = areaOfPolygon(A);
    long double area_B = areaOfPolygon(B);

    vector<Point> C = sumOfMink(A, B);
    long double area_C = areaOfPolygon(C);

    long double ans = (area_C - area_A - area_B) / 2;

    return ans;
}

int main() {
    vector<Point> A;
    vector<Point> B;

    long long n, m;
    std::cin >> n;

    for (long long i = 0; i < n; ++i) {
        Point temp;
        std::cin >> temp;
        A.push_back(temp);
    }

    std::cin >> m;

    for (long long i = 0; i < m; ++i) {
        Point temp;
        std::cin >> temp;
        B.push_back(temp);
    }
    
    std::cout.precision(6);
    std::cout << std::fixed << std::setprecision(6) << findMixedArea(A, B);

    return 0;
}