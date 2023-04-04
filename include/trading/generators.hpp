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

    class int_range {
    protected:
        int from_, to_, step_;

        static int validate_step(int step)
        {
            if (step==0) throw std::invalid_argument("Step cannot be zero");
            return step;
        }

        int_range(int from, int to, int step)
                :from_{from}, to_{to}, step_(validate_step(step))
        {
            if (from==to) {
                throw std::invalid_argument("From and to has to be different");
            }
            else if (from>to) {
                if ((from-to)%step!=0)
                    throw std::invalid_argument("Difference between from and to must be multiple of step");

                if (step>0)
                    throw std::invalid_argument("Step has to be lower than 0 if from is greater than to");
            }
            else if (from<to) {
                if ((to-from)%step!=0)
                    throw std::invalid_argument("Difference between from and to must be multiple of step");

                if (step<0)
                    throw std::invalid_argument("Step has to be greater than 0 if from is lower than to");
            }
        }

    public:
        using value_type = int;

        int step() const
        {
            return step_;
        }

        int from() const
        {
            return from_;
        }

        int to() const
        {
            return to_;
        }
    };
}

#endif //BACKTESTING_GENERATORS_HPP
