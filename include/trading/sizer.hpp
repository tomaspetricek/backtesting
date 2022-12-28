//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_SIZER_HPP
#define BACKTESTING_SIZER_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/utils.hpp>

namespace trading {
    template<std::size_t n_fracs>
    class sizer {
        std::array<fraction_t, n_fracs> cum_fracs_;
        index_t curr_idx{0};

        auto calc_cumulative_fractions(const std::array<fraction_t, n_fracs>& fracs)
        {
            std::array<fraction_t, n_fracs> cum_fracs;
            cum_fracs[0] = fracs[0];
            fraction_t sum_prev{fracs[0]};

            for (index_t i{1}; i<n_fracs-1; i++) {
                cum_fracs[i] = fracs[i]/(fraction_t{1.0}-sum_prev);
                sum_prev += fracs[i];
            }

            cum_fracs[n_fracs-1] = fraction_t{1.0};
            return cum_fracs;
        }

        template<std::size_t size>
        std::array<fraction_t, size> validate_fractions(const std::array<fraction_t, size>& fracs)
        {
            fraction_t sum{0.0};

            for (index_t i{0}; i<size; i++) {
                if (fracs[i]<=fraction_t{0} || fracs[i]>fraction_t{1})
                    throw std::invalid_argument("Fraction has to be in interval (0, 1]");
                sum += fracs[i];
            }

            if (!is_close(1.0, sum))
                throw std::invalid_argument("Fractions have to sum up to 1.0");

            return fracs;
        }

    public:
        void reset()
        {
            curr_idx = 0;
        }

        amount_t operator()(amount_t rest)
        {
            assert(curr_idx<n_fracs);
            return cum_fracs_[curr_idx++]*rest;
        }

        explicit sizer(const std::array<fraction_t, n_fracs>& fracs)
                :cum_fracs_(calc_cumulative_fractions(validate_fractions(fracs))) { }

        sizer() = default;
    };
}

#endif //BACKTESTING_SIZER_HPP
