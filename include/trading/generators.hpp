//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_GENERATORS_HPP
#define BACKTESTING_GENERATORS_HPP

#include <array>
#include <trading/types.hpp>
#include <cppcoro/recursive_generator.hpp>

namespace trading {
    // generates unique subsequent fractions in interval (0, max), such as max is in interval (0.0, 1.0]
    template<std::size_t n_levels>
    class levels_generator {
        static_assert(n_levels>=1);
        constexpr static std::size_t step_ = 1;
        constexpr static fraction_t default_max_{1.0};
        std::array<fraction_t, n_levels> levels_;
        std::size_t n_fracs_;
        fraction_t max_;
        using generator_type = cppcoro::recursive_generator<std::array<fraction_t, n_levels>>;

        template<std::size_t depth = n_levels>
        requires (depth==n_levels)
        generator_type generate(std::size_t prev) { co_yield levels_; }

        template<std::size_t depth = 0>
        requires (depth<n_levels)
        generator_type generate(std::size_t prev = 0)
        {
            for (std::size_t level{prev+step_}; level<n_fracs_+(-n_levels+1+depth)*step_; level += step_) {
                std::get<depth>(levels_) = fraction_t(static_cast<double>(level)/n_fracs_)*max_;
                co_yield generate<depth+1>(level);
            }
        }

        const fraction_t& validate_max(const fraction_t& max)
        {
            if (max<=fraction_t{0.0} || max>default_max_)
                throw std::invalid_argument{"Maximum has to be in interval (0.0, 1.0]"};
            return max;
        }

    public:
        explicit levels_generator(size_t n_unique_fracs, const fraction_t& max = default_max_)
                :n_fracs_(n_unique_fracs), max_(validate_max(max)) { }

        generator_type operator()()
        {
            co_yield generate();
        }
    };
}

#endif //BACKTESTING_GENERATORS_HPP
