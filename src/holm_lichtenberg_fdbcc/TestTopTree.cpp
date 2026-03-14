#include <string.h>
#include <time.h>
#include <cmath>
#include <utility>
#include <cstdio>
#include <algorithm>
#include <set>
#include <vector>
#include <queue>
#include "my_types.h"
#include "top_tree.h"
#include "tree.h"
#include <iostream>
using namespace std;

int main() {
    TopTree tp;
    tp.create_toptree(20);
    cout << "finish init" << endl;
    VArrP path;


    tp.link(1, 2);
    tp.link(2, 3);
    tp.link(3, 4);

    tp.link(5, 6);
    tp.link(4, 5);

    cout << "test1" << endl;
    path = tp.find_path(1, 6);
    for (int k = 0; k < path->size(); k++) {
        cout << path->at(k) << " ";
    }
    cout << endl;
    cout << "test1 end" << endl;
    cout << "test2" << endl;
    path->clear();
    tp.cut(2, 8);
    tp.cut(1, 2);
    tp.link(6, 8);
    path = tp.find_path(4, 8);
    for (int k = 0; k < path->size(); k++) {
        cout << path->at(k) << " ";
    }
    cout << endl;
    cout << "test2 end" << endl;
    cout << "test3" << endl;
    if (tp.is_edge(2, 8))
        cout << 1 << endl;
    else
        cout << 0 << endl;
    if (tp.is_edge(5, 6))
        cout << 1 << endl;
    else
        cout << 0 << endl;
    cout << "test3 end" << endl;
    cout << "test4" << endl;
    vector<Edge> ve;
    tp.get_all_edges(ve);
    for (int i = 0; i < ve.size(); i++) {
        cout << ve[i].first << " " << ve[i].second << endl;
    }
    cout << "test4 end" << endl;

    return 0;
}