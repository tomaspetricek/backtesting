//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_FRACTIONER_HPP
#define BACKTESTING_FRACTIONER_HPP

#include <array>
#include <trading/types.hpp>

namespace trading {
    template<std::size_t n_fracs>
    class fractioner {
        std::array<percent_t, n_fracs> cum_fracs_;
        index_t curr_idx{0};

        auto calc_cumulative_fractions(const std::array<percent_t, n_fracs>& fracs)
        {
            std::array<percent_t, n_fracs> cum_fracs;
            cum_fracs[0] = fracs[0];
            percent_t sum_prev{fracs[0]};

            for (index_t i{1}; i<n_fracs-1; i++) {
                cum_fracs[i] = fracs[i]/(percent_t{1.0}-sum_prev);
                sum_prev += fracs[i];
            }

            cum_fracs[n_fracs-1] = percent_t{1.0};
            return cum_fracs;
        }

        template<std::size_t size>
        std::array<percent_t, size> validate_fractions(const std::array<percent_t, size>& fracs)
        {
            percent_t sum{0};

            for (index_t i{0}; i<size; i++) {
                if (fracs[i]<=percent_t{0} || fracs[i]>percent_t{1})
                    throw std::invalid_argument("Fraction has to be in interval (0, 1]");

                sum += fracs[i];
            }

            if (sum!=percent_t{1.0})
                throw std::invalid_argument("Fractions have to sum up to 1.0");

            return fracs;
        }

    public:
        void reset()
        {
            curr_idx = 0;
        }

        amount_t operator()(const amount_t& rest)
        {
            assert(curr_idx<n_fracs);
            return amount_t{value_of(cum_fracs_[curr_idx++])*value_of(rest)};
        }

        explicit fractioner(const std::array<percent_t, n_fracs>& fracs)
                :cum_fracs_(calc_cumulative_fractions(validate_fractions(fracs))) { }

        fractioner() = default;
    };
}

#endif //BACKTESTING_FRACTIONER_HPP
