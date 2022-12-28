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
        trading::position pos{order(amount_t{1159.0478454228034}, open, ptime())};
        pos.close(order(pos.size(), price_t{9'693.60}, ptime()));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), 34.40, 0.01);
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<percent>(), 2.97, 0.1);

        open = price_t{45'582.47};
        pos = trading::position{order(open*0.03307, open, ptime())};
        open = price_t{44'112.07};
        pos.increase(order(open*0.03307, open, ptime()));
        pos.close(order(pos.size(), price_t{46'656.07}, ptime()));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), 119.7, 1);

        open = price_t{6'850.51};
        pos = trading::position{trading::order(open*0.17046, open, ptime())};
        open = price_t{6'629.53};
        pos.increase(trading::order(open*0.17046, open, ptime()));
        open = price_t{6'261.22};
        pos.increase(trading::order(open*0.34092, open, ptime()));
        open = price_t{5'892.91};
        pos.increase(trading::order(open*0.68185, open, ptime()));
        pos.close(trading::order(pos.size(), price_t{6'074.48}, ptime()));
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<amount>(), -166.85, 1);
    }

    BOOST_AUTO_TEST_CASE(spot_loss_test)
    {
        trading::position pos{trading::order(amount_t{100}, price_t{10'000}, ptime())};
        pos.close(trading::order(pos.size(), price_t{5'000}, ptime()));
        BOOST_REQUIRE_EQUAL(pos.total_realized_profit<percent>(), -50.0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_POSITION_HPP
