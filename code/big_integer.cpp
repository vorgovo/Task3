#include "big_integer.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iostream>

const int PARSE_SIZE = 9;
const unsigned long long POW32 = (1ULL << 32);
const unsigned int MAX_UI = std::numeric_limits<unsigned int>::max();

void big_integer::norm() {
    while (!data.empty() && ((sign == 0 && data.back() == 0) || (sign && data.back() == MAX_UI))) {
        data.pop_back();
    }
    if (sign) { data.push_back(MAX_UI); }
    else {
        data.push_back(0);
    }
}

big_integer::big_integer() {
    sign = false;
}

big_integer::big_integer(int a) {
    unsigned int c;
    if (a < 0) {
        sign = true;
        c = (a & MAX_UI);
    } else {
        sign = false;
        c = static_cast<unsigned int>(a);
    }
    data.push_back(c);
}

big_integer::big_integer(std::string const &str) {
    big_integer res(0);
    size_t ps = 0;
    sign = false;
    if (str[0] == '-') {
        ps = 1;
    }
    for (size_t i = ps; i < str.size(); i += PARSE_SIZE) {
        int ans = 0;
        int k = 1;
        for (size_t j = i; j < std::min(str.size(), i + PARSE_SIZE); j++) {
            if (str[j] < '0' || str[j] > '9') {
                throw std::runtime_error("invalid string");
            }
            ans = 10 * ans + str[j] - '0';
            k *= 10;
        }
        res = res * k + ans;
    }
    if (ps) {
        res = -res;
    }
    *this = res;
}

big_integer::~big_integer() = default;

big_integer &big_integer::operator=(int c) {
    *this = big_integer(c);
    return *this;
}

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
    size_t c = std::max(data.size(), rhs.data.size()) + 1;
    std::vector<long long> sm(c, 0);
    std::copy(rhs.data.begin(), rhs.data.end(), sm.begin());
    if (rhs.sign) {
        std::fill(sm.begin() + rhs.data.size(), sm.end(), MAX_UI);
    }
    std::transform(sm.begin(), sm.begin() + data.size(), data.begin(), sm.begin(), std::plus<long long>());
    if (sign) {
        for (size_t i = data.size(); i < c; i++) { sm[i] += MAX_UI; }
    }
    for (size_t i = 0; i < c; i++) {
        if (sm[i] > MAX_UI) {
            if (i + 1 == sm.size()) {
                sm[i] &= MAX_UI;
                continue;
            }
            sm[i + 1] += (sm[i] >> 32);
            sm[i] &= MAX_UI;
        }
    }
    data.resize(sm.size());
    std::copy(sm.begin(), sm.end(), data.begin());
    sign = (data.back() & (1LL << 31)) != 0;
    norm();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    auto v = -rhs;
    *this += v;
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer a = rhs, b = *this;
    if (a.sign) { a = -a; }
    if (b.sign) { b = -b; }
    sign ^= rhs.sign;
    std::vector<unsigned long long> ans(a.data.size() + b.data.size() + 1, 0);
    for (size_t i = 0; i < a.data.size(); i++) {
        for (size_t j = 0; j < b.data.size(); j++) {
            ans[i + j] += (unsigned long long) b.data[j] * a.data[i];
        }
        for (size_t jj = i; jj < i + b.data.size() + 1; jj++) {
            auto c = static_cast<unsigned int>(ans[jj]);
            if (c != ans[jj]) {
                if (jj + 1 == ans.size()) { ans.push_back(0); }
                ans[jj + 1] += (ans[jj] >> 32);
                ans[jj] &= MAX_UI;
            }
        }
    }
    ans.push_back(0);
    data.resize(ans.size());
    std::copy(ans.begin(), ans.end(), data.begin());
    if (sign) {
        sign = false;
        (*this) = -(*this);
    }
    norm();
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    big_integer a = *this, b = rhs;
    sign = a.sign ^ b.sign;
    if (a.sign) {
        a = -a;
    }
    if (b.sign) {
        b = -b;
    }
    while (!b.data.empty() && b.data.back() == 0) { b.data.pop_back(); }
    while (!a.data.empty() && a.data.back() == 0) { a.data.pop_back(); }
    my_vector ans;
    int pr_pos = -1;
    while (a >= b) {
        unsigned long long l;
        big_integer q;
        q.sign = false;
        q.data.resize(0);
        size_t sc = b.data.size();
        for (size_t i = 0; i < b.data.size(); i++) {
            q.data.push_back(a.data[a.data.size() - b.data.size() + i]);
        }
        while (q < b) {
            sc++;
            q.sign = false;
            q.data.resize(0);
            for (size_t i = 0; i < sc; i++) {
                q.data.push_back(a.data[a.data.size() - sc + i]);
            }
        }
        __int128_t c = 0, k = b.data.back();
        if (b.data.size() >= 2) {
            k = 0;
            if (q.data.size() == b.data.size()) {
                for (size_t i = 0; i < 2; i++) {
                    c = c * POW32 + q.data[q.data.size() - i - 1];
                    k = k * POW32 + b.data[b.data.size() - i - 1];
                }
            } else {
                for (size_t i = 0; i < 2; i++) {
                    k = k * POW32 + b.data[b.data.size() - i - 1];
                }
                for (size_t i = 0; i < 3; i++) {
                    c = c * POW32 + q.data[q.data.size() - i - 1];
                }
            }
        } else {
            for (int i = int(q.data.size()) - 1; i >= 0; i--) {
                c = c * POW32 + q.data[i];
            }
        }
        l = static_cast<unsigned long long int>(c / k);
        big_integer ct;
        ct.sign = false;
        ct.data.resize(0);
        ct.data.push_back(static_cast<unsigned int &&>(l));
        ct.data.push_back(0);
        ct *= b;
        if (ct > q) {
            ct -= b;
            l--;
        }
        q -= ct;
        for (size_t i = q.data.size(); i < sc; i++) {
            a.data[a.data.size() - sc + i] = 0;
        }
        for (size_t i = 0; i < std::min(sc, q.data.size()); i++) {
            a.data[a.data.size() - sc + i] = q.data[i];
        }
        while (pr_pos - 1 > int(a.data.size()) - sc && pr_pos != -1) {
            ans.push_back(0);
            pr_pos--;
        }
        pr_pos = static_cast<int>(a.data.size() - sc);

        ans.push_back(static_cast<unsigned int &&>(l));

        for (size_t i = 0; i < sc; i++) {
            if (a.data[a.data.size() - 1]) { break; }
            a.data.pop_back();
        }
    }
    while (pr_pos > 0) {
        pr_pos--;
        ans.push_back(0);
    }
    std::reverse(ans.begin(), ans.end());
    ans.push_back(0);
    data = ans;
    if (sign) {
        sign = false;
        *this = -(*this);
    }
    norm();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    big_integer a = *this, b;
    b = rhs;
    big_integer c = (a / b) * b;
    *this = a - c;
    norm();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    unsigned int k = MAX_UI * sign;
    while (data.size() < rhs.data.size()) {
        data.push_back(k);
    }
    for (size_t i = 0; i < rhs.data.size(); i++) {
        data[i] &= rhs.data[i];
    }
    if (!rhs.sign && rhs.data.size() < data.size()) {
        std::fill(data.begin() + rhs.data.size(), data.end(), 0);
    }
    sign &= rhs.sign;
    norm();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    unsigned int k = MAX_UI * sign;
    while (data.size() < rhs.data.size()) {
        data.push_back(k);
    }
    for (size_t i = 0; i < rhs.data.size(); i++) {
        data[i] |= rhs.data[i];
    }
    if (rhs.sign && rhs.data.size() < data.size()) {
        std::fill(data.begin() + rhs.data.size(), data.end(), MAX_UI);
    }
    sign |= rhs.sign;
    norm();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    unsigned int k = MAX_UI * sign;
    while (data.size() < rhs.data.size()) {
        data.push_back(k);
    }
    for (size_t i = 0; i < rhs.data.size(); i++) {
        data[i] ^= rhs.data[i];
    }
    if (rhs.sign) {
        for (size_t i = rhs.data.size(); i < data.size(); i++) {
            data[i] ^= MAX_UI;
        }
    }
    sign ^= rhs.sign;
    norm();
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (data.empty()) { return *this; }
    my_vector ans;
    ans.resize(static_cast<size_t>(rhs / 32));
    rhs %= 32;
    for (size_t i = 0; i < data.size(); i++) {
        ans.push_back(data[i] << rhs);
        if (i && rhs) {
            ans[ans.size() - 1] += ((data[i - 1]) >> (32 - rhs));
        }
    }
    data = ans;
    norm();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (data.empty()) { return *this; }
    my_vector ans;
    int k = rhs / 32;
    rhs %= 32;
    for (int i = k; i < data.size(); i++) {
        ans.push_back(data[i] >> rhs);
        if (i + 1 < data.size() && rhs) {
            ans[ans.size() - 1] |= (data[i + 1] & (((unsigned int) 1 << rhs) - 1)) << (32 - rhs);
        } else if (rhs && sign) {
            ans[ans.size() - 1] |= (((1 << rhs) - 1)) << (32 - rhs);
        }
    }
    data = ans;
    if (data.empty()) { sign = false; }
    norm();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r;
    std::vector<long long> sm(data.size(), 0);
    std::transform(data.begin(), data.end(), sm.begin(), std::bit_not<unsigned int>());
    if (!sign) {
        sm.push_back(MAX_UI);
    } else {
        sm.push_back(0);
    }
    sm[0]++;
    for (size_t i = 0; i < data.size() + 1; i++) {
        if (sm[i] > MAX_UI) {
            if (i + 1 == sm.size()) {
                sm[i] &= MAX_UI;
                continue;
            }
            sm[i + 1] += (sm[i] >> 32);
            sm[i] &= MAX_UI;
        }
    }
    r.data.resize(sm.size());
    std::copy(sm.begin(), sm.end(), r.data.begin());
    r.sign = (r.data.back() & (1LL << 31)) != 0;
    r.norm();
    return r;
}

big_integer big_integer::operator~() const {
    big_integer c;
    c.data.resize(data.size());
    std::transform(data.begin(), data.end(), c.data.begin(), std::bit_not<unsigned int>());
    c.sign = !sign;
    if (c.data.empty()) { c.sign = false; }
    c.norm();
    return c;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    a += b;
    return a;
}

big_integer operator-(big_integer a, big_integer const &b) {
    a -= b;
    return a;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    for (size_t i = 0; i < std::max(a.data.size(), b.data.size()); i++) {
        unsigned int c = 0, d = 0;
        if (i < a.data.size()) {
            c = a.data[i];
        } else if (a.sign) { c = MAX_UI; }
        if (i < b.data.size()) {
            d = b.data[i];
        } else if (b.sign) { d = MAX_UI; }
        if (c != d) { return false; }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign > b.sign) { return true; }
    if (a.sign < b.sign) { return false; }
    for (int i = static_cast<int>(std::max(b.data.size(), a.data.size()) - 1); i >= 0; i--) {
        unsigned int c = 0, d = 0;
        if (i < a.data.size()) {
            c = a.data[i];
        } else if (a.sign) { c = MAX_UI; }
        if (i < b.data.size()) {
            d = b.data[i];
        } else if (b.sign) { d = MAX_UI; }
        if (c < d) { return true; }
        if (c > d) { return false; }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return (b < a);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer c = a;
    if (a.sign) {
        c = -c;
    }
    c.sign = false;
    while (c > 0) {
        res += char('0' + (c % 10).data[0]);
        c /= 10;
    }
    if (res.empty()) { res += "0"; }
    if (a.sign) { res += '-'; }
    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
