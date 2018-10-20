#include <cstring>
#include <cstdlib>
#include <iostream>
#include "my_vector.h"

const bool my_vector::is_small() const {
    return my_size <= SMALL_SIZE;
}

unsigned int &my_vector::ref_cnt() {
    return (*_data.big_data.start);
}

void my_vector::my_delete() {
    if (!is_small()) {
        ref_cnt()--;
        if (ref_cnt() == 0) {
            delete[] _data.big_data.start;
        }
    }
}

my_vector::~my_vector() {
    my_delete();
};

void my_vector::copy_small(const_iterator from, const_iterator to) {
    std::fill(_data.small_data.start, _data.small_data.start + SMALL_SIZE, 0);
    std::copy(from, to, _data.small_data.start);
    my_size = to - from;
}

void my_vector::copy_data(const_iterator from, size_t capacity, size_t _size) {
    auto v = new unsigned int[capacity + 1];
    std::copy(from, from + _size, v + 1);
    std::fill(v + 1 + _size, v + capacity + 1, 0);
    _data.big_data.start = v;
    ref_cnt() = 1;
    _data.big_data.capacity = capacity;
    my_size = _size;
}

void my_vector::copy_big_data(const_iterator from, size_t capacity, size_t _size) {
    ref_cnt()--;
    auto cnt = ref_cnt();
    auto for_del = _data.big_data.start;
    copy_data(from + 1, capacity, _size);
    if (cnt == 0) {
        delete[] for_del;
    }
}

void my_vector::my_copy(my_vector const &other) {
    my_size = other.size();
    if (other.is_small()) {
        copy_small(other.begin(), other.end());
    } else {
        _data.big_data = other._data.big_data;
        ref_cnt()++;
    }
}

my_vector::my_vector() {
    my_size = 0;
    any_data();
};

my_vector::my_vector(my_vector const &other) {
    my_copy(other);
};

my_vector &my_vector::operator=(my_vector const &other) {
    if (this != (&other)) {
        my_delete();
        my_copy(other);
    }
    return (*this);
};

my_vector::const_iterator my_vector::begin() const {
    if (is_small()) {
        return _data.small_data.start;
    } else {
        return _data.big_data.start + 1;
    }
};

my_vector::iterator my_vector::begin() {
    if (is_small()) {
        return _data.small_data.start;
    } else {
        if (ref_cnt() != 1) {
            copy_big_data(_data.big_data.start, _data.big_data.capacity, my_size);
        }
        return _data.big_data.start + 1;
    }
};

my_vector::const_iterator my_vector::end() const {
    return begin() + my_size;
};

my_vector::iterator my_vector::end() {
    return begin() + my_size;
};

unsigned int &my_vector::operator[](unsigned int i) {
    return begin()[i];
};

const unsigned int &my_vector::operator[](unsigned int i) const {
    return begin()[i];
};

void my_vector::push_back(unsigned int a) {
    size_t i = my_size;
    resize(my_size + 1);
    begin()[i] = a;
};

void my_vector::pop_back() {
    resize(my_size - 1);
};

size_t my_vector::size() const {
    return my_size;
};

void my_vector::resize(size_t new_size) {
    if (new_size == my_size) { return; }
    if (new_size <= SMALL_SIZE) {
        if (!is_small()) {
            unsigned int &cnt = ref_cnt();
            cnt--;
            const_iterator prev_start = _data.big_data.start;
            copy_small(_data.big_data.start + 1, _data.big_data.start + new_size + 1);
            if (cnt == 0) {
                delete[] prev_start;
            }
        } else {
            my_size = new_size;
        }
        return;
    }
    size_t capacity = 1;
    while (capacity < new_size) { capacity *= 2; }
    if (is_small()) {
        copy_data(_data.small_data.start, capacity, my_size);
    } else if (ref_cnt() != 1 || capacity != _data.big_data.capacity) {
        copy_big_data(_data.big_data.start, capacity, std::min(new_size, my_size));
    }
    my_size = new_size;
};

unsigned int &my_vector::back() {
    return *(end() - 1);
}

bool my_vector::empty() {
    return size() == 0;
};

