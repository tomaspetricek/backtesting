//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_TEST_POSITION_HPP
#define BACKTESTING_TEST_POSITION_HPP

#include <boost/test/unit_test.hpp>
#include <trading/position.hpp>
#include <trading/order.hpp>

BOOST_AUTO_TEST_SUITE(position_test)
    BOOST_AUTO_TEST_CASE(spot_test)
    {
        trading::price_t open{9'414.21};
        trading::position pos{trading::open_order{1159.0478454228034, open, 0}};
        pos.close_all(trading::close_all_order{9'693.60, 0});
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<trading::amount>(), 34.40, 0.01);
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<trading::percent>(), 2.97, 0.1);

        open = 45'582.47;
        pos = trading::position{trading::open_order{static_cast<trading::amount_t>(open*0.03307), open, 0}};
        open = 44'112.07;
        pos.increase(trading::open_order{static_cast<trading::amount_t>(open*0.03307), open, 0});
        pos.close_all(trading::close_all_order{46'656.07, 0});
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<trading::amount>(), 119.7, 1);

        open = 6'850.51;
        pos = trading::position{trading::open_order{static_cast<trading::amount_t>(open*0.17046), open, 0}};
        open = 6'629.53;
        pos.increase(trading::open_order{static_cast<trading::amount_t>(open*0.17046), open, 0});
        open = 6'261.22;
        pos.increase(trading::open_order{static_cast<trading::amount_t>(open*0.34092), open, 0});
        open = 5'892.91;
        pos.increase(trading::open_order{static_cast<trading::amount_t>(open*0.68185), open, 0});
        pos.close_all(trading::close_all_order{6'074.48, 0});
        BOOST_REQUIRE_CLOSE(pos.total_realized_profit<trading::amount>(), -166.85, 1);
    }

    BOOST_AUTO_TEST_CASE(spot_loss_test)
    {
        trading::position pos{trading::open_order{100, 10'000, 0}};
        pos.close_all(trading::close_all_order{5'000, 0});
        BOOST_REQUIRE_EQUAL(pos.total_realized_profit<trading::percent>(), -50.0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_POSITION_HPP
