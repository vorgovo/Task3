//
// Created by demon1999 on 29.05.18.
//

#include <limits>
#include <cstddef>
#include <iosfwd>

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

struct my_vector {
    typedef unsigned int *iterator;
    typedef const unsigned int *const_iterator;

    my_vector();

    my_vector(my_vector const &other);

    ~my_vector();

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    unsigned int &back();

    void copy_small(const_iterator from, const_iterator to);

    void copy_big_data(const_iterator from, size_t capacity, size_t _size);

    void copy_data(const_iterator from, size_t capacity, size_t _size);

    void push_back(unsigned int a);

    void pop_back();

    unsigned int &ref_cnt();

    size_t size() const;

    void resize(size_t new_size);

    unsigned int &operator[](unsigned int i);

    const unsigned int &operator[](unsigned int i) const;

    bool empty();

    const bool is_small() const;

    void my_copy(my_vector const &other);

    void my_delete();

    my_vector &operator=(my_vector const &other);

private:
    static const int SMALL_SIZE = 4;
    size_t my_size;
    struct big {
        size_t capacity;
        unsigned int *start;
    };

    struct small {
        unsigned int start[SMALL_SIZE];

        small() = default;
    };

    union any_data {
        big big_data;
        small small_data;
    } _data;
};

#endif //BIGINT_MY_VECTOR_H