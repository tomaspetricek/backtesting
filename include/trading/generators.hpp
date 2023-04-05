//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_GENERATORS_HPP
#define BACKTESTING_GENERATORS_HPP

#include <array>
#include <numeric>
#include <random>
#include <utility>
#include <tuple>
#include <cppcoro/recursive_generator.hpp>
#include <etl/flat_set.h>
#include <fmt/format.h>
#include <trading/types.hpp>
#include <trading/tuple.hpp>

namespace trading {
    template<std::size_t n_sizes>
    class sizes_generator {
    protected:
        static_assert(n_sizes>1);
        std::array<fraction_t, n_sizes> sizes_;
        std::size_t denom_;
        std::size_t max_num_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (!n_unique)
                throw std::invalid_argument{"The number of unique sizes has to be greater than zero"};
            return n_unique;
        }

        explicit sizes_generator(std::size_t n_unique)
                :denom_(n_sizes+validate_n_unique(n_unique)-1), max_num_(denom_-n_sizes+1) { }

    public:
        using value_type = std::array<fraction_t, n_sizes>;

        std::size_t unique_count()
        {
            return denom_-n_sizes+1;
        }
    };

    template<std::size_t n_levels>
    class levels_generator {
        static_assert(n_levels>0);
    protected:
        std::array<fraction_t, n_levels> levels_;
        std::size_t denom_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (n_unique<n_levels)
                throw std::invalid_argument{
                        "The number of unique fractions must be greater than or equal to the number of levels"};
            return n_unique;
        }

        explicit levels_generator(size_t n_unique = n_levels)
                :denom_(validate_n_unique(n_unique)+1) { }

    public:
        using value_type = std::array<fraction_t, n_levels>;

        std::size_t unique_count()
        {
            return denom_-1;
        }
    };
}

#endif //BACKTESTING_GENERATORS_HPP
