//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_CSV_WRITER_HPP
#define BACKTESTING_CSV_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <utility>
#include "trading/table.hpp"

namespace trading::io {
    class cvs_writer final {
        std::ofstream file_;
        const std::filesystem::path path_;
        const char delim_;
        std::ios_base::openmode mode_;

        template<typename T>
        void write_element(T el)
        {
            file_ << el;
        }

        template<typename T>
        void write_element(std::vector<T> vec)
        {
            std::ostringstream oss;

            if (!vec.empty()) {
                std::copy(vec.begin(), vec.end()-1,
                        std::ostream_iterator<T>(oss, " "));

                oss << vec.back();
            }

            file_ << oss.str();
        }

        template<int col_idx = 0, class ...T_s>
        void write_row(const std::tuple<T_s...>& row)
        {
            write_element(file_, std::get<col_idx>(row));

            if (col_idx<sizeof...(T_s)-1) {
                file_ << delim_;
            }
            else {
                file_ << "\n";
            }

            if constexpr(col_idx+1!=sizeof...(T_s)) {
                write_row<col_idx+1>(file_, row);
            }
        }

        template<int N, typename Type>
        void write_row(const std::array<Type, N>& row)
        {
            for (int row_idx{0}; row_idx<row.size(); row_idx++) {
                file_ << row[row_idx];

                if (row_idx<row.size()-1) {
                    file_ << delim_;
                }
                else {
                    file_ << "\n";
                }
            }
        }

    public:
        explicit cvs_writer(std::filesystem::path path, char delim = ',')
                :path_{std::move(path)}, delim_{delim}
        {
            if (std::filesystem::exists(path_.string())) {
                mode_ = std::ios_base::app;
            }
            else {
                mode_ = std::ios_base::out;
            }
        }

        template<class ...T_s>
        void write(const trading::table<T_s...> tab)
        {
            file_ = std::ofstream{path_.string(), mode_};

            if (file_.is_open()) {
                if (mode_!=std::ios_base::app)
                    // write header
                    write_row<tab.n_cols(), std::string>(tab.col_names());

                for (const auto& row : tab.rows())
                    write_row(row);

                file_.close();
            }
            else {
                std::cerr << path_.string() << std::endl;
                throw std::runtime_error("Could not open file");
            }
        }
    };
}

#endif //BACKTESTING_CSV_WRITER_HPP
