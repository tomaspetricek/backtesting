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
        std::array<percent_t, n_open_fracs> open_fracs_;
        std::array<percent_t, n_close_fracs> close_fracs_;

        struct state {
            index_t open_idx{0};
            index_t close_idx{0};
        };

        state curr_;

    public:
        void reset_state()
        {
            curr_ = state{};
        }

        amount_t open_amount(const amount_t& balance)
        {
            assert(curr_.open_idx<n_open_fracs);
            return amount_t{value_of(open_fracs_[curr_.open_idx++])*value_of(balance)};
        }

        amount_t close_amount(const amount_t& pos_size)
        {
            assert(curr_.close_idx<n_close_fracs);
            return amount_t{value_of(close_fracs_[curr_.close_idx++])*value_of(pos_size)};
        }

        explicit varying_sizer(const std::array<percent_t, n_open_fracs>& open_fracs,
                const std::array<percent_t, n_close_fracs>& close_fracs)
                :open_fracs_(open_fracs), close_fracs_(close_fracs) { }

        varying_sizer() = default;
    };
}

#endif //BACKTESTING_VARYING_SIZER_HPP
