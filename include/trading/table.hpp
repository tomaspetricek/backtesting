//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_TABLE_HPP
#define BACKTESTING_TABLE_HPP

#include <vector>
#include <tuple>
#include <iostream>

namespace trading {
    template<class ...T_s>
    class table final {
    public:
        typedef std::tuple<T_s...> row;

    private:
        std::vector<row> rows_;
        static constexpr int n_cols_ = sizeof...(T_s);
        const std::array<std::string, n_cols_> col_names_;

    public:
        table() = default;

        explicit table(const std::array<std::string, n_cols_>& col_names)
                :col_names_{col_names}{ }

        void add_row(const table::row& row)
        {
            rows_.emplace_back(row);
        }

        constexpr int n_cols() const
        {
            return n_cols_;
        }

        const std::vector<row>& rows() const
        {
            return rows_;
        }

        const std::array<std::string, n_cols_>& col_names() const
        {
            return col_names_;
        }
    };
}

#endif //BACKTESTING_TABLE_HPP
