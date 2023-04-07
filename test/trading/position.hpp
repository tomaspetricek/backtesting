//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_TEST_POSITION_HPP
#define BACKTESTING_TEST_POSITION_HPP

#include <boost/test/unit_test.hpp>
#include <trading/position.hpp>
#include <trading/order.hpp>

BOOST_AUTO_TEST_SUITE(position_test)
    auto default_fee = trading::position::default_fee;

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        trading::position position;
        BOOST_REQUIRE_EQUAL(position.open_fee(), default_fee);
        BOOST_REQUIRE_EQUAL(position.close_fee(), default_fee);
        BOOST_REQUIRE_EQUAL(position.size(), 0);
        BOOST_REQUIRE_EQUAL(position.total_invested(), 0);
        BOOST_REQUIRE_EQUAL(position.total_realized_profit<amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(position.current_value(1'000), 0.0);
    }

    void check_open_position(trading::position& position, trading::amount_t expect_total_invested, trading::amount_t expect_size, trading::price_t curr_price) {
        BOOST_REQUIRE_EQUAL(position.size(), expect_size);
        BOOST_REQUIRE_EQUAL(position.total_invested(), expect_total_invested);
        BOOST_REQUIRE_EQUAL(position.total_realized_profit<amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(position.total_realized_profit<percent>(), 0.0);
        BOOST_REQUIRE_EQUAL(position.current_value(curr_price), expect_size*curr_price);
        BOOST_REQUIRE_EQUAL(position.current_profit<amount>(curr_price),
                (expect_size*curr_price)-expect_total_invested);
        BOOST_REQUIRE_EQUAL(position.current_profit<percent>(curr_price),
                (((expect_size*curr_price)-expect_total_invested)/expect_total_invested)*100);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        // open position
        trading::open_order open{1'000, 50, 0};
        trading::amount_t expect_total_invested = open.sold;
        trading::amount_t expect_size = open.sold/open.price;

        trading::position position{open};
        BOOST_REQUIRE_EQUAL(position.open_fee(), default_fee);
        BOOST_REQUIRE_EQUAL(position.close_fee(), default_fee);
        check_open_position(position, expect_total_invested, expect_size, 2'500);
        check_open_position(position, expect_total_invested, expect_size, 1'000);
        check_open_position(position, expect_total_invested, expect_size, 500);

        // increase position
        trading::open_order increase{5'000, 600, 0};
        expect_total_invested += increase.sold;
        expect_size += increase.sold/increase.price;

        position.increase(increase);
        check_open_position(position, expect_total_invested, expect_size, 10'000);
        check_open_position(position, expect_total_invested, expect_size, 5'000);
        check_open_position(position, expect_total_invested, expect_size, 500);

        // close position
        trading::close_all_order close_all{3'500, 0};
        position.close_all(close_all);

        BOOST_REQUIRE_EQUAL(position.total_invested(), expect_total_invested);
        BOOST_REQUIRE_EQUAL(position.total_realized_profit<amount>(), (expect_size*close_all.price)-expect_total_invested);
        BOOST_REQUIRE_EQUAL(position.total_realized_profit<percent>(), (((expect_size*close_all.price)-expect_total_invested)/expect_total_invested)*100);
        expect_size = 0;
        price_t curr_price{6'000};
        BOOST_REQUIRE_EQUAL(position.size(), expect_size);
        BOOST_REQUIRE_EQUAL(position.current_value(curr_price), expect_size*curr_price);
    }

    BOOST_AUTO_TEST_CASE(comparison_test)
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

    BOOST_AUTO_TEST_CASE(loss_test)
    {
        trading::position pos{trading::open_order{100, 10'000, 0}};
        pos.close_all(trading::close_all_order{5'000, 0});
        BOOST_REQUIRE_EQUAL(pos.total_realized_profit<trading::percent>(), -50.0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_POSITION_HPP
