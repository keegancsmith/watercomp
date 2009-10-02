#include "TreeSerialiser.h"

#include "../arithmetic/AdaptiveModelEncoder.h"
#include "../quantiser/QuantisedFrame.h"

#include <cassert>
#include <cstdio>
#include <queue>

using namespace std;

void serialise_tree(ArithmeticEncoder & ae, Graph * g, int root) {
    QuantisedFrame * frame = (QuantisedFrame *)g->data;

    AdaptiveModelEncoder tree_encoder(&ae);
    AdaptiveModelEncoder errx_encoder(&ae);
    AdaptiveModelEncoder erry_encoder(&ae);
    AdaptiveModelEncoder errz_encoder(&ae);
    AdaptiveModelEncoder index_encoder(&ae);

    unsigned int predictions[g->nVerticies][3];
    fill(predictions[root], predictions[root] + 3, 0);

    string tree_symbols[3] = {"0", "1", "2"};
    char buf[100];

    queue<int> q;
    q.push(root);

    while(!q.empty()) {
	int v = q.front();
	q.pop();

	int index = v;
	int error[3];
	for (int i = 0; i < 3; i++)
	    error[i] = frame->quantised_frame[3*v + i] - predictions[v][i];

	assert(g->adjacent[v].size() <= 2);
	tree_encoder.encode(tree_symbols[g->adjacent[v].size()]);

	sprintf(buf, "%d", error[0]);
	errx_encoder.encode(buf);

	sprintf(buf, "%d", error[1]);
	erry_encoder.encode(buf);

	sprintf(buf, "%d", error[2]);
	errz_encoder.encode(buf);

	sprintf(buf, "%d", index);
	index_encoder.encode(buf);

	for (size_t i = 0; i < g->adjacent[v].size(); i++) {
	    int u = g->adjacent[v][i];
	    for (int j = 0; j < 3; j++)
		predictions[u][j] = frame->quantised_frame[3*v + i];
	    q.push(u);
	}
    }
}
