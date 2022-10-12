//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_VARYING_SIZER_HPP
#define BACKTESTING_VARYING_SIZER_HPP

#include <array>
#include <trading/amount_t.hpp>
#include <trading/fraction.hpp>
#include <trading/index_t.hpp>

namespace trading {
    template<std::size_t n_open_fracs, std::size_t n_close_fracs>
    class varying_sizer {
        std::array<percent_t, n_open_fracs> cum_open_fracs_; // cum -> cumulative
        std::array<percent_t, n_close_fracs> cum_close_fracs_;

        struct state {
            index_t open_idx{0};
            index_t close_idx{0};
        };

        state curr_;

        template<std::size_t size>
        std::array<percent_t, size> calc_cumulative_fractions(const std::array<percent_t, size>& fracs)
        {
            std::array<percent_t, size> cum_fracs;
            cum_fracs[0] = fracs[0];
            percent_t sum_prev{fracs[0]};

            for (index_t i{1}; i<size; i++) {
                cum_fracs[i] = sum_prev/(percent_t{1.0}-sum_prev);
                sum_prev += fracs[i];
            }

            assert(cum_fracs[size-1]==percent_t{1.0});
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
        void reset_state()
        {
            curr_ = state{};
        }

        amount_t open_amount(const amount_t& curr_balance)
        {
            assert(curr_.open_idx<n_open_fracs);
            return amount_t{value_of(cum_open_fracs_[curr_.open_idx++])*value_of(curr_balance)};
        }

        amount_t close_amount(const amount_t& curr_pos_size)
        {
            assert(curr_.close_idx<n_close_fracs);
            return amount_t{value_of(cum_close_fracs_[curr_.close_idx++])*value_of(curr_pos_size)};
        }

        explicit varying_sizer(const std::array<percent_t, n_open_fracs>& open_frac,
                const std::array<percent_t, n_close_fracs>& close_frac)
                :cum_open_fracs_(calc_cumulative_fractions(validate_fractions(open_frac))),
                 cum_close_fracs_(calc_cumulative_fractions(validate_fractions(close_frac))) { }

        varying_sizer() = default;
    };
}

#endif //BACKTESTING_VARYING_SIZER_HPP
