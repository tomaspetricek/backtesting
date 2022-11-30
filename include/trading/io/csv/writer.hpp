//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_CSV_WRITER_HPP
#define BACKTESTING_CSV_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <utility>
#include "trading/table.hpp"
#include <trading/io/stringifier.hpp>

namespace trading::io::csv {
    class writer final {
        std::ofstream file_;
        const std::filesystem::path path_;
        const char delim_;
        std::ios_base::openmode mode_{std::ios_base::out};
        stringifier stringifier_;

        template<std::size_t n_cols>
        void write_value(std::size_t i, const std::string& val) {
            char sep = (i<n_cols-1) ? delim_ : '\n';
            file_ << val << sep;
        }

    public:
//        using row_type = std::tuple<Types...>;

        explicit writer(const std::filesystem::path& path, char delim = ',')
                :path_(path), delim_(delim)
        {
            file_ = std::ofstream{path.string(), mode_};

            if (!file_.is_open())
                throw std::runtime_error("Cannot open "+path.string());
        }

        template<std::size_t n_cols>
        void write_header(const std::array<std::string, n_cols>& header) {
            for(std::size_t i{0}; i<header.size(); i++)
                write_value<n_cols>(i, header[i]);
        }

        template<class ...Types>
        void write_row(const Types& ...outputs)
        {
            std::size_t i{0};
            constexpr std::size_t n_cols = sizeof...(Types);

            auto write_line = [&](auto& out){
                write_value<n_cols>(i++, stringifier_.to_string(out));
            };

            (write_line(outputs), ...);
        }
    };
}

#endif //BACKTESTING_CSV_WRITER_HPP
