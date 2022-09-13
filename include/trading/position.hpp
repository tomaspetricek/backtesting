//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef BACKTESTING_POSITION_HPP
#define BACKTESTING_POSITION_HPP

#include <ostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/currency.hpp>
#include <trading/price_t.hpp>
#include <trading/currency.hpp>
#include <trading/amount_t.hpp>
#include <trading/trade.hpp>

namespace trading {
    class position {
    protected:
        amount_t size_;
        std::vector<trade> trades_;
        amount_t invested_{0.0};
        bool is_opened_{true};
        amount_t realized_profit_{0.0};

        explicit position(const trade& open)
                :size_{open.bought}, invested_{open.sold}, trades_{{open}} { }

        position() = default;

    public:
        bool is_closed() const
        {
            return !is_opened_;
        }

        const amount_t& size() const
        {
            return size_;
        }

        const amount_t& invested() const
        {
            return invested_;
        }
    };
}

#endif //BACKTESTING_POSITION_HPP
