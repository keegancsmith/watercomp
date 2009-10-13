#include "pdbio/DCDReader.h"
#include "pdbio/Frame.h"
#include "quantiser/QuantisedFrame.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
    assert(argc > 2);
    char * original     = argv[1];
    char * decompressed = argv[2];
    int quantx = argc > 3 ? atoi(argv[3]) : 8;
    int quanty = argc > 4 ? atoi(argv[4]) : 8;
    int quantz = argc > 5 ? atoi(argv[5]) : 8;
    bool quiet = argc > 6;
    bool all_same = true;

    DCDReader reader1, reader2;
    reader1.open_file(original);
    reader2.open_file(decompressed);

    assert(reader1.nframes() == reader2.nframes());
    assert(reader1.natoms() == reader2.natoms());

    for (size_t frame = 1; frame <= reader1.nframes(); frame++) {
        cout << "Frame " << frame << endl;

        Frame f1(reader1.natoms());
        Frame f2(reader1.natoms());
        reader1.next_frame(f1);
        reader2.next_frame(f2);

        QuantisedFrame q1(f1, quantx, quanty, quantz);
        QuantisedFrame q2(f2, quantx, quanty, quantz);

        bool same = true;
        for (size_t i = 0; i < q1.quantised_frame.size() && same; i++)
            same = q1.quantised_frame[i] == q2.quantised_frame[i];

        float avg_error[3] = {0, 0, 0};
        float max_error[3] = {0, 0, 0};
        float min_error[3] = {9e10, 9e10, 9e10};
        for (size_t i = 0; i < f1.natoms(); i++) {
            for (int d = 0; d < 3; d++) {
                float v1 = f1.atom_data[3*i + d];
                float v2 = f2.atom_data[3*i + d];
                float e  = fabs(v1 - v2);
                avg_error[d] += e;
                max_error[d] = max(max_error[d], e);
                min_error[d] = min(min_error[d], e);
            }
        }

        // avg_error contains the sum, so we need to divide by natoms to get
        // the average
        for (int d = 0; d < 3; d++)
            avg_error[d] /= f1.natoms();

        if (!same)
            all_same = false;

        if (quiet)
            continue;

        if (same)
            cout << "SUCCESS: Frames are the same when quantised" << endl;
        else
            cout << "FAIL: Frames are different when quantised" << endl;
        cout << "average error: " << avg_error[0] << ' ' << avg_error[1]
             << ' ' << avg_error[2] << endl
             << "min error:     " << min_error[0] << ' ' << min_error[1]
             << ' ' << min_error[2] << endl
             << "max error:     " << max_error[0] << ' ' << max_error[1]
             << ' ' << max_error[2] << endl << endl;
    }

    if (quiet)
        return all_same ? 0 : 1;

    if (all_same)
        cout << "SUCCESS: Files are the same when quantised" << endl;
    else
        cout << "FAIL: Files are different when quantised" << endl;
    
    return 0;
}
