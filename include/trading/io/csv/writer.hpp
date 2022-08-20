//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_CSV_WRITER_HPP
#define BACKTESTING_CSV_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <utility>
#include "trading/table.hpp"

namespace trading::io::csv {
    template<class Output, class ...ColumnTypes>
    class writer final {
        std::ofstream file_;
        const std::filesystem::path path_;
        const char delim_;
        std::ios_base::openmode mode_{std::ios_base::out};
        std::function<std::tuple<ColumnTypes...>(Output)> serializer_;
        static constexpr int n_cols_ = sizeof...(ColumnTypes);

        template<int col_idx = 0>
        void write_row(const std::tuple<ColumnTypes...>& row)
        {
            file_ << std::get<col_idx>(row);

            if (col_idx<n_cols_-1) {
                file_ << delim_;
            }
            else {
                file_ << "\n";
            }

            if constexpr (col_idx+1!=n_cols_) {
                write_row<col_idx+1>(row);
            }
        }

        void write_header(const std::array<std::string, n_cols_>& row)
        {
            for (index_t row_idx{0}; row_idx<row.size(); row_idx++) {
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
        explicit writer(const std::filesystem::path& path,
                const std::function<std::tuple<ColumnTypes...>(Output)>& serializer, char delim = ',')
                :path_(path), delim_(delim), serializer_(serializer) { }

        void operator()(const std::array<std::string, n_cols_>& col_names, const std::vector<Output>& outputs)
        {
            file_ = std::ofstream{path_.string(), mode_};

            if (file_.is_open()) {
                if (mode_!=std::ios_base::app)
                    write_header(col_names);

                for (const auto& out: outputs)
                    write_row(serializer_(out));

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
