//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
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

#include <boost/lexical_cast.hpp>


#include "octio.hpp"

using boost::lexical_cast;
using namespace ssan::octio;

template<typename T>
void print_matrix(const std::vector<std::vector<T>> &matrix,
        const std::string & name) {
    if (!matrix.empty()) {
        std::cout << name << "["
                  << matrix.size() << ","
                  << matrix[0].size() << "]: [" << std::endl;
        for (const auto & row : matrix) {
            for (auto element : row) {
                std::cout << " " << lexical_cast<std::string>(element);
            }
            std::cout << std::endl;
        }
        std::cout << "]" << std::endl;
    }
}

template<typename T>
void print_vector(const std::vector<T> &vector, const std::string & name) {
    if (!vector.empty()) {
        std::cout << name << "["
                  << vector.size() << "]: [";
        for (auto element : vector) {
                std::cout << " " << lexical_cast<std::string>(element);
        }
        std::cout << "]" << std::endl;
    }
}



int main() {
    int int_var = -1;
    float float_var(nanf(""));
    double double_var(nan(""));
    long double ldouble_var(nanl(""));
    std::complex<float> cfloat_var(nanf(""), nanf(""));
    std::complex<double> cdouble_var(nan(""), nan(""));
    std::complex<long double> cldouble_var(nanl(""), nanl(""));
    std::vector<int> int_vect;
    std::vector<float> float_vect;
    std::vector<double> double_vect;
    std::vector<long double> ldouble_vect;
    std::vector<int> int_covect;
    std::vector<float> float_covect;
    std::vector<double> double_covect;
    std::vector<long double> ldouble_covect;
    std::vector<std::complex<int>> cint_vect;
    std::vector<std::complex<float>> cfloat_vect;
    std::vector<std::complex<double>> cdouble_vect;
    std::vector<std::complex<long double>> cldouble_vect;
    std::vector<std::complex<float>> cfloat_covect;
    std::vector<std::complex<double>> cdouble_covect;
    std::vector<std::complex<long double>> cldouble_covect;
    std::vector<std::vector<int>> int_mat;
    std::vector<std::vector<float>> float_mat;
    std::vector<std::vector<double>> double_mat;
    std::vector<std::vector<long double>> ldouble_mat;
    std::vector<std::vector<std::complex<float>>> cfloat_mat;
    std::vector<std::vector<std::complex<double>>> cdouble_mat;
    std::vector<std::vector<std::complex<long double>>> cldouble_mat;

    const std::string file_name = "test_octio.mat";
    const std::string file_name_out = "test_octio_out.mat";

    std::ifstream file(file_name);
    reader input(&file);
    std::ofstream ofile(file_name_out);
    writer output(&ofile, "Test file");

    std::cout << input.title() << std::endl;
    do {
        bool consumed = false;
        // std::cout << input.next_name() << std::endl;
        // std::cout << input.next_type() << std::endl;
        if (input.next_type() == SCALAR) {
            if (input.next_name() == "int_var") {
                consumed = input.read(int_var);
                output.write(int_var, "int_var");
            } else if (input.next_name() == "float_var") {
                consumed = input.read(float_var);
                output.write(float_var, "float_var");
            } else if (input.next_name() == "double_var") {
                consumed = input.read(double_var);
                output.write(double_var, "double_var");
            } else if (input.next_name() == "ldouble_var") {
                consumed = input.read(ldouble_var);
                output.write(ldouble_var, "ldouble_var");
            }
        } else if (input.next_type() == COMPLEX_SCALAR) {
            if (input.next_name() == "cfloat_var") {
                consumed = input.read(cfloat_var);
                output.write(cfloat_var, "cfloat_var");
            } else if (input.next_name() == "cdouble_var") {
                consumed = input.read(cdouble_var);
                output.write(cdouble_var, "cdouble_var");
            } else if (input.next_name() == "cldouble_var") {
                consumed = input.read(cldouble_var);
                output.write(cldouble_var, "cldouble_var");
            }
        } else if (input.next_type() == VECTOR
                   || input.next_type() == COVECTOR) {
            if (input.next_name() == "int_vect") {
                consumed = input.read(int_vect);
            } else if (input.next_name() == "float_vect") {
                consumed = input.read(float_vect);
                output.write(float_vect, "float_vect");
            } else if (input.next_name() == "double_vect") {
                consumed = input.read(double_vect);
            } else if (input.next_name() == "ldouble_vect") {
                consumed = input.read(ldouble_vect);
            } else if (input.next_name() == "int_covect") {
                consumed = input.read(int_covect);
            } else if (input.next_name() == "float_covect") {
                consumed = input.read(float_covect);
                output.write_covec(float_covect, "float_covect");
            } else if (input.next_name() == "double_covect") {
                consumed = input.read(double_covect);
            } else if (input.next_name() == "ldouble_covect") {
                consumed = input.read(ldouble_covect);
            }
        } else if (input.next_type() == COMPLEX_VECTOR
                   || input.next_type() == COMPLEX_COVECTOR) {
            if (input.next_name() == "cint_vect") {
                consumed = input.read(cint_vect);
                output.write(cint_vect, "cint_vect");
            } else if (input.next_name() == "cfloat_vect") {
                consumed = input.read(cfloat_vect);
                output.write(cfloat_vect, "cfloat_vect");
            } else if (input.next_name() == "cdouble_vect") {
                consumed = input.read(cdouble_vect);
            } else if (input.next_name() == "cldouble_vect") {
                consumed = input.read(cldouble_vect);
            } else if (input.next_name() == "cfloat_covect") {
                consumed = input.read(cfloat_covect);
                output.write_covec(cfloat_covect, "cfloat_covect");
            } else if (input.next_name() == "cdouble_covect") {
                consumed = input.read(cdouble_covect);
            } else if (input.next_name() == "cldouble_covect") {
                consumed = input.read(cldouble_covect);
            }
        } else if (input.next_type() == MATRIX) {
            if (input.next_name() == "int_mat") {
                consumed = input.read(int_mat);
            } else if (input.next_name() == "float_mat") {
                consumed = input.read(float_mat);
                output.write(float_mat, "float_mat");
            } else if (input.next_name() == "double_mat") {
                consumed = input.read(double_var);
            } else if (input.next_name() == "ldouble_mat") {
                consumed = input.read(ldouble_var);
            }
        } else if (input.next_type() == COMPLEX_MATRIX) {
            if (input.next_name() == "cfloat_mat") {
                consumed = input.read(cfloat_mat);
                output.write(cfloat_mat, "cfloat_mat");
            } else if (input.next_name() == "cdouble_mat") {
                consumed = input.read(cdouble_mat);
            } else if (input.next_name() == "cldouble_mat") {
                consumed = input.read(cldouble_mat);
            }
        }


        if (!consumed) {
            input.skip_one();
        }

    } while (input.next_type() != INVALID);


    std::cout << "int_var: " << int_var << std::endl;
    std::cout << "float_var: " << lexical_cast<std::string>(float_var)
              << std::endl;
    std::cout << "double_var: " << lexical_cast<std::string>(double_var)
              << std::endl;
    std::cout << "ldouble_var: " << lexical_cast<std::string>(ldouble_var)
              << std::endl;
    std::cout << "cfloat_var: ("
              << lexical_cast<std::string>(cfloat_var.real()) << ","
              << lexical_cast<std::string>(cfloat_var.imag()) << ")"
              << std::endl;
    std::cout << "cdouble_var: ("
              << lexical_cast<std::string>(cdouble_var.real()) << ","
              << lexical_cast<std::string>(cdouble_var.imag()) << ")"
              << std::endl;
    std::cout << "cldouble_var: ("
              << lexical_cast<std::string>(cldouble_var.real()) << ","
              << lexical_cast<std::string>(cldouble_var.imag()) << ")"
              << std::endl;
    print_vector(float_vect, "float_vect");
    print_vector(float_covect, "float_covect");
    print_vector(cint_vect, "cint_vect");
    print_vector(cfloat_vect, "cfloat_vect");
    print_vector(cfloat_covect, "cfloat_covect");
    print_matrix(float_mat, "float_mat");
    print_matrix(cfloat_mat, "cfloat_mat");

    return 0;
}