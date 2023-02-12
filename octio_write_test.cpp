//
// Copyright (c) 2022 Sergey Panov.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <complex>
#include <iostream>

#include <vector>
#include <iterator>
#include <numeric>
#include <fstream>
#include <cmath>
#include <limits>

#define OCTAVE_WRITE_ONLY

#include "octio.hpp"

using namespace ssan::octio;

int main() {
    int int_var = -1;

    int8_t int8_var{-1};
    uint8_t uint8_var{1};
    int16_t int16_var{-2};
    uint16_t uint16_var{2};
    int32_t int32_var{-4};
    uint32_t uint32_var{4};
    int64_t int64_var{-8};
    uint64_t uint64_var{8};

    float float_var(nanf(""));
    double double_var(nan(""));
    long double ldouble_var(nanl(""));

    std::complex<int> cint_var(-1, 1);
    std::complex<float> cfloat_var(nanf(""), nanf(""));
    std::complex<double> cdouble_var(nan(""), nan(""));
    std::complex<long double> cldouble_var(nanl(""), nanl(""));

    std::vector<int> int_vect{0, 1, -2, 3,-4};
    std::vector<unsigned> uint_vect{0, 1, 2, 3, 4};

    std::vector<float> float_vect{nanf(""), 1.1, -2.2, 3.3, -4.4};
    std::vector<double> double_vect{nan(""),1.1, -2.2, 3.3, -4.4};
    std::vector<long double> ldouble_vect{nan(""), 1.1, -2.2, 3.3, -4.4};

    std::vector<int8_t> int8_vect{0, 1, -2, 3,-4};
    std::vector<uint8_t> uint8_vect{0, 1, 2, 3, 4};
    std::vector<int16_t> int16_vect{0, 1, -2, 3, -4};
    std::vector<uint16_t> uint16_vect{0, 1, 2, 3, 4};
    std::vector<int32_t> int32_vect{0, 1, -2, 3, -4};
    std::vector<uint32_t> uint32_vect{0, 1, 2, 3, 4};
    std::vector<int64_t> int64_vect{0, 1, -2, 3, -4};
    std::vector<uint64_t> uint64_vect{0, 1, 2, 3, 4};

    std::vector<std::complex<int>> cint_vect{{0, -1}, {-1, 2}, {2, -3}, {-3 , 4} , {4, -5}};
    //std::vector<std::complex<unsigned>> cuint_vect{{0, 1}, {1, 2}, {2, 3}, {3 , 4} , {4, 5}};
    std::vector<std::complex<float>> cfloat_vect{{0.0, -1.1}, {-1.1, 2.2}, {2.2, -3.3}, {-3.3, 4.4} , {4.4, -5.5}};
    std::vector<std::complex<double>> cdouble_vect{{0.0, -1.1}, {-1.1, 2.2}, {2.2, -3.3}, {-3.3, 4.4} , {4.4, -5.5}};
    std::vector<std::complex<long double>> cldouble_vect{{0.0, -1.1}, {-1.1, 2.2}, {2.2, -3.3}, {-3.3, 4.4} , {4.4,-5.5}};

    std::vector<std::vector<int>> int_mat{{0, 1, -2},{3, -4, 5}};
    //std::vector<std::vector<unsigned>> uint_mat{{0, 1, 2},{3, 4, 5}};
    std::vector<std::vector<float>> float_mat{{nanf(""), 1.1, -2.2},{3.3, -4.4, 5.5}};
    std::vector<std::vector<double>> double_mat{{nanf(""), 1.1, -2.2},{3.3, -4.4, 5.5}};
    std::vector<std::vector<long double>> ldouble_mat{{nanf(""), 1.1, -2.2},{3.3, -4.4, 5.5}};

    std::vector<std::vector<std::complex<int>>> cint_mat{{{0, 1}, {1, -2}, {-2, 3}},{{3, -4}, {-4, 5}, {5, -6}}};
    //std::vector<std::vector<std::complex<unsigned>>> cuint_mat{{{0, 1}, {1, 2}, {2, 3}},{{3, 4}, {4, 5}, {5, 6}}};
    std::vector<std::vector<std::complex<float>>> cfloat_mat{{{nanf(""), 1.1}, {1.1, -2.2}, {-2.2, 3.3}},
                                                             {{3.3, -4.4}, {-4.4, 5.5}, {5.5, -6.6}}};
    std::vector<std::vector<std::complex<double>>> cdouble_mat{{{nan(""), 1.1}, {1.1, -2.2}, {-2.2, 3.3}},
                                                             {{3.3, -4.4}, {-4.4, 5.5}, {5.5, -6.6}}};
    std::vector<std::vector<std::complex<long double>>> cldouble_mat{{{nanl(""), 1.1}, {1.1, -2.2}, {-2.2, 3.3}},
                                                             {{3.3, -4.4}, {-4.4, 5.5}, {5.5, -6.6}}};

    const std::string file_name_out = "test_octio_write.mat";

    std::ofstream ofile(file_name_out);
    writer output(&ofile, "Test file");


    output.write(int_var, "int_var");

    output.write(int8_var, "int8_var");
    output.write(uint8_var, "unt8_var");
    output.write(int16_var, "int16_var");
    output.write(uint16_var, "unt16_var");
    output.write(int32_var, "int32_var");
    output.write(uint32_var, "unt32_var");
    output.write(int64_var, "int64_var");
    output.write(uint64_var, "unt64_var");

    output.write(float_var, "float_var_nan");
    float_var = -1.1;
    output.write(float_var, "float_var_1d1");
    float_var = std::numeric_limits<float>::min();
    output.write(float_var, "float_var_min");
    float_var = std::numeric_limits<float>::max();
    output.write(float_var, "float_var_max");

    output.write(double_var, "double_var_nan");
    double_var = -1.1;
    output.write(double_var, "double_var_1d1");
    double_var = std::numeric_limits<double>::min();
    output.write(double_var, "double_var_min");
    double_var = std::numeric_limits<double>::max();
    output.write(double_var, "double_var_max");

    output.write(ldouble_var, "ldouble_var_nan");
    ldouble_var = -1.1;
    output.write(ldouble_var, "ldouble_var_1d1");

    output.write(cint_var, "cint_var");

    output.write(cfloat_var, "cfloat_var_nan");
    cfloat_var = {-1.1, 2.2};
    output.write(cfloat_var, "cfloat_var_1d1");

    output.write(cdouble_var, "cdouble_var_nan");
    cdouble_var = {-1.1, 2.2};
    output.write(cdouble_var, "cdouble_var_1d1");

    output.write(cldouble_var, "cldouble_var_nan");
    cldouble_var = {-1.1, 2.2};
    output.write(cldouble_var, "cldouble_var_1d1");

    output.write(int_vect, "int_vect");
    output.write(uint_vect, "uint_vect");

    output.write(float_vect, "float_vect");
    output.write(double_vect, "double_vect");
    output.write(ldouble_vect, "ldouble_vect");

    output.write(cint_vect, "cint_vect");
    //output.write(cuint_vect, "cuint_vect");

    output.write(cfloat_vect, "cfloat_vect");
    output.write(cdouble_vect, "cdouble_vect");
    output.write(cldouble_vect, "cldouble_vect");

    output.write(int_mat, "int_mat");
    //output.write(uint_mat, "uint_mat");

    output.write(float_mat, "float_mat");
    output.write(double_mat, "double_mat");
    output.write(ldouble_mat, "ldouble_mat");

    output.write(cint_mat, "cint_mat");
    //output.write(cuint_mat, "cuint_mat");

    output.write(cfloat_mat, "cfloat_mat");
    output.write(cdouble_mat, "cdouble_mat");
    output.write(cldouble_mat, "cldouble_mat");


    return 0;
}