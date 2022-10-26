//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_TEST_FUTURES_POSITION_HPP
#define BACKTESTING_TEST_FUTURES_POSITION_HPP

#include <trading/binance/futures/position.hpp>
#include <boost/test/unit_test.hpp>

using namespace trading;

BOOST_AUTO_TEST_SUITE(futures_position_test)
    BOOST_AUTO_TEST_CASE(no_leverage_test)
    {
        std::size_t leverage{1};
        price_t open{9'414.21};
        binance::futures::long_position pos{trade{amount_t{1159.0478454228034}, open, ptime()}, leverage};
        pos.add_close(trade{pos.size(), price_t{9'693.60}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), 34.40, 0.01);
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<percent_t>())*100, 2.97, 0.1);

        open = price_t{45'582.47};
        pos = binance::futures::long_position{trade{amount_t{value_of(open)*0.03307}, open, ptime()}, leverage};
        open = price_t{44'112.07};
        pos.add_open(trade{amount_t{value_of(open)*0.03307}, open, ptime()});
        pos.add_close(trade{pos.size(), price_t{46'656.07}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), 119.7, 1);

        open = price_t{6'850.51};
        pos = binance::futures::long_position{trade{amount_t{value_of(open)*0.17046}, open, ptime()}, leverage};
        open = price_t{6'629.53};
        pos.add_open(trade{amount_t{value_of(open)*0.17046}, open, ptime()});
        open = price_t{6'261.22};
        pos.add_open(trade{amount_t{value_of(open)*0.34092}, open, ptime()});
        open = price_t{5'892.91};
        pos.add_open(trade{amount_t{value_of(open)*0.68185}, open, ptime()});
        pos.add_close(trade{pos.size(), price_t{6'074.48}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), -166.85, 1);
    }

    BOOST_AUTO_TEST_CASE(leverage_test)
    {
        std::size_t leverage{10};
        price_t open{7'176.47};
        binance::futures::long_position pos{
                trade{amount_t{value_of(open)*(1.74815/static_cast<double>(leverage))}, open, ptime()}, leverage};
        pos.add_close(trade{pos.size(), price_t{7'242.66}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), 115.28, 1);
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<percent_t>())*100, 0.92*leverage, 1);

        open = price_t{9'748.24};
        pos = binance::futures::long_position{
                trade{amount_t{value_of(open)*(1.33945/static_cast<double>(leverage))}, open, ptime()}, leverage};
        open = price_t{9'443.61};
        pos.add_open(trade{amount_t{value_of(open)*(1.33945/static_cast<double>(leverage))}, open, ptime()});
        pos.add_close(trade{pos.size(), price_t{9'627.19}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), 83.75, 1);

        open = price_t{7'099.12};
        pos = binance::futures::long_position{
                trade{amount_t{value_of(open)*(2.04744/static_cast<double>(leverage))}, open, ptime()}, leverage};
        open = price_t{6'877.28};
        pos.add_open(trade{amount_t{value_of(open)*(2.04744/static_cast<double>(leverage))}, open, ptime()});
        open = price_t{6'655.43};
        pos.add_open(trade{amount_t{value_of(open)*(4.09488/static_cast<double>(leverage))}, open, ptime()});
        open = price_t{6'285.68};
        pos.add_open(trade{amount_t{value_of(open)*(8.18976/static_cast<double>(leverage))}, open, ptime()});
        pos.add_close(trade{pos.size(), price_t{6'038.32}, ptime()});
        BOOST_REQUIRE_CLOSE(value_of(pos.total_realized_profit<amount_t>()), -8'447.039999999999, 1);
    }

    BOOST_AUTO_TEST_CASE(loss_test) {
        std::size_t leverage{1};
        binance::futures::long_position pos{trade{amount_t{100}, price_t{10'000}, ptime()}, leverage};
        pos.add_close(trade{pos.size(), price_t{5'000}, ptime()});
        BOOST_REQUIRE_EQUAL(value_of(pos.total_realized_profit<percent_t>()), -0.5);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_FUTURES_POSITION_HPP
