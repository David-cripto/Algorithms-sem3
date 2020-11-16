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

struct ConvexHull {
    ConvexHull(vector<Point>& pointers) :
        pointers(pointers), perimetr(0) {}

    void algGraham();

private:
    vector<Point> pointers;//точки на плоскости
    Point p0;//самая нижняя и левая точка. От неё будет идти отсчёт
    long double perimetr;
};

Point nextToTop(stack<Point>& S) {
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

//квадрат расстояния между точками
long double dist(Point p1, Point p2) {
    return static_cast<long double>((p1.x - p2.x))* static_cast<long double>((p1.x - p2.x)) + static_cast<long double>((p1.y - p2.y))* static_cast<long double>((p1.y - p2.y));
}

//смотрим вектор pp1 и p1p2. смотрим на ориентация с помощью векторного произведения
int crossProduct(Point p, Point p1, Point p2) {
    long long det = (p1.x - p.x) * (p2.y - p1.y) - (p1.y - p.y) * (p2.x - p1.x);//определитель матрицы

    if (det == 0) {//определитель матрицы равен нулю => вектора коллинеарны
        return 0;
    }
    else if (det > 0) {//против часовой
        return 2;
    }
    else {//по часовой
        return 1;
    }
}

// компаратор для qsort
struct Compare {
    bool operator()(const Point& p1, const Point& p2) {
        //находим ориентацию
        long long det = crossProduct(p0, p1, p2);
        if (det == 0) {//коллинеарность
            if (dist(p0, p2) > dist(p0, p1)) {
                return true;
            }
            else {
                return false;
            }
        }
        else if (det == 2) {//против часовой
            return true;
        }
        else {//по часовой
            return false;
        }
    }
    Compare(Point p0) :
        p0(p0) {}

private:
    Point p0;
};

int main() {
    long long n;
    std::cin >> n;
    vector<Point> pointers;

    for (int i = 0; i < n; ++i) {
        long long x;
        long long y;
        std::cin >> x >> y;
        Point temp(x, y);
        pointers.push_back(temp);
    }

    ConvexHull ans(pointers);
    ans.algGraham();
    return 0;
}

void ConvexHull::algGraham() {
    long long n = static_cast<long long>(pointers.size());
    // ищем нижнюю точку
    long long ymin = pointers[0].y;
    long long min = 0;
    for (long long i = 1; i < n; i++) {
        long long y = pointers[i].y;

        if ((y < ymin) || (ymin == y && pointers[i].x < pointers[min].x)) {// сорт по y, потом по x. Самый нижние => самые левые из нижних 
            ymin = pointers[i].y;
            min = i;
        }
    }

    std::swap(pointers[0], pointers[min]);//на нулевую позицию ставим наш минимум

    // сортирую относительно моей точки по углу. На хабре такая красивая картинка была, жалко не могу её сюда вставить :(
    Compare comp(pointers[0]);
    vector<Point>::iterator it = pointers.begin();
    ++it;
    std::sort(it, pointers.end(), comp);

    //закидываем в стек первые три точки. точка 0 и 1 точно принадлежат вып. оболочке
    stack<Point> S;
    S.push(pointers[0]);
    S.push(pointers[1]);
    S.push(pointers[2]);

    for (int i = 3; i < n; ++i) {
        Point np = pointers[i];

        //while (crossProduct(nextToTop(S), S.top(), pointers[i]) == 0) {//лежат на одной прямой
        //    

        //}

        while (S.size() > 1 && crossProduct(nextToTop(S), S.top(), pointers[i]) != 2) {// по часовой, удаляем
            S.pop();
        }
        S.push(pointers[i]);//против часовой пушим в стек
    }

    Point p0 = S.top();
    Point p1 = S.top();
    S.pop();

    while (!S.empty()) {
        Point p2 = S.top();
        perimetr += sqrt(dist(p1, p2));
        p1 = p2;
        S.pop();
    }
    perimetr += sqrt(dist(p0, p1));

    std::cout << std::setprecision(10) << perimetr;
}
