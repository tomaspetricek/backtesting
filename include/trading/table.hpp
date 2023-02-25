//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_TABLE_HPP
#define BACKTESTING_TABLE_HPP

#include <vector>
#include <tuple>
#include <iostream>

namespace trading {
    template<class Indices, class ...Types>
    class table final {
    public:
        typedef std::tuple<Types...> row_type;
        using indices = Indices;
        static constexpr std::size_t column_count = sizeof...(Types);

    private:
        std::vector<row_type> rows_;

    public:
        table() = default;

        void clear()
        {
            rows_.clear();
        }

        void emplace_back(const table::row_type& row)
        {
            rows_.emplace_back(row);
        }

        auto begin() const
        {
            return rows_.begin();
        }

        auto end() const
        {
            return rows_.end();
        }
    };
}

#endif //BACKTESTING_TABLE_HPP
