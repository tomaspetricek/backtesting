//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/price_t.hpp>
#include <trading/wallet.hpp>
#include <trading/order.hpp>
#include <trading/trade.hpp>

using namespace boost::posix_time;

namespace trading {
    template<class Position>
    class market {
    protected:
        std::optional<Position> active_;
        std::vector<Position> closed_;

    public:
        bool has_active_position()
        {
            return active_.has_value();
        }

        Position active_position()
        {
            assert(active_);
            return *active_;
        }

        const std::vector<Position>& closed_positions() const
        {
            return closed_;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
