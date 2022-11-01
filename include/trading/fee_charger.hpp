//
// Created by Tomáš Petříček on 21.09.2022.
//

#ifndef BACKTESTING_FEE_CHARGER_HPP
#define BACKTESTING_FEE_CHARGER_HPP

#include <trading/types.hpp>

namespace trading {
    class fee_charger {
        percent_t fee_{0.0};

        static percent_t validate_fee(const percent_t& fee)
        {
            if (fee<percent_t{0.0} || fee>percent_t{1.0})
                throw std::invalid_argument("Fee has to be in interval: [0.0, 1.0]");

            return fee;
        }

    public:
        explicit fee_charger(const percent_t& fee)
                :fee_(validate_fee(fee)) { }

        fee_charger() = default;

        inline amount_t apply_fee(amount_t amount) const
        {
            return amount_t{value_of(amount)*(1.0-value_of(fee_))};
        }

        const percent_t& fee() const
        {
            return fee_;
        }
    };
}

#endif //BACKTESTING_FEE_CHARGER_HPP
