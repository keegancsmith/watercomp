/**
 * This file tests the devillers & gandoin compression method as it was
 * intended, on a list of points.
 *
 * The format for the input file is a binary list of 32-bit unsigned
 * integers. The first integer is the number points in the file. Then each
 * point is a struct point_t
 */


#include "devillersgandoin.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <ctime>

using namespace std;

#define BITS 31


struct point_t_cmp {
    bool operator()(const point_t & a, const point_t & b) const {
        for (int i = 0; i < DIMENSIONS; i++) {
            if (a.coords[i] < b.coords[i])
                return true;
            else if (a.coords[i] > b.coords[i])
                return false;
        }
        return false;
    }
};


vector<point_t> read_uncompressed(istream & in) {
    unsigned int N;
    vector<point_t> points;

    in.read((char*)&N, sizeof(unsigned int));
    for (unsigned int i = 0; i < N; i++) {
        point_t p;
        in.read((char*)&p, sizeof(point_t));
        points.push_back(p);
    }

    return points;
}


vector<coord_t> read_compressed(istream & in) {
    unsigned int N;
    vector<coord_t> data;

    in.read((char*)&N, sizeof(unsigned int));
    for (unsigned int i = 0; i < N; i++) {
        coord_t c;
        in.read((char*)&c, sizeof(coord_t));
        data.push_back(c);
    }

    return data;
}


void write_uncompressed(ostream & out, const vector<point_t> & points) {
    unsigned int N = points.size();

    out.write((char*)&N, sizeof(unsigned int));
    for (unsigned int i = 0; i < N; i++)
        out.write((char*)&(points[i]), sizeof(point_t));
}


void write_compressed(ostream & out, const vector<coord_t> & data) {
    unsigned int N = data.size();

    out.write((char*)&N, sizeof(unsigned int));
    for (unsigned int i = 0; i < N; i++)
        out.write((char*)&(data[i]), sizeof(coord_t));
}


bool test_if_same(const char * fname1, const char * fname2) {
    ifstream fin1(fname1, ios::binary);
    ifstream fin2(fname2, ios::binary);

    vector<point_t> p1 = read_uncompressed(fin1);
    vector<point_t> p2 = read_uncompressed(fin2);

    if (p1.size() != p2.size())
        return false;

    point_t_cmp cmp;
    sort(p1.begin(), p1.end(), cmp);
    sort(p2.begin(), p2.end(), cmp);

    for (size_t i = 0; i < p1.size(); i++)
        if (cmp(p1[i], p2[i]) || cmp(p2[i], p1[i]))
            return false;

    return true;
}


vector<point_t> generate(int N) {
    vector<point_t> points;

    srand(time(0));
    coord_t mod = 1 << BITS;
    for (int i = 0; i < N; i++) {
        point_t p;
        for (int j = 0; j < DIMENSIONS; j++)
            p.coords[j] = ((coord_t)rand()) % mod;
        points.push_back(p);
    }

    return points;
}


int usage() {
    cerr << "./test_point_compression (enc|dec) in out\n";
    cerr << "./test_point_compression same in1 in2\n";
    cerr << "./test_point_compression gen N out\n";
    return 1;
}


int main(int argc, char **argv) {
    if (argc < 4)
        return usage();

    string cmd = argv[1];

    if (cmd == "enc") {
        ifstream fin(argv[2], ios::binary);
        ofstream fout(argv[3], ios::binary);

        vector<point_t> points = read_uncompressed(fin);
        write_compressed(fout, encode(points, BITS));
    } else if (cmd == "dec") {
        ifstream fin(argv[2], ios::binary);
        ofstream fout(argv[3], ios::binary);

        vector<coord_t> data = read_compressed(fin);
        write_uncompressed(fout, decode(data, BITS));
    } else if (cmd == "same") {
        bool same = test_if_same(argv[2], argv[3]);

        cout << (same ? "same" : "different") << endl;
    } else if (cmd == "gen") {
        int N = atoi(argv[2]);
        ofstream fout(argv[3], ios::binary);

        vector<point_t> points = generate(N);
        write_uncompressed(fout, points);
    } else {
        return usage();
    }
}
