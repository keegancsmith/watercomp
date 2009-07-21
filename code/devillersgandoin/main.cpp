#include "devillersgandoin.h"
#include <cassert>
#include <iostream>

using namespace std;

int raw_points[7][2] =  {{0, 0}, {1, 1}, {0, 2}, {1, 2}, {2, 2}, {1, 3}, {2, 3}};
unsigned int raw_data[12] = {7, 5, 2, 0, 1, 1, 2, 1, 0, 1, 1, 1};

int main() {
    vector<point_t> points;
    for (int i = 0; i < 7; i++) {
        point_t p;
        p.coords[0] = raw_points[i][0];
        p.coords[1] = raw_points[i][1];
        points.push_back(p);
    }

    vector<coord_t> data = encode(points, 2);
    
    for (int i = 0; i < 12; i++)
        cout << raw_data[i] << ' ';
    cout << endl;

    for (size_t i = 0; i < data.size(); i++)
        cout << data[i] << ' ';
    cout << endl;

    assert(data.size() == 12);
    for (int i = 0; i < 12; i++)
        assert(data[i] == raw_data[i]);

    return 0;
}
