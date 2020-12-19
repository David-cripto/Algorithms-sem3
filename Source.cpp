#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <iomanip>

using std::stack;
using std::vector;
using std::iterator;
using std::pair;

const double EPS = 1e-9;// для вычилсений с точностью

struct Segment;
bool intersect(const Segment& a, const Segment& b);

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

struct Segment {
    Point begin;
    Point end;
    //номер отрезка для вывода соответствующему условию задачи
    int number;

    Segment() = default;

    Segment(const Point& begin, const Point& end, int number) :
        begin(begin), end(end), number(number) {}

    friend bool operator<(const Segment& a, const Segment& b);
    double get_y(double x) const {
        //выводим у если не хватает точности(верт отрезки), то выодим начало у
        if (std::abs(begin.x - end.x) < EPS) {
            return begin.y;
        }

        return begin.y + (end.y - begin.y) * (x - begin.x) / (end.x - begin.x);
    }
};

std::istream& operator >> (std::istream& in, Segment& temp) {
    Point begin;
    Point end;

    in >> begin >> end;

    temp.begin = begin;
    temp.end = end;

    return in;
}

struct Event {
    // события происходят по х
    int event_x;

    //номер отрезка
    int numner_of_segment;

    //добавлять или удалять отрезок
    bool is_begin;

    Event(int event_x, int number_of_segment, bool is_begin) :
        event_x(event_x), numner_of_segment(number_of_segment),
        is_begin(is_begin) {}
};

//компаратор для Event
struct CompEvents {
    bool operator()(const Event& e1, const Event& e2) {
        if (std::abs(e1.event_x - e2.event_x) < EPS) {// события по одной вертикали
            return e1.is_begin > e2.is_begin;
        }

        return e1.event_x < e2.event_x;
    }
}cmp_events;

//Идея:
//Создаем вертикальную прямую на минус бесконечности,
//которая будет двигаться вдоль х 
//(на самом деле можно выбрать произвольное направление), 
//и находить event -- момент добавления и удаления отрезка
//при добалвение проверяем отрезок с соседями,
//при удалении проверяем соседей
pair<int, int> findIntersectingSegments(const vector<Segment>& a) {
    int n = static_cast<int>(a.size());
    pair<int, int> is_itersect = std::make_pair(-1, -1);

    vector<Event> events;

    //добавляем концы отрезков по х -- наши события
    for (int i = 0; i < n; ++i) {
        events.push_back(Event(std::min(a[i].begin.x, a[i].end.x), a[i].number, true));
        events.push_back(Event(std::max(a[i].begin.x, a[i].end.x), a[i].number, false));
    }

    std::sort(events.begin(), events.end(), cmp_events);
    int n_e = static_cast<int>(events.size());

    //храним очередь из отрезков персекающиеся на данный момент с sweep line
    std::set<Segment> order_of_segments;

    //итераторы на отрезки для простого удалени и вставки
    vector<std::set<Segment>::iterator> number_of_seg;
    number_of_seg.resize(n);

    //пробегаемся по всем events, и либо втсавляем, либо удаляем с проверками
    for (int i = 0; i < n_e; ++i) {
        //добавление отрезка
        if (events[i].is_begin) {
            std::set<Segment>::iterator next;
            std::set<Segment>::iterator temp;
            std::set<Segment>::iterator prev;

            next = order_of_segments.lower_bound(a[events[i].numner_of_segment]);
            temp = next;
            if (temp == order_of_segments.begin()) {
                prev = order_of_segments.end();
            } else {
                prev = --temp;
            }

            //нашли пересечение
            if (next != order_of_segments.end() && intersect(*next, a[events[i].numner_of_segment])) {
                is_itersect = std::make_pair(next->number, events[i].numner_of_segment);
                break;
            }
            if (prev != order_of_segments.end() && intersect(*prev, a[events[i].numner_of_segment])) {
                is_itersect = std::make_pair(prev->number, events[i].numner_of_segment);
                break;
            }

            number_of_seg[events[i].numner_of_segment] = order_of_segments.insert(next, a[events[i].numner_of_segment]);
        } else {//удаление
            std::set<Segment>::iterator next;
            std::set<Segment>::iterator temp;
            std::set<Segment>::iterator prev;

            temp = number_of_seg[events[i].numner_of_segment];
            next = ++temp;
            temp = number_of_seg[events[i].numner_of_segment];
            if (temp == order_of_segments.begin()) {
                prev = order_of_segments.end();
            }
            else {
                prev = --temp;
            }

            if (next != order_of_segments.end() && prev != order_of_segments.end() && intersect(*next, *prev)) {
                is_itersect = std::make_pair(prev->number, next->number);
                break;
            }

            order_of_segments.erase(number_of_seg[events[i].numner_of_segment]);
        }
    }

    return is_itersect;
}

//смотрим на проекции по х и у
bool intersectOfProjection(double l1, double r1, double l2, double r2) {
    if (l1 > r1) {
        std::swap(l1, r1);
    }
    if (l2 > r2) {
        std::swap(l2, r2);
    }
    return std::max(l1, l2) <= std::min(r1, r2);
}

//возвращаем знак произведения
int SignOfCrossProduct(const Point& a, const Point& b, const Point& c) {
    double temp = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return std::abs(temp) < EPS ? 0 : temp > 0 ? 1 : -1;
}

bool intersect(const Segment& a, const Segment& b) {
    //вначале проверяем на персечение по х
    bool first_step = intersectOfProjection(a.begin.x, a.end.x, b.begin.x, b.end.x);

    //далее по у
    bool sec_step = intersectOfProjection(a.begin.y, a.end.y, b.begin.y, b.end.y);

    //теперь фиксируем один отрезок а с концами другого ищем век произведение
    bool th_step = SignOfCrossProduct(a.begin, a.end, b.begin) * SignOfCrossProduct(a.begin, a.end, b.end) <= 0;

    //повторяем с другим отрезком
    bool four_step = SignOfCrossProduct(b.begin, b.end, a.begin) * SignOfCrossProduct(b.begin, b.end, a.end) <= 0;

    return first_step && sec_step && th_step && four_step;
}

bool operator<(const Segment& a, const Segment& b) {
    double x = std::max(std::min(a.begin.x, a.end.x), std::min(b.begin.x, b.end.x));
    return a.get_y(x) < b.get_y(x);
}


int main() {
    int n;
    std::cin >> n;

    vector<Segment> a;

    for (int i = 0; i < n; ++i) {
        Segment temp;

        std::cin >> temp;
        temp.number = i;

        a.push_back(temp);
    }

    pair<int, int> is_intersect = findIntersectingSegments(a);

    if (is_intersect.first == -1 && is_intersect.second == -1) {
        std::cout << "NO" << "\n";
    }
    else {
        std::cout << "YES" << "\n" << is_intersect.first + 1 << " " << is_intersect.second + 1;
    }

    return 0;
}