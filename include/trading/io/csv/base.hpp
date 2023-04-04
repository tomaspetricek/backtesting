//
// Created by Tomáš Petříček on 04.04.2023.
//

#ifndef BACKTESTING_IO_CSV_BASE_HPP
#define BACKTESTING_IO_CSV_BASE_HPP

#include <array>

namespace trading::io::csv {
    template<std::size_t n_cols, class FileStream>
    class base {
    protected:
        FileStream file_;
        char delim_;
        constexpr static char default_delim = ',';

        explicit base(char delim = default_delim)
                :delim_(delim) { }

    public:
        constexpr static std::size_t column_count = n_cols;
        using file_stream_type = FileStream;
    };
}

#endif //BACKTESTING_IO_CSV_BASE_HPP
