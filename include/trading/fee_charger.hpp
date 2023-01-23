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
            if (fee<fraction_t{0} || fee>fraction_t{1})
                throw std::invalid_argument("Fee has to be in interval: [0.0, 1.0]");
            return fee;
        }

    public:
        explicit fee_charger(fraction_t fee)
                :fee_(validate_fee(fee)) { }

        fee_charger() = default;

        inline amount_t apply_fee(amount_t amount_) const
        {
            return amount_*boost::rational_cast<double>(fraction_t{1}-(fee_));
        }

        fraction_t fee() const
        {
            return fee_;
        }
    };
}

#endif //BACKTESTING_FEE_CHARGER_HPP
