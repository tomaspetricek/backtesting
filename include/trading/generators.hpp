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
        std::size_t denom_, max_num_;
        static constexpr std::size_t default_unique_count{1};

        std::size_t validate_unique_count(std::size_t n_unique)
        {
            if (!n_unique)
                throw std::invalid_argument{"The number of unique sizes has to be greater than zero"};
            return n_unique;
        }

        explicit sizes_generator(std::size_t n_unique = default_unique_count)
                :denom_(n_sizes+validate_unique_count(n_unique)-1), max_num_(denom_-n_sizes+1) { }

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
        static constexpr fraction_t default_lower_bound{0, 1};
        std::array<fraction_t, n_levels> levels_;
        std::size_t unique_count_;
        fraction_t lower_bound_{default_lower_bound};

        std::size_t unscaled_denom() const
        {
            return unique_count_+1;
        }

        trading::fraction_t rescale(std::size_t num) const
        {
            return {num*(lower_bound_.denominator()-lower_bound_.numerator())+lower_bound_.numerator()*unscaled_denom(),
                    lower_bound_.denominator()*unscaled_denom()};
        }

        std::size_t validate_unique_count(std::size_t n_unique)
        {
            if (n_unique<n_levels)
                throw std::invalid_argument{
                        "The number of unique fractions must be greater than or equal to the number of levels"};
            return n_unique;
        }

        explicit levels_generator(size_t unique_count = n_levels, const fraction_t& lower_bound = default_lower_bound)
                :unique_count_{validate_unique_count(unique_count)}, lower_bound_(lower_bound) { }

    public:
        using value_type = std::array<fraction_t, n_levels>;

        std::size_t unique_count()
        {
            return unique_count_;
        }

        fraction_t lower_bound() const
        {
            return {lower_bound_.numerator()*unscaled_denom(), lower_bound_.denominator()*unscaled_denom()};
        }
    };
}

#endif //BACKTESTING_GENERATORS_HPP
