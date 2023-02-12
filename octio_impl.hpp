//
// Copyright (c) 2018, 2022 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>
#include <cstdint>

namespace ssan {
namespace octio {

namespace {
const char comment_char = '#';
const std::string name_tag = "name:";
const std::string type_tag = "type:";
const std::string rows_tag = "rows:";
const std::string cols_tag = "columns:";
const std::string length_tag = "length:";
const std::string elements_tag = "elements:";
const std::string dim_tag = "ndims:";
const std::string scalar_tag = "scalar";
const std::string string_tag = "string";
const std::string matrix_tag = "matrix";
const std::string complex_tag = "complex";
const std::string int8_tag = "int8";
const std::string uint8_tag = "uint8";
const std::string int16_tag = "int16";
const std::string uint16_tag = "uint16";
const std::string int32_tag = "int32";
const std::string uint32_tag = "uint32";
const std::string int64_tag = "int64";
const std::string uint64_tag = "uint64";
}

class writer_impl {
private:
    std::ostream *m_os;
    bool own_stream;

    void write_file_header(const std::string &header) {
        *m_os << comment_char << " " << header << std::endl;
    }

    /// Convert T to Octave type
    template<typename T>
    octave_var_type var_type();

    /// Write header
    void write_header(octave_obj_descriptor &desc, const std::string &name);

    /// Write one scalar value
    template<typename T>
    void write_one(const T &val);

    /// Write one complex value in "(<real>,<imag>)" format
    template<typename T>
    void write_one_complex(const std::complex<T> &val);

    template<typename T>
    bool write_scalar(const T &val, const std::string name);

    bool write_string(const std::string &str,
                      const std::string &name);

    template<typename T>
    bool write_vector(const std::vector<T> &vect,
                      const std::string &name);

    template<typename T>
    bool write_covector(const std::vector<T> &vect,
                        const std::string &name);

    template<typename T>
    bool write_matrix(const std::vector<std::vector<T> > &mat,
                      const std::string &name);

    template<typename T>
    bool write_complex_scalar(const std::complex<T> &val, const std::string
    &name);

    template<typename T>
    bool write_complex_vector(const std::vector<std::complex<T>> &vect,
                              const std::string &name);

    template<typename T>
    bool write_complex_covector(const std::vector<std::complex<T>> &vect,
                                const std::string &name);

    template<typename T>
    bool
    write_complex_matrix(const std::vector<std::vector<std::complex<T>>> &mat,
                         const std::string &name);

    writer_impl() {}

public:
    writer_impl (std::ostream *os, const std::string &header) {
        m_os = os;
        own_stream = false;
        write_file_header(header);
    }

    writer_impl (const std::string &file, const std::string &header) {
        m_os = new std::ofstream(file, std::ios::out);
        own_stream = true;
        write_file_header(header);
    }

    ~writer_impl () {
        if (own_stream) {
            dynamic_cast<std::ofstream *>(m_os)->close();
            delete (m_os);
        }
    }

    template<typename T>
    bool write(const T &val, const std::string &name);

    /// Transpose vector and write it out as a single column
    template<typename T>
    bool write_covec(const std::vector<T> &val, const std::string &name);
};

template<typename T> octave_var_type writer_impl::var_type() {
    octave_var_type var_type = DOUBLE;
    if (std::is_same<T, std::complex<double>>::value &&
        std::is_same<T, std::complex<float>>::value) {
        var_type = COMPLEX;
    } else if (std::is_same<T, int8_t>::value) {
        var_type = INT8;
    } else if (std::is_same<T, uint8_t>::value) {
        var_type = UINT8;
    } else if (std::is_same<T, int16_t>::value) {
        var_type = INT16;
    } else if (std::is_same<T, uint16_t>::value) {
        var_type = UINT16;
    } else if (std::is_same<T, int32_t>::value) {
        var_type = INT32;
    } else if (std::is_same<T, uint32_t>::value) {
        var_type = UINT32;
    } else if (std::is_same<T, int64_t>::value) {
        var_type = INT64;
    } else if (std::is_same<T, uint64_t>::value) {
        var_type = UINT64;
    }

    return var_type;
}

/// Write header
inline void writer_impl::write_header(octave_obj_descriptor & type,
                                      const std::string &name) {
    *m_os << comment_char << ' ' << name_tag << ' ' << name << std::endl;
    *m_os << comment_char << ' ' << type_tag << ' ';

    // Add type specifier, if needed
    if (type.obj_type != STRING) {
        switch (type.var_type) {
            case COMPLEX:
                *m_os <<  complex_tag <<  ' ';
                break;
            case INT8:
                *m_os <<  int8_tag <<  ' ';
                break;
            case UINT8:
                *m_os <<  uint8_tag <<  ' ';
                break;
            case INT16:
                *m_os <<  int16_tag <<  ' ';
                break;
            case UINT16:
                *m_os <<  uint16_tag <<  ' ';
                break;
            case INT32:
                *m_os <<  int32_tag <<  ' ';
                break;
            case UINT32:
                *m_os <<  uint32_tag <<  ' ';
                break;
            case INT64:
                *m_os <<  int64_tag <<  ' ';
                break;
            case UINT64:
                *m_os <<  uint64_tag <<  ' ';
                break;
            default:
                break;
        }
    }

    // Add object (geometry) type
    switch (type.obj_type) {
        case SCALAR:
            *m_os << scalar_tag << std::endl;
            break;
        case STRING:
            *m_os << string_tag << std::endl;
            break;
        case VECTOR:
        case COVECTOR:
        case MATRIX:
        case NDIM_ARRAY:
            *m_os << matrix_tag << std::endl;
            break;
        default:
            *m_os << "unknown" << std::endl;
    }

    // Add dimentions for non-scalar types
    if (type.obj_type != SCALAR) {
        *m_os << comment_char << ' ';
        switch (type.obj_type) {
            case STRING:
                assert(type.dims.size() > 0 && type.dims.size() < 3);
                *m_os << elements_tag << ' ' << ((type.dims.size() > 1) ? type.dims[1] : 1) << std::endl;
                *m_os << length_tag << ' ' << type.dims[0] << std::endl;
                break;
            case VECTOR:
                assert(type.dims.size() == 1);
                *m_os << dim_tag  <<  ' ' << 2 << std::endl;
                *m_os << 1 << ' ' << type.dims[0] << std::endl;
                break;
            case COVECTOR:
                assert(type.dims.size() == 1);
                *m_os << dim_tag  <<  ' ' << 2 << std::endl;
                *m_os << type.dims[0] << ' ' << 1 << std::endl;
                break;
            case MATRIX:
            case NDIM_ARRAY:
                assert(type.dims.size() > 1);
                *m_os << dim_tag  <<  ' ' << type.dims.size() << std::endl;
                for (auto dim: type.dims) {
                    *m_os << ' ' << dim;
                }
                *m_os << std::endl;
                break;
            default:
                break;
        }
    }

}

// Write one scalar value
template<typename T>
void writer_impl::write_one(const T &val) {

    if(std::is_same<T, int8_t>::value || std::is_same<T, uint8_t>::value) {
        *m_os << ' ' << int(val);
    } else {
        *m_os << ' ' << boost::lexical_cast<std::string>(val);
    }
}

/// Write one complex value in "(<real>,<imag>)" format
template<typename T>
void writer_impl::write_one_complex(const std::complex<T> &val) {
    if(std::is_same<T, int8_t>::value || std::is_same<T, uint8_t>::value) {
        *m_os << ' ' << "("
              << int(val.real()) << ','
              << int(val.imag())
              << ")";
    } else {
        *m_os << ' ' << "("
              << boost::lexical_cast<std::string>(val.real()) << ','
              << boost::lexical_cast<std::string>(val.imag())
              << ")";
    }
}

template<typename T>
bool writer_impl::write_scalar(const T &val, const std::string name) {
    octave_obj_descriptor descriptor = {SCALAR, var_type<T>()};
    write_header(descriptor, name);
    write_one(val);
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}

bool writer_impl::write_string(const std::string &str,
                          const std::string &name) {
    octave_obj_descriptor descriptor = {STRING, CHAR,
                                        .dims = {unsigned(str.length())}};
    write_header(descriptor, name);
    *m_os << str;
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}


template<typename T>
bool writer_impl::write_complex_scalar(const std::complex<T> &val,
                                  const std::string &name) {
    octave_obj_descriptor descriptor = {SCALAR, COMPLEX};
    write_header(descriptor, name);
    write_one_complex(val);
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}

template<typename T>
bool writer_impl::write_complex_vector(const std::vector<std::complex<T>> &vect,
                                  const std::string &name) {
    bool success = false;

    if (vect.size()) {
        octave_obj_descriptor descriptor = {VECTOR, COMPLEX, .dims = {unsigned(vect.size())}};
        write_header(descriptor, name);

        for (auto element : vect)
            write_one_complex(element);

        *m_os << std::endl;
        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer_impl::write_complex_covector(const std::vector<std::complex<T>> &vect,
                                    const std::string &name) {
    bool success = false;

    if (vect.size()) {
        octave_obj_descriptor descriptor = {COVECTOR, COMPLEX, .dims = {unsigned(vect.size())}};
        write_header(descriptor, name);

        for (auto element : vect) {
            write_one_complex(element);
            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer_impl::write_complex_matrix(
        const std::vector<std::vector<std::complex<T>>> &mat,
        const std::string &name) {
    bool success = false;

    if (mat.size() && mat[0].size()) {
        octave_obj_descriptor descriptor = {MATRIX, COMPLEX,
                .dims = {unsigned(mat.size()), unsigned(mat[0].size())}};
        write_header(descriptor, name);

        for (auto &row : mat) {
            for (auto element : row)
                write_one_complex(element);

            *m_os << std::endl;
        }

        *m_os << std::endl << std::endl;

        success = true;
    }

    return success;
}

template<typename T>
bool writer_impl::write_vector(const std::vector<T> &vect,
                          const std::string &name) {
    bool success = false;

    if (vect.size()) {
        octave_obj_descriptor descriptor = {VECTOR, var_type<T>(),
                .dims = {unsigned(vect.size())}};
        write_header(descriptor, name);

        for (auto element : vect) {
            write_one(element);
        }

        *m_os << std::endl;
        *m_os << std::endl << std::endl;
        success = true;
    }

    return success;
}

template<typename T>
bool writer_impl::write_covector(const std::vector<T> &vect,
                            const std::string &name) {
    bool success = false;

    if (vect.size()) {
        octave_obj_descriptor descriptor = {COVECTOR, var_type<T>(),
                .dims = {unsigned(vect.size())}};
        write_header(descriptor, name);

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
bool writer_impl::write_matrix(const std::vector<std::vector<T> > &mat,
                          const std::string &name) {
    bool success = false;

    if (mat.size() && mat[0].size()) {
        octave_obj_descriptor descriptor = {MATRIX, var_type<T>(),
                .dims = {unsigned(mat.size()), unsigned(mat[0].size())}};
        write_header(descriptor, name);

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
inline bool writer_impl::write<int8_t>(const int8_t &val,
                                  const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<int16_t>(const int16_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<int32_t>(const int32_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<int64_t>(const int64_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<uint8_t>(const uint8_t &val,
                                  const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<uint16_t>(const uint16_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<uint32_t>(const uint32_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<uint64_t>(const uint64_t &val,
                                   const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<float>(const float &val,
                                 const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<double>(const double &val,
                                  const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool writer_impl::write<long double>(const long double &val,
                                       const std::string &name) {
    return writer_impl::write_scalar(val, name);
}

template<>
inline bool
writer_impl::write<std::vector<int8_t>>(const std::vector<int8_t> &vect,
                                   const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<int16_t>>(const std::vector<int16_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<int32_t>>(const std::vector<int32_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<int64_t>>(const std::vector<int64_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<uint8_t>>(const std::vector<uint8_t> &vect,
                                   const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<uint16_t>>(const std::vector<uint16_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<uint32_t>>(const std::vector<uint32_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool
writer_impl::write<std::vector<uint64_t>>(const std::vector<uint64_t> &vect,
                                    const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<float>>(const std::vector<float> &vect,
                                              const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<double>>(const std::vector<double> &vect,
                                               const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<long double>>
        (const std::vector<long double> &vect,
         const std::string &name) {
    return writer_impl::write_vector(vect, name);
}

template<>
inline bool writer_impl::write_covec<int8_t>(const std::vector<int8_t> &vect,
                                        const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<int16_t>(const std::vector<int16_t> &vect,
                                         const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<int32_t>(const std::vector<int32_t> &vect,
                                         const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<int64_t>(const std::vector<int64_t> &vect,
                                         const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<float>(const std::vector<float> &vect,
                                       const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<double>(const std::vector<double> &vect,
                                        const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool
writer_impl::write_covec<long double>(const std::vector<long double> &vect,
                                 const std::string &name) {
    return writer_impl::write_covector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<int8_t>>>
        (const std::vector<std::vector<int8_t> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<int16_t>>>
        (const std::vector<std::vector<int16_t> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<int32_t>>>
        (const std::vector<std::vector<int32_t> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<int64_t>>>
        (const std::vector<std::vector<int64_t> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<float>>>
        (const std::vector<std::vector<float> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<double>>>
        (const std::vector<std::vector<double> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<long double>>>
        (const std::vector<std::vector<long double> > &matrix,
         const std::string &name) {
    return writer_impl::write_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::complex<int8_t>>(const std::complex<int8_t> &val,
                                                const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool
writer_impl::write<std::complex<int16_t>>(const std::complex<int16_t> &val,
                                     const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool
writer_impl::write<std::complex<int32_t>>(const std::complex<int32_t> &val,
                                     const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool
writer_impl::write<std::complex<int64_t>>(const std::complex<int64_t> &val,
                                     const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool writer_impl::write<std::complex<float>>(const std::complex<float> &val,
                                               const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool writer_impl::write<std::complex<double>>(const std::complex<double> &val,
                                                const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool
writer_impl::write<std::complex<long double>>(const std::complex<long
double> &val,
                                         const std::string &name) {
    return writer_impl::write_complex_scalar(val, name);
}

template<>
inline bool writer_impl::write<std::string>
        (const std::string &s,
         const std::string &name) {
    return writer_impl::write_string(s, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<int8_t>>>
        (const std::vector<std::complex<int8_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<int16_t>>>
        (const std::vector<std::complex<int16_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<int32_t>>>
        (const std::vector<std::complex<int32_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<int64_t>>>
        (const std::vector<std::complex<int64_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<float>>>
        (const std::vector<std::complex<float>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<double>>>
        (const std::vector<std::complex<double>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::complex<long double>>>
        (const std::vector<std::complex<long double>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_vector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<int8_t>>
        (const std::vector<std::complex<int8_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<int16_t>>
        (const std::vector<std::complex<int16_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<int32_t>>
        (const std::vector<std::complex<int32_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<int64_t>>
        (const std::vector<std::complex<int64_t>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<float>>
        (const std::vector<std::complex<float>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<double>>
        (const std::vector<std::complex<double>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write_covec<std::complex<long double>>
        (const std::vector<std::complex<long double>> &vect,
         const std::string &name) {
    return writer_impl::write_complex_covector(vect, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<int8_t>>>>
        (const std::vector<std::vector<std::complex<int8_t>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<int16_t>>>>
        (const std::vector<std::vector<std::complex<int16_t>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<int32_t>>>>
        (const std::vector<std::vector<std::complex<int32_t>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<int64_t>>>>
        (const std::vector<std::vector<std::complex<int64_t>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<unsigned int>>>>
        (const std::vector<std::vector<std::complex<unsigned int>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<float>>>>
        (const std::vector<std::vector<std::complex<float>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<double>>>>
        (const std::vector<std::vector<std::complex<double>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

template<>
inline bool writer_impl::write<std::vector<std::vector<std::complex<long double>>>>
        (const std::vector<std::vector<std::complex<long double>>> &matrix,
         const std::string &name) {
    return writer_impl::write_complex_matrix(matrix, name);
}

writer::writer(std::ostream *os, const std::string &header) :
        impl(new writer_impl(os, header)) {}

writer::writer(const std::string &file, const std::string &header):
        impl(new writer_impl(file, header)) {}

template<typename T>
bool writer::write(const T &val, const std::string &name) {
    return impl->write<T>(val, name);
}
template<typename T>
bool writer::write_covec(const std::vector<T> &val, const std::string &name) {
    return impl->write_covec<T>(val, name);
}

#ifndef OCTAVE_WRITE_ONLY

class reader_impl {
private:
    std::size_t count;
    std::string header;
    std::string var_name;
    octave_obj_descriptor var_desc;
    std::size_t rows;
    std::size_t cols;
    bool valid;
    std::istream *m_is;

    void read_header(void);

    /// Read one scalar value
    template<typename T>
    void read_one(std::istream &istr, T &val);

    /// Read one complex value in "(<real>,<imag>)" format
    template<typename T>
    void read_one_complex(std::istream &istr, std::complex<T> &val);

    template<typename T>
    bool read_scalar(T &val);

    template<typename T>
    bool read_complex_scalar(std::complex<T> &val);

    template<typename T>
    bool read_vector(std::vector<T> &vect);

    template<typename T>
    bool read_complex_vector(std::vector<std::complex<T>> &vect);

    template<typename T>
    bool read_matrix(std::vector<std::vector<T>> &matrix);

    template<typename T>
    bool read_complex_matrix(std::vector<std::vector<std::complex<T>>> &matrix);

public:
    reader_impl(std::istream *is) {
        m_is = is;
        count = 0;
        read_header();
    }

    std::string title() {
        if (valid) {
            return header;
        } else {
            return "INVALID";
        }
    }

    octave_obj_descriptor next_type() {
        if (valid) {
            return var_desc;
        } else {
            return {INVALID};
        }
    }

    std::string next_name() {
        if (valid) {
            return var_name;
        } else {
            return "INVALID";
        }
    }

    bool skip_one();

    template<typename T>
    bool read(T &var);
};


inline void  reader_impl::read_header(void) {
    std::string str;
    char ch;
    valid = false;
    rows = 0;
    cols = 0;
    var_desc.obj_type = INVALID;
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
             var_desc.obj_type = SCALAR;
        } else if (str == matrix_tag) {
             var_desc.obj_type = MATRIX;
        } else if (str == complex_tag && *m_is >> str) {
            if (str == scalar_tag) {
                 var_desc.var_type = COMPLEX;
                 var_desc.obj_type = SCALAR;
            } else if (str == matrix_tag) {
                var_desc.var_type = COMPLEX;
                var_desc.obj_type = MATRIX;
            } else {
                return; // failed
            }
        } else {
            return; // failed
        }
    } else {
        return; // failed
    }

    if ( var_desc.obj_type == SCALAR) {
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
        if (var_desc.obj_type == MATRIX) {
             var_desc.obj_type = VECTOR;
        }
    } else if (cols == 1) {
        if (var_desc.obj_type == MATRIX) {
             var_desc.obj_type = COVECTOR;
        }
    }

    valid = true;
    count++;
    std::getline(*m_is, str); // consume last end of line
    return; // success
}

/// Read one scalar value
template<typename T>
void  reader_impl::read_one(std::istream &istr, T &val) {
    std::string val_string;
    istr >> val_string;
    val = boost::lexical_cast<T>(val_string);
}

/// Read one complex value in "(<real>,<imag>)" format
template<typename T>
void  reader_impl::read_one_complex(std::istream &istr, std::complex<T> &val) {
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
bool  reader_impl::read_scalar(T &val) {
    if (var_desc.var_type != COMPLEX && var_desc.obj_type == SCALAR) {
        read_one(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool  reader_impl::read_complex_scalar(std::complex<T> &val) {
    if (var_desc.var_type == COMPLEX && var_desc.obj_type == SCALAR) {
        read_one_complex(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool  reader_impl::read_vector(std::vector<T> &vect) {
    if (var_desc.var_type != COMPLEX && var_desc.obj_type == VECTOR) {
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
    } else if (var_desc.var_type != COMPLEX && var_desc.obj_type == COVECTOR) {
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
bool  reader_impl::read_complex_vector(std::vector<std::complex<T>> &vect) {
    if (var_desc.var_type == COMPLEX && var_desc.obj_type == VECTOR) {
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
    } else if (var_desc.var_type == COMPLEX && var_desc.obj_type == COVECTOR) {
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
bool  reader_impl::read_matrix(std::vector<std::vector<T> > &matrix) {
    if (var_desc.var_type != COMPLEX &&
        (var_desc.obj_type == MATRIX ||
         var_desc.obj_type == VECTOR ||
         var_desc.obj_type == COVECTOR)) {
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
bool  reader_impl::read_complex_matrix(std::vector<std::vector<std::complex<T>>>
                                 &matrix) {
    if (var_desc.var_type == COMPLEX &&
        (var_desc.obj_type == MATRIX ||
         var_desc.obj_type == VECTOR ||
         var_desc.obj_type == COVECTOR)) {
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


inline bool  reader_impl::skip_one() {
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
bool  reader_impl::read(T &var) {
    return false; // always fail for unknown types
}

template<>
inline bool  reader_impl::read<int8_t>(int8_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<int16_t>(int16_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<int32_t>(int32_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<int64_t>(int64_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<uint8_t>(uint8_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<uint16_t>(uint16_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<uint32_t>(uint32_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<uint64_t>(uint64_t &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<float>(float &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<double>(double &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<long double>(long double &val) {
    return  reader_impl::read_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<int8_t>>(std::complex<int8_t> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<int16_t>>(std::complex<int16_t> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<int32_t>>(std::complex<int32_t> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<int64_t>>(std::complex<int64_t> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<float>>(std::complex<float> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool  reader_impl::read<std::complex<double>>(std::complex<double> &val) {
    return  reader_impl::read_complex_scalar(val);
}

template<>
inline bool
 reader_impl::read<std::complex<long double>>(std::complex<long double> &val) {
    return  reader_impl::read_complex_scalar(val);
}


template<>
inline bool  reader_impl::read<std::vector<int8_t>>
        (std::vector<int8_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<int16_t>>
        (std::vector<int16_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<int32_t>>
        (std::vector<int32_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<int64_t>>
        (std::vector<int64_t> &vect) {
    return  reader_impl::read_vector(vect);
}


template<>
inline bool  reader_impl::read<std::vector<uint8_t>>
        (std::vector<uint8_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<uint16_t>>
        (std::vector<uint16_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<uint32_t>>
        (std::vector<uint32_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<uint64_t>>
        (std::vector<uint64_t> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<float>>
        (std::vector<float> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<double>>
        (std::vector<double> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<long double>>
        (std::vector<long double> &vect) {
    return  reader_impl::read_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<int8_t>>>
        (std::vector<std::complex<int8_t>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<int16_t>>>
        (std::vector<std::complex<int16_t>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<int32_t>>>
        (std::vector<std::complex<int32_t>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<int64_t>>>
        (std::vector<std::complex<int64_t>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<float>>>
        (std::vector<std::complex<float>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<double>>>
        (std::vector<std::complex<double>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::complex<long double>>>
        (std::vector<std::complex<long double>> &vect) {
    return  reader_impl::read_complex_vector(vect);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<int8_t>>>
        (std::vector<std::vector<int8_t> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<int16_t>>>
        (std::vector<std::vector<int16_t> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<int32_t>>>
        (std::vector<std::vector<int32_t> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<int64_t>>>
        (std::vector<std::vector<int64_t> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<float>>>
        (std::vector<std::vector<float> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<double>>>
        (std::vector<std::vector<double> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<long double>>>
        (std::vector<std::vector<long double> > &matrix) {
    return  reader_impl::read_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<int8_t>>>>
        (std::vector<std::vector<std::complex<int8_t>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<int16_t>>>>
        (std::vector<std::vector<std::complex<int16_t>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<int32_t>>>>
        (std::vector<std::vector<std::complex<int32_t>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<int64_t>>>>
        (std::vector<std::vector<std::complex<int64_t>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<float>>>>
        (std::vector<std::vector<std::complex<float>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<double>>>>
        (std::vector<std::vector<std::complex<double>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

template<>
inline bool  reader_impl::read<std::vector<std::vector<std::complex<long double>>>>
        (std::vector<std::vector<std::complex<long double>>> &matrix) {
    return  reader_impl::read_complex_matrix(matrix);
}

reader::reader(std::istream *is) : impl(new reader_impl(is)) {}

std::string reader::title() {return impl->title();}

octave_obj_descriptor reader::next_type() {return impl->next_type();}

std::string reader::next_name() {return impl->next_name();}

bool reader::skip_one() {return impl->skip_one();}

template<typename T>
bool reader::read(T &var) {return impl->template read<T>(var);}

#endif

}
}
