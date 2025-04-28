//
// Copyright (c) 2018, 2022 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cstdint>
#include <complex>
#include <fstream>
#include <type_traits>

namespace ssan {
namespace octio {

namespace {
// make sure int8_t and uint8_t are treated as integer types and not as char
inline
std::ostream& operator<<(std::ostream& os, int8_t val) {
    return os << static_cast<short>(val);
}

inline
std::ostream& operator<<(std::ostream& os, uint8_t val) {
    return os << static_cast<unsigned short>(val);
}

inline
std::ostream& operator<<(std::ostream& os, std::complex<int8_t> val) {
    return os << std::complex<short>(short(val.real()),short(val.imag()));
}
}

/// Write header
inline void writer::write_header(enum octave_type type,
                                 const std::string &name) {
    *m_os << comment_char << ' ' << name_tag << ' ' << name << std::endl;
    *m_os << comment_char << ' ' << type_tag << ' ';
    switch (type) {
        case SCALAR:
            *m_os << scalar_tag << std::endl;
            break;
        case STRING:
            *m_os << string_tag << std::endl;
            break;
        case VECTOR:
        case COVECTOR:
        case MATRIX:
            *m_os << matrix_tag << std::endl;
            break;
        case COMPLEX_SCALAR:
            *m_os << complex_tag << ' ' << scalar_tag << std::endl;
            break;
        case COMPLEX_VECTOR:
        case COMPLEX_COVECTOR:
        case COMPLEX_MATRIX:
            *m_os << complex_tag << ' ' << matrix_tag << std::endl;
            break;
        default:
            *m_os << "unknown" << std::endl;
    }
}

/// Write dimentions
inline void writer::write_dims(std::size_t rows, std::size_t cols) {
    // TODI: add crosscheck with var type.
    *m_os << "# rows: " << rows << std::endl;
    *m_os << "# columns: " << cols << std::endl;
}

//// Write string dims
inline void writer::write_strdesc(std::size_t length, std::size_t count) {
    // TODI: add crosscheck with var type.
    *m_os << "# elements: " << count << std::endl;
    *m_os << "# length: " << length << std::endl;
}

// Write one scalar value
template<typename T>
void writer::write_one(const T &val) {
    *m_os << ' ' << val;
}

template<typename T>
bool writer::write_scalar(const T &val, const std::string name) {
    write_header(SCALAR, name);
    write_one(val);
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}

bool writer::write_string(const std::string &str,
                          const std::string &name) {
    write_header(STRING, name);
    write_strdesc(str.length());
    *m_os << str;
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}


template<typename T>
bool writer::write_complex_scalar(const std::complex<T> &val,
                                  const std::string &name) {
    write_header(COMPLEX_SCALAR, name);
    write_one(val);
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}

template<typename T>
bool writer::write_complex_vector(const std::vector<std::complex<T>> &vect,
                                  const std::string &name) {
    bool success = false;

    if (vect.size()) {
        write_header(COMPLEX_VECTOR, name);
        write_dims(1, vect.size());

        for (auto element : vect)
            write_one(element);

        *m_os << std::endl;
        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer::write_complex_covector(const std::vector<std::complex<T>> &vect,
                                    const std::string &name) {
    bool success = false;

    if (vect.size()) {
        write_header(COMPLEX_COVECTOR, name);
        write_dims(vect.size(), 1);

        for (auto element : vect) {
            write_one(element);
            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer::write_complex_matrix(
        const std::vector<std::vector<std::complex<T>>> &mat,
        const std::string &name) {
    bool success = false;

    if (mat.size() && mat[0].size()) {
        write_header(COMPLEX_MATRIX, name);
        write_dims(mat.size(), mat[0].size());
        for (auto &row : mat) {
            for (auto element : row)
                write_one(element);

            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;

        success = true;
    }

    return success;
}

template<typename T>
bool writer::write_vector(const std::vector<T> &vect,
                          const std::string &name) {
    bool success = false;

    if (vect.size()) {
        write_header(VECTOR, name);
        write_dims(1, vect.size());

        for (auto element : vect)
            write_one(element);

        *m_os << std::endl;
        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer::write_covector(const std::vector<T> &vect,
                            const std::string &name) {
    bool success = false;

    if (vect.size()) {
        write_header(COVECTOR, name);
        write_dims(vect.size(), 1);

        for (auto element : vect) {
            write_one(element);
            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer::write_matrix(const std::vector<std::vector<T> > &mat,
                          const std::string &name) {
    bool success = false;

    if (mat.size() && mat[0].size()) {
        write_header(MATRIX, name);
        write_dims(mat.size(), mat[0].size());
        for (auto &row : mat) {
            for (auto element : row)
                write_one(element);

            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<>
inline bool writer::write<int8_t>(const int8_t &val,
                                  const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<int16_t>(const int16_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<int32_t>(const int32_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<int64_t>(const int64_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<uint8_t>(const uint8_t &val,
                                  const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<uint16_t>(const uint16_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<uint32_t>(const uint32_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<uint64_t>(const uint64_t &val,
                                   const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<float>(const float &val,
                                 const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<double>(const double &val,
                                  const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool writer::write<long double>(const long double &val,
                                       const std::string &name) {
    return writer::write_scalar(val, name);
}

template<>
inline bool
writer::write<std::vector<int8_t>>(const std::vector<int8_t> &vect,
                                   const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<int16_t>>(const std::vector<int16_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<int32_t>>(const std::vector<int32_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<int64_t>>(const std::vector<int64_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<uint8_t>>(const std::vector<uint8_t> &vect,
                                   const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<uint16_t>>(const std::vector<uint16_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<uint32_t>>(const std::vector<uint32_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool
writer::write<std::vector<uint64_t>>(const std::vector<uint64_t> &vect,
                                    const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<float>>(const std::vector<float> &vect,
                                              const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<double>>(const std::vector<double> &vect,
                                               const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<long double>>
        (const std::vector<long double> &vect,
         const std::string &name) {
    return writer::write_vector(vect, name);
}

template<>
inline bool writer::write_covec<int8_t>(const std::vector<int8_t> &vect,
                                        const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write_covec<int16_t>(const std::vector<int16_t> &vect,
                                         const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write_covec<int32_t>(const std::vector<int32_t> &vect,
                                         const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write_covec<int64_t>(const std::vector<int64_t> &vect,
                                         const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write_covec<float>(const std::vector<float> &vect,
                                       const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write_covec<double>(const std::vector<double> &vect,
                                        const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool
writer::write_covec<long double>(const std::vector<long double> &vect,
                                 const std::string &name) {
    return writer::write_covector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::vector<int8_t>>>
        (const std::vector<std::vector<int8_t> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<int16_t>>>
        (const std::vector<std::vector<int16_t> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<int32_t>>>
        (const std::vector<std::vector<int32_t> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<int64_t>>>
        (const std::vector<std::vector<int64_t> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<float>>>
        (const std::vector<std::vector<float> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<double>>>
        (const std::vector<std::vector<double> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<long double>>>
        (const std::vector<std::vector<long double> > &matrix,
         const std::string &name) {
    return writer::write_matrix(matrix, name);
}

template<>
inline bool writer::write<std::complex<int8_t>>(const std::complex<int8_t> &val,
                                                const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool
writer::write<std::complex<int16_t>>(const std::complex<int16_t> &val,
                                     const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool
writer::write<std::complex<int32_t>>(const std::complex<int32_t> &val,
                                     const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool
writer::write<std::complex<int64_t>>(const std::complex<int64_t> &val,
                                     const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool writer::write<std::complex<float>>(const std::complex<float> &val,
                                               const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool writer::write<std::complex<double>>(const std::complex<double> &val,
                                                const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool
writer::write<std::complex<long double>>(const std::complex<long
double> &val,
                                         const std::string &name) {
    return writer::write_complex_scalar(val, name);
}

template<>
inline bool writer::write<std::string>
        (const std::string &s,
         const std::string &name) {
    return writer::write_string(s, name);
}

template<>
inline bool writer::write<std::vector<std::complex<int8_t>>>
        (const std::vector<std::complex<int8_t>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<int16_t>>>
        (const std::vector<std::complex<int16_t>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<int32_t>>>
        (const std::vector<std::complex<int32_t>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<int64_t>>>
        (const std::vector<std::complex<int64_t>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<float>>>
        (const std::vector<std::complex<float>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<double>>>
        (const std::vector<std::complex<double>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::complex<long double>>>
        (const std::vector<std::complex<long double>> &vect,
         const std::string &name) {
    return writer::write_complex_vector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<int8_t>>
        (const std::vector<std::complex<int8_t>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<int16_t>>
        (const std::vector<std::complex<int16_t>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<int32_t>>
        (const std::vector<std::complex<int32_t>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<int64_t>>
        (const std::vector<std::complex<int64_t>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<float>>
        (const std::vector<std::complex<float>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<double>>
        (const std::vector<std::complex<double>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write_covec<std::complex<long double>>
        (const std::vector<std::complex<long double>> &vect,
         const std::string &name) {
    return writer::write_complex_covector(vect, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<int8_t>>>>
        (const std::vector<std::vector<std::complex<int8_t>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<int16_t>>>>
        (const std::vector<std::vector<std::complex<int16_t>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<int32_t>>>>
        (const std::vector<std::vector<std::complex<int32_t>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<int64_t>>>>
        (const std::vector<std::vector<std::complex<int64_t>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<float>>>>
        (const std::vector<std::vector<std::complex<float>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<double>>>>
        (const std::vector<std::vector<std::complex<double>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}

template<>
inline bool writer::write<std::vector<std::vector<std::complex<long double>>>>
        (const std::vector<std::vector<std::complex<long double>>> &matrix,
         const std::string &name) {
    return writer::write_complex_matrix(matrix, name);
}


inline void reader::read_header(void) {
    std::string str;
    char ch;
    valid = false;
    rows = 0;
    cols = 0;
    var_type = INVALID;
    var_name = "INVALID";

    if (!count) {
        if (!(*m_is >> ch && ch == comment_char &&
              std::getline(*m_is, header))) {
            return; // failed
        }
    } else { // 2 empty lines
        std::getline(*m_is, str);
        std::getline(*m_is, str);
    }

    if (!(*m_is >> ch && ch == comment_char && *m_is >> str &&
          str == name_tag && *m_is >> var_name)) {
        return; // failed
    }

    if (*m_is >> ch && ch == comment_char && *m_is >> str &&
        str == type_tag && *m_is >> str) {
        if (str == scalar_tag) {
            var_type = SCALAR;
        } else if (str == matrix_tag) {
            var_type = MATRIX;
        } else if (str == complex_tag && *m_is >> str) {
            if (str == scalar_tag) {
                var_type = COMPLEX_SCALAR;
            } else if (str == matrix_tag) {
                var_type = COMPLEX_MATRIX;
            } else {
                return; // failed
            }
        } else {
            return; // failed
        }
    } else {
        return; // failed
    }

    if (var_type == SCALAR || var_type == COMPLEX_SCALAR) {
        valid = true;
        count++;
        return; // success
    } else {
        if (!(*m_is >> ch && ch == comment_char && *m_is >> str &&
              str == rows_tag && *m_is >> rows &&
              *m_is >> ch && ch == comment_char && *m_is >> str &&
              str == cols_tag && *m_is >> cols)) {
            return; // failed
        }
    }

    if (rows == 1) {
        if (var_type == MATRIX) {
            var_type = VECTOR;
        } else if (var_type == COMPLEX_MATRIX) {
            var_type = COMPLEX_VECTOR;
        }
    } else if (cols == 1) {
        if (var_type == MATRIX) {
            var_type = COVECTOR;
        } else if (var_type == COMPLEX_MATRIX) {
            var_type = COMPLEX_COVECTOR;
        }
    }

    valid = true;
    count++;
    std::getline(*m_is, str); // consume last end of line
    return; // success
}

/// Read one scalar value
template<typename T>
void reader::read_one(std::istream &istr, T &val) {
    std::string val_string;
    istr >> val_string;
    val = boost::lexical_cast<T>(val_string);
}

/// Read one complex value in "(<real>,<imag>)" format
template<typename T>
void reader::read_one_complex(std::istream &istr, std::complex<T> &val) {
    std::string strval;

    istr >> strval;

    // turn string "(<real>,<imag>)" into " <real> <imag> "
    std::replace(strval.begin(), strval.end(), ',', ' ');
    std::replace(strval.begin(), strval.end(), '(', ' ');
    std::replace(strval.begin(), strval.end(), ')', ' ');

    std::stringstream line_stream(strval);
    std::string val_string;

    // read real part
    line_stream >> val_string;
    val.real(boost::lexical_cast<T>(val_string));

    // read imaginary part
    line_stream >> val_string;
    val.imag(boost::lexical_cast<T>(val_string));
}


template<typename T>
bool reader::read_scalar(T &val) {
    if (var_type == SCALAR) {
        read_one(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool reader::read_complex_scalar(std::complex<T> &val) {
    if (var_type == COMPLEX_SCALAR) {
        read_one_complex(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool reader::read_vector(std::vector<T> &vect) {
    if (var_type == VECTOR) {
        // rows = 1, cols = N
        std::string line;
        if (std::getline(*m_is, line)) {
            std::stringstream line_stream(line);
            for (int col = 0; col < int(cols); col++) {
                T val;
                read_one(line_stream, val);
                vect.push_back(val);
            }
        }
        read_header();
        return true;
    } else if (var_type == COVECTOR) {
        // rows = N, cols = 1
        std::string line;

        for (int row = 0; row < int(rows); row++) {
            if (std::getline(*m_is, line)) {
                T val;
                std::stringstream line_stream(line);
                read_one(line_stream, val);
                vect.push_back(val);
            }
        }
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool reader::read_complex_vector(std::vector<std::complex<T>> &vect) {
    if (var_type == COMPLEX_VECTOR) {
        // rows = 1, cols = N
        std::string line;
        if (std::getline(*m_is, line)) {
            std::stringstream line_stream(line);
            for (int col = 0; col < int(cols); col++) {
                std::complex<T> val;
                read_one_complex(line_stream, val);
                vect.push_back(val);
            }
        }
        read_header();
        return true;
    } else if (var_type == COMPLEX_COVECTOR) {
        // rows = N, cols = 1
        std::string line;

        for (int row = 0; row < int(rows); row++) {
            if (std::getline(*m_is, line)) {
                std::complex<T> val;
                std::stringstream line_stream(line);
                read_one_complex(line_stream, val);
                vect.push_back(val);
            }
        }
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool reader::read_matrix(std::vector<std::vector<T> > &matrix) {
    if (var_type == MATRIX
        || var_type == VECTOR
        || var_type == COVECTOR) {
        std::string line;
        for (int row = 0; row < int(rows); row++) {
            if (std::getline(*m_is, line)) {
                std::vector<T> mat_row;
                std::stringstream line_stream(line);
                for (int col = 0; col < int(cols); col++) {
                    T val;
                    read_one(line_stream, val);
                    mat_row.push_back(val);
                }
                matrix.push_back(mat_row);
            }
        }
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool reader::read_complex_matrix(std::vector<std::vector<std::complex<T>>>
                                 &matrix) {
    if (var_type == COMPLEX_MATRIX
        || var_type == COMPLEX_VECTOR
        || var_type == COMPLEX_COVECTOR) {
        std::string line;
        for (int row = 0; row < int(rows); row++) {
            if (std::getline(*m_is, line)) {
                std::vector<std::complex<T>> mat_row;
                std::stringstream line_stream(line);
                for (int col = 0; col < int(cols); col++) {
                    std::complex<T> val;
                    read_one_complex(line_stream, val);
                    mat_row.push_back(val);
                }
                matrix.push_back(mat_row);
            }
        }
        read_header();
        return true;
    } else {
        return false;
    }
}


inline bool reader::skip_one() {
    std::size_t lines = rows;
    std::string str;

    if (valid) {
        if (lines == 0)
            lines++;

        for (int i = 0; i < int(lines); i++) {
            std::getline(*m_is, str);
        }
        read_header();

        return true;
    } else {
        return false;
    }
}


template<typename T>
bool reader::read(T &var) {
    return false; // always fail for unknown types
}

template<>
inline bool reader::read<int8_t>(int8_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<int16_t>(int16_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<int32_t>(int32_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<int64_t>(int64_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<uint8_t>(uint8_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<uint16_t>(uint16_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<uint32_t>(uint32_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<uint64_t>(uint64_t &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<float>(float &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<double>(double &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<long double>(long double &val) {
    return reader::read_scalar(val);
}

template<>
inline bool reader::read<std::complex<int8_t>>(std::complex<int8_t> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool reader::read<std::complex<int16_t>>(std::complex<int16_t> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool reader::read<std::complex<int32_t>>(std::complex<int32_t> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool reader::read<std::complex<int64_t>>(std::complex<int64_t> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool reader::read<std::complex<float>>(std::complex<float> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool reader::read<std::complex<double>>(std::complex<double> &val) {
    return reader::read_complex_scalar(val);
}

template<>
inline bool
reader::read<std::complex<long double>>(std::complex<long double> &val) {
    return reader::read_complex_scalar(val);
}


template<>
inline bool reader::read<std::vector<int8_t>>
        (std::vector<int8_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<int16_t>>
        (std::vector<int16_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<int32_t>>
        (std::vector<int32_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<int64_t>>
        (std::vector<int64_t> &vect) {
    return reader::read_vector(vect);
}


template<>
inline bool reader::read<std::vector<uint8_t>>
        (std::vector<uint8_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<uint16_t>>
        (std::vector<uint16_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<uint32_t>>
        (std::vector<uint32_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<uint64_t>>
        (std::vector<uint64_t> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<float>>
        (std::vector<float> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<double>>
        (std::vector<double> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<long double>>
        (std::vector<long double> &vect) {
    return reader::read_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<int8_t>>>
        (std::vector<std::complex<int8_t>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<int16_t>>>
        (std::vector<std::complex<int16_t>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<int32_t>>>
        (std::vector<std::complex<int32_t>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<int64_t>>>
        (std::vector<std::complex<int64_t>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<float>>>
        (std::vector<std::complex<float>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<double>>>
        (std::vector<std::complex<double>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::complex<long double>>>
        (std::vector<std::complex<long double>> &vect) {
    return reader::read_complex_vector(vect);
}

template<>
inline bool reader::read<std::vector<std::vector<int8_t>>>
        (std::vector<std::vector<int8_t> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<int16_t>>>
        (std::vector<std::vector<int16_t> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<int32_t>>>
        (std::vector<std::vector<int32_t> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<int64_t>>>
        (std::vector<std::vector<int64_t> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<float>>>
        (std::vector<std::vector<float> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<double>>>
        (std::vector<std::vector<double> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<long double>>>
        (std::vector<std::vector<long double> > &matrix) {
    return reader::read_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<int8_t>>>>
        (std::vector<std::vector<std::complex<int8_t>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<int16_t>>>>
        (std::vector<std::vector<std::complex<int16_t>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<int32_t>>>>
        (std::vector<std::vector<std::complex<int32_t>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<int64_t>>>>
        (std::vector<std::vector<std::complex<int64_t>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<float>>>>
        (std::vector<std::vector<std::complex<float>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<double>>>>
        (std::vector<std::vector<std::complex<double>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

template<>
inline bool reader::read<std::vector<std::vector<std::complex<long double>>>>
        (std::vector<std::vector<std::complex<long double>>> &matrix) {
    return reader::read_complex_matrix(matrix);
}

}
}
