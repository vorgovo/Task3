#include "big_integer.cpp"
#include <bits/stdc++.h>
using namespace std;

namespace
{
    unsigned const number_of_iterations = 1;
    size_t const number_of_multipliers = 1000;

    int myrand()
    {
        int val = rand() - RAND_MAX / 2;
        if (val != 0)
            return val;
        else
            return 1;
    }
}

namespace
{
    template <typename T>
    void erase_unordered(std::vector<T>& v, typename std::vector<T>::iterator pos)
    {
        std::swap(v.back(), *pos);
        v.pop_back();
    }

    template <typename T>
    T extract_random_element(std::vector<T>& v)
    {
        size_t index = rand() % v.size();
        T copy = v[index];
        erase_unordered(v, v.begin() + index);
        return copy;
    }

    template <typename T>
    void merge_two(std::vector<T>& v)
    {
        assert(v.size() >= 2);
        bool ok = false;
        if (sz(v) == 1) ok = true;
        T a = extract_random_element(v);
        T b = extract_random_element(v);
        T ab = a * b;
        assert(ab / a == b);
        assert(ab / b == a);
        //if (ok) exit(0);
        v.push_back(ab);
    }

    template <typename T>
    T merge_all(std::vector<T> v)
    {
        assert(!v.empty());

        while (v.size() >= 2) {
            merge_two(v);
            //if (v.size() == 964) exit(0);
        }

        return v[0];
    }
}


int main() {
    freopen("o", "w", stdout);
    for (unsigned itn = 0; itn != number_of_iterations; ++itn)
    {
        std::vector<big_integer> x;
        for (size_t i = 0; i != number_of_multipliers; ++i)
            x.push_back(myrand());

        big_integer a = merge_all(x);
        big_integer b = merge_all(x);
        cout << itn << "\n";
        cout << (a == b) << "\n";
    }
}