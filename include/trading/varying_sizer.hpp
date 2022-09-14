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
    template<std::size_t n_open_amounts, std::size_t n_close_fracs>
    class varying_sizer {
        std::array<amount_t, n_open_amounts> open_amounts_;
        std::array<fraction, n_close_fracs> close_fracs_;

        struct state {
            index_t open_idx{0};
            index_t close_idx{0};
        };

        state curr_;

        static amount_t validate_pos_size(amount_t pos_size)
        {
            if (pos_size<=amount_t{0.0})
                throw std::invalid_argument("Open amount has to be greater than 0");

            return pos_size;
        }

    public:
        void reset_state()
        {
            curr_ = state{};
        }

        amount_t open_amount()
        {
            assert(curr_.open_idx<n_open_amounts);
            return open_amounts_[curr_.open_idx++];
        }

        amount_t close_amount(const amount_t& pos_size)
        {
            assert(curr_.close_idx<n_close_fracs);
            return amount_t{static_cast<double>(close_fracs_[curr_.close_idx++])*value_of(pos_size)};
        }

        explicit varying_sizer(const std::array<amount_t, n_open_amounts>& open_amounts,
                const std::array<fraction, n_close_fracs>& close_fracs)
                :open_amounts_(open_amounts), close_fracs_(close_fracs) { }

        varying_sizer() = default;
    };
}

#endif //BACKTESTING_VARYING_SIZER_HPP
