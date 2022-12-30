//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_TEST_POSITION_HPP
#define BACKTESTING_TEST_POSITION_HPP

#include <trading/position.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(position_test)
    BOOST_AUTO_TEST_CASE(spot_test)
    {
        price_t open{9'414.21};
        trading::position pos{order(1159.0478454228034, open, 0)};
        pos.close(order(pos.size(), 9'693.60, 0));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), 34.40, 0.01);
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<percent>(), 2.97, 0.1);

        open = 45'582.47;
        pos = trading::position{order(open*0.03307, open, 0)};
        open = 44'112.07;
        pos.increase(order(open*0.03307, open, 0));
        pos.close(order(pos.size(), 46'656.07, 0));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), 119.7, 1);

        open = 6'850.51;
        pos = trading::position{trading::order(open*0.17046, open, 0)};
        open = 6'629.53;
        pos.increase(trading::order(open*0.17046, open, 0));
        open = 6'261.22;
        pos.increase(trading::order(open*0.34092, open, 0));
        open = 5'892.91;
        pos.increase(trading::order(open*0.68185, open, 0));
        pos.close(trading::order(pos.size(), 6'074.48, 0));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), -166.85, 1);
    }

    BOOST_AUTO_TEST_CASE(spot_loss_test)
    {
        trading::position pos{trading::order(100, 10'000, 0)};
        pos.close(trading::order(pos.size(), 5'000, 0));
        BOOST_REQUIRE_EQUAL(pos.total_realized_profit<percent>(), -50.0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_POSITION_HPP
