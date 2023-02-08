//
// Created by Tomáš Petříček on 21.09.2022.
//

#ifndef BACKTESTING_FEE_CHARGER_HPP
#define BACKTESTING_FEE_CHARGER_HPP

#include <trading/types.hpp>

namespace trading {
    class fee_charger {
        fraction_t fee_{0};

        static fraction_t validate_fee(fraction_t fee)
        {
            auto denom = fee.denominator();
            if (fee<fraction_t{0, denom} || fee>fraction_t{denom, denom})
                throw std::invalid_argument("Fee has to be in interval: [0.0, 1.0]");
            return fee;
        }

    public:
        explicit fee_charger(fraction_t fee)
                :fee_(validate_fee(fee)) { }

        fee_charger() = default;

        inline amount_t apply_fee(amount_t amount_) const
        {
            auto denom = fee_.denominator();
            return amount_*fraction_cast<amount_t>(fraction_t{denom, denom}-(fee_));
        }

        fraction_t fee() const
        {
            return fee_;
        }
    };
}

#endif //BACKTESTING_FEE_CHARGER_HPP
