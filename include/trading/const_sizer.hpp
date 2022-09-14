//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_CONST_SIZER_HPP
#define BACKTESTING_CONST_SIZER_HPP

#include <trading/amount_t.hpp>
#include <trading/fraction.hpp>

namespace trading {
    class const_sizer {
        amount_t open_amount_;
        fraction close_frac_;

        static amount_t validate_open_amount(amount_t pos_size)
        {
            if (pos_size<=amount_t{0.0})
                throw std::invalid_argument("Buy size has to be greater than 0");

            return pos_size;
        }

    public:
        void reset_state() { }

        amount_t open_amount()
        {
            return open_amount_;
        }

        amount_t close_amount(const amount_t pos_size)
        {
            return amount_t{static_cast<double>(close_frac_)*value_of(pos_size)};
        }

        explicit const_sizer(const amount_t& open_amount, const fraction& close_frac)
                :open_amount_(open_amount), close_frac_(close_frac) { }

        const_sizer() = default;
    };
}

#endif //BACKTESTING_CONST_SIZER_HPP
