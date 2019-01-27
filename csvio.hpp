//
// Copyright (c) 2017-2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PROJECT_CSVIO_HPP
#define PROJECT_CSVIO_HPP
//std::vector<double> read_vector(std::istream& is) {
//    std::vector<float> result;
//    for (; ;) {
//        float number;
//        if (!is >> number) break;
//        result.push_back(number);
//    }
//    if (!is.eof())
//        std::cerr << "failed to reach and end of the file";
//
//    return result;
//}

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

namespace ssan {
namespace csvio {
template<typename T>
std::istream &operator>>(std::istream &is, std::vector<T> &v) {
    v.resize(0);
    for (;;) {
        T number;
        if (!(is >> number)) break;
        v.push_back(number);
    }
    if (!is.eof())
        std::runtime_error("Failed to read vector from stream");

    return is;
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
    out << std::fixed;
    out << std::setprecision(18);
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i] << std::endl;
    }
    return out;
}

template<typename T>
std::istream &operator>>(std::istream &is, std::vector<std::vector<T>> &m) {
    std::string line;

    m.resize(0);

    while (std::getline(is, line)) {
        std::stringstream iss(line);
        std::string string_value;
        T val;
        std::vector<T> raw;
        while (std::getline(iss, string_value, ',')) {
            std::stringstream converter(string_value);
            converter >> val;
            raw.push_back(val);
        }
        if (!iss.eof())
            std::runtime_error("Failed to parse vector from the ");

        m.push_back(raw);
    }

    if (!is.eof())
        std::runtime_error("Failed to read vector from stream");

    return is;
}

template<typename T>
ostream &operator<<(ostream &out, const vector<vector<T>> &m) {
    for (auto &row : m) {
        for (auto col : row)
            out << col << ',';
        out << '\n';
    }

    return out;
}

#if 0
template<typename T>
ostream &operator<<(std::ostream &out, vector<vector<vector<vector<T>>>> &t4d) {
    //the 4-dim tensor we are dumping as an unfolded vector
    for (auto &t3d : t4d) {
        for (auto &t2d: t3d) {
            for (auto &t1d: t2d) {
                for (auto val : t1d) {
                    out << val << endl;
                }
            }
        }
    }
}
#else

template<typename T>
ostream &operator<<(std::ostream &out, vector<vector<vector<vector<T>>>> &t4d) {
    //the 4-dim tensor we are dumping as an unfolded vector
    int dim1 = t4d.size();
    int dim2 = t4d[0].size();
    int dim3 = t4d[0][0].size();
    int dim4 = t4d[0][0][0].size();
    for (int i1 = 0; i1 < dim4; i1++) {
        for (int i2 = 0; i2 < dim3; i2++) {
            for (int i3 = 0; i3 < dim2; i3++) {
                for (int i4 = 0; i4 < dim1; i4++) {
                    out << t4d[i4][i3][i2][i1] << endl;
                }
            }
        }
    }

    return out;
}

}
}
#endif


#endif //PROJECT_CSVIO_HPP
