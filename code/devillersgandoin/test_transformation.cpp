// Tests transformation using 2d example from paper

#include "devillersgandoin.h"
#include <cassert>
#include <iostream>

using namespace std;

unsigned int raw_points[7][2] =  {{0, 0}, {1, 1}, {0, 2}, {1, 2},
                                  {2, 2}, {1, 3}, {2, 3}};
unsigned int raw_data[12] = {7, 5, 2, 0, 1, 1, 2, 1, 0, 1, 1, 1};

bool has_point(const point_t & x) {
    for (int i = 0; i < 7; i++)
        if (raw_points[i][0] == x.coords[0] &&
            raw_points[i][1] == x.coords[1])
            return true;
    return false;
}

int main() {
    vector<point_t> points;
    for (int i = 0; i < 7; i++) {
        point_t p;
        p.coords[0] = raw_points[i][0];
        p.coords[1] = raw_points[i][1];
        points.push_back(p);
    }

    vector<int> perm;
    vector<coord_t> data = encode(points, perm, 2);
    vector<point_t> decoded = decode(data, 2);

    for (int i = 0; i < 12; i++)
        cout << raw_data[i] << ' ';
    cout << endl;

    for (size_t i = 0; i < data.size(); i++)
        cout << data[i] << ' ';
    cout << endl;

    assert(data.size() == 12);
    for (int i = 0; i < 12; i++)
        assert(data[i] == raw_data[i]);

    assert(decoded.size() == 7);
    for (int i = 0; i < 7; i++)
        assert(has_point(decoded[i]));

    return 0;
}
