//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_CSV_WRITER_HPP
#define BACKTESTING_CSV_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <utility>
#include <trading/io/stringifier.hpp>
#include <trading/io/csv/base.hpp>

namespace trading::io::csv {
    template<std::size_t n_cols>
    class writer final : public base<n_cols,std::ofstream>{
        using base_type = base<n_cols,std::ofstream>;
        std::ios_base::openmode mode_{std::ios_base::out};

        void write_value(std::size_t i, const std::string& val)
        {
            char sep = (i<n_cols-1) ? this->delim_ : '\n';
            this->file_ << val << sep;
        }

    public:
        explicit writer(const std::filesystem::path& path, char delim = base_type::default_delim)
                :base_type{delim}
        {
            this->file_ = typename base_type::file_stream_type{path.string(), mode_};
        }

        void write_header(const std::array<std::string, n_cols>& header)
        {
            for (std::size_t i{0}; i<header.size(); i++)
                write_value(i, header[i]);
        }

        template<class ...Types>
        void write_row(const Types& ...outputs)
        {
            static_assert(sizeof...(Types)==n_cols);

            std::size_t i{0};
            auto write_line = [&](auto& out) {
                write_value(i++, stringifier::to_string(out));
            };

            (write_line(outputs), ...);
        }
    };
}

#endif //BACKTESTING_CSV_WRITER_HPP
