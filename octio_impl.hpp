//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cstdint>

namespace ssan {
namespace octio {

static const char comment_char = '#';
static const std::string name_tag = "name:";
static const std::string type_tag = "type:";
static const std::string rows_tag = "rows:";
static const std::string cols_tag = "columns:";
static const std::string complex_tag = "complex";
static const std::string scalar_tag = "scalar";
static const std::string string_tag = "string";
static const std::string matrix_tag = "matrix";

class writer_impl {
private:
    std::ostream *m_os;
    bool own_stream;

    void write_file_header(const std::string &header) {
        *m_os << comment_char << " " << header << std::endl;
    }

    /// Write header
    void write_header(enum octave_type type, const std::string &name);

    /// Write dimentions
    void write_dims(std::size_t rows, std::size_t cols);

    //// Write string dims
    void write_strdesc(std::size_t length, std::size_t count = 1);

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

/// Write header
inline void writer_impl::write_header(enum octave_type type,
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
inline void writer_impl::write_dims(std::size_t rows, std::size_t cols) {
    // TODO: add crosscheck with var type.
/* Old way
    *m_os << "# rows: " << rows << std::endl;
    *m_os << "# columns: " << cols << std::endl;

  Use new way from now on
 */
   *m_os << "# ndims: 2" << std::endl;
   *m_os << rows << " " << cols << std::endl;
}

//// Write string dims
inline void writer_impl::write_strdesc(std::size_t length, std::size_t count) {
    // TODI: add crosscheck with var type.
    *m_os << "# elements: " << count << std::endl;
    *m_os << "# length: " << length << std::endl;
}

// Write one scalar value
template<typename T>
void writer_impl::write_one(const T &val) {
    *m_os << ' ' << boost::lexical_cast<std::string>(val);
}

/// Write one complex value in "(<real>,<imag>)" format
template<typename T>
void writer_impl::write_one_complex(const std::complex<T> &val) {
    *m_os << ' ' << "("
          << boost::lexical_cast<std::string>(val.real()) << ','
          << boost::lexical_cast<std::string>(val.imag())
          << ")";
}

template<typename T>
bool writer_impl::write_scalar(const T &val, const std::string name) {
    write_header(SCALAR, name);
    write_one(val);
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}

bool writer_impl::write_string(const std::string &str,
                          const std::string &name) {
    write_header(STRING, name);
    write_strdesc(str.length());
    *m_os << str;
    *m_os << std::endl;
    *m_os << std::endl << std::endl;
    return true;
}


template<typename T>
bool writer_impl::write_complex_scalar(const std::complex<T> &val,
                                  const std::string &name) {
    write_header(COMPLEX_SCALAR, name);
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
        write_header(COMPLEX_VECTOR, name);
        write_dims(1, vect.size());

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
        write_header(COMPLEX_COVECTOR, name);
        write_dims(vect.size(), 1);

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
        write_header(COMPLEX_MATRIX, name);
        write_dims(mat.size(), mat[0].size());
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
bool writer_impl::write_covector(const std::vector<T> &vect,
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
bool writer_impl::write_matrix(const std::vector<std::vector<T> > &mat,
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

class reader_impl {
private:
    std::size_t count;
    std::string header;
    std::string var_name;
    octave_type var_type;
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

    octave_type next_type() {
        if (valid) {
            return var_type;
        } else {
            return INVALID;
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
    if (var_type == SCALAR) {
        read_one(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool  reader_impl::read_complex_scalar(std::complex<T> &val) {
    if (var_type == COMPLEX_SCALAR) {
        read_one_complex(*m_is, val);
        read_header();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool  reader_impl::read_vector(std::vector<T> &vect) {
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
bool  reader_impl::read_complex_vector(std::vector<std::complex<T>> &vect) {
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
bool  reader_impl::read_matrix(std::vector<std::vector<T> > &matrix) {
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
bool  reader_impl::read_complex_matrix(std::vector<std::vector<std::complex<T>>>
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

octave_type reader::next_type() {return impl->next_type();}

std::string reader::next_name() {return impl->next_name();}

bool reader::skip_one() {return impl->skip_one();}

template<typename T>
bool reader::read(T &var) {return impl->template read<T>(var);}

}
}
