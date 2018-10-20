#include "my_vector.cpp"
#include <bits/stdc++.h>
using namespace std;

int main() {
    my_vector v;
    for (int i = 0; i < 10; i++) {
        v.push_back(i);
        for (int j = 0; j < v.size(); j++) printf("%d ", v[j]);
        printf("\n");
    }
    for (int i = 0; i < 10; i++) {
        v.pop_back();
        for (int j = 0; j < v.size(); j++) printf("%d ", v[j]);
        printf("\n");
    }

}