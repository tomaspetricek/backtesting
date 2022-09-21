//
// Created by Tomáš Petříček on 21.09.2022.
//

#ifndef BACKTESTING_FEE_CHARGER_HPP
#define BACKTESTING_FEE_CHARGER_HPP

#include "percent_t.hpp"
#include "amount_t.hpp"

namespace trading {
    class fee_charger {
        percent_t open_fee_{0.0};
        percent_t close_fee_{0.0};

        inline static amount_t apply_fee(amount_t amount, percent_t fee)
        {
            return amount_t{value_of(amount)*(1.0-value_of(fee))};
        }

    public:
        constexpr explicit fee_charger(percent_t open_fee, percent_t close_fee)
                :open_fee_{open_fee}, close_fee_{close_fee} { }

        fee_charger() = default;

        inline amount_t apply_open_fee(amount_t invested) const
        {
            return apply_fee(invested, open_fee_);
        }

        inline amount_t apply_close_fee(amount_t received) const
        {
            return apply_fee(received, close_fee_);
        }
    };
}

#endif //BACKTESTING_FEE_CHARGER_HPP
