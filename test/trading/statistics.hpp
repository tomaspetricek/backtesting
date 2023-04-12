//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TEST_STATISTICS_HPP
#define BACKTESTING_TEST_STATISTICS_HPP

#include <boost/test/unit_test.hpp>
#include <random>
#include <trading/statistics.hpp>
#include <trading/types.hpp>
#include <trading/random/generators.hpp>
#include <trading/motion_tracker.hpp>
#include <limits>

BOOST_AUTO_TEST_SUITE(statistics_test)
    double nan{0.0/0.0};

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::statistics{-10'000}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        trading::amount_t init_balance{10'000};
        trading::statistics stats{init_balance};
        BOOST_REQUIRE_EQUAL(stats.init_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.final_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::percent>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.total_open_orders(), 0);
        BOOST_REQUIRE_EQUAL(stats.total_close_orders(), 0);

        BOOST_REQUIRE_EQUAL(stats.min_equity(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.max_equity(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.max_equity_drawdown<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_equity_drawdown<trading::percent>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_equity_run_up<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_equity_run_up<trading::percent>(), 0.0);

        BOOST_REQUIRE_EQUAL(stats.min_close_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.max_close_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.max_close_balance_drawdown<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_close_balance_drawdown<trading::percent>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_close_balance_run_up<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.max_close_balance_run_up<trading::percent>(), 0.0);

        BOOST_REQUIRE_EQUAL(stats.gross_profit(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.gross_loss(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.net_profit(), 0.0);

        // nans
        BOOST_REQUIRE(stats.profit_factor()!=nan);
        BOOST_REQUIRE(stats.order_ratio()!=nan);
    }

    BOOST_AUTO_TEST_CASE(increase_order_counters_test)
    {
        trading::amount_t init_balance{10'000};
        trading::statistics stats{init_balance};
        std::size_t expect_open_order_count = 0, expect_close_order_count = 0;
        std::mt19937 gen{std::random_device{}()};
        auto coin_flip = std::uniform_int_distribution<int>(0, 1);
        BOOST_REQUIRE_EQUAL(stats.total_open_orders(), expect_open_order_count);
        BOOST_REQUIRE_EQUAL(stats.total_close_orders(), expect_close_order_count);
        BOOST_REQUIRE(stats.order_ratio()!=static_cast<double>(expect_open_order_count)/expect_close_order_count);

        for (std::size_t i{0}; i<10; i++) {
            if (coin_flip(gen)) {
                stats.increase_total_open_orders();
                expect_open_order_count++;
            }
            else {
                stats.increase_total_close_orders();
                expect_close_order_count++;
            }
            BOOST_REQUIRE_EQUAL(stats.total_open_orders(), expect_open_order_count);
            BOOST_REQUIRE_EQUAL(stats.total_close_orders(), expect_close_order_count);
            BOOST_REQUIRE(stats.order_ratio()==static_cast<double>(expect_open_order_count)/expect_close_order_count);
        }
    }

    BOOST_AUTO_TEST_CASE(profit_test)
    {
        trading::random::real_interval_generator<amount_t> rand_profit{-1'000, 1'000};
        trading::statistics stats{10'000};
        amount_t profit, expect_gross_profit{0.0}, expect_gross_loss{0.0};
        BOOST_REQUIRE_EQUAL(stats.gross_profit(), expect_gross_profit);
        BOOST_REQUIRE_EQUAL(stats.gross_loss(), expect_gross_loss);

        for (std::size_t i{0}; i<100; i++) {
            // check
            BOOST_REQUIRE_EQUAL(stats.gross_profit(), expect_gross_profit);
            BOOST_REQUIRE_EQUAL(stats.gross_loss(), expect_gross_loss);

            if (stats.gross_loss()==amount_t{0.0}) {
                if (stats.gross_profit()==amount_t{0.0}) {
                    BOOST_REQUIRE(stats.profit_factor()!=nan);
                }
                else {
                    BOOST_REQUIRE(stats.profit_factor()==std::numeric_limits<amount_t>::infinity());
                }
            }
            else {
                BOOST_REQUIRE_EQUAL(stats.profit_factor(), expect_gross_profit/std::abs(expect_gross_loss));
            }

            // update
            profit = rand_profit();
            stats.update_profit(profit);
            if (profit<0) expect_gross_loss -= profit;
            else expect_gross_profit += profit;
        }
    }

    BOOST_AUTO_TEST_CASE(final_balance_test)
    {
        amount_t init_balance{10'000}, final_balance{15'000};
        trading::statistics stats{10'000};
        BOOST_REQUIRE_EQUAL(stats.init_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.final_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::amount>(), 0.0);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::percent>(), 0.0);

        stats.set_final_balance(final_balance);
        BOOST_REQUIRE_EQUAL(stats.init_balance(), init_balance);
        BOOST_REQUIRE_EQUAL(stats.final_balance(), final_balance);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::amount>(), final_balance-init_balance);
        BOOST_REQUIRE_EQUAL(stats.total_profit<trading::percent>(), ((final_balance-init_balance)/init_balance)*100);
    }

    BOOST_AUTO_TEST_CASE(equity_test)
    {
        amount_t init_balance{10'000}, expect_min{init_balance}, expect_max{init_balance}, equity{init_balance};
        trading::drawdown_tracker expect_drawdown{init_balance};
        trading::run_up_tracker expect_run_up{init_balance};
        trading::statistics stats{init_balance};
        trading::random::real_interval_generator<amount_t> rand_profit{-1'000, 1'000};

        for (std::size_t i{0}; i<100; i++) {
            // check
            BOOST_REQUIRE_EQUAL(stats.min_equity(), expect_min);
            BOOST_REQUIRE_EQUAL(stats.max_equity(), expect_max);
            BOOST_REQUIRE_EQUAL(stats.max_equity_drawdown<trading::amount>(),
                    expect_drawdown.max().value<trading::amount>());
            BOOST_REQUIRE_EQUAL(stats.max_equity_drawdown<trading::percent>(),
                    expect_drawdown.max().value<trading::percent>());
            BOOST_REQUIRE_EQUAL(stats.max_equity_run_up<trading::amount>(),
                    expect_run_up.max().value<trading::amount>());
            BOOST_REQUIRE_EQUAL(stats.max_equity_run_up<trading::percent>(),
                    expect_run_up.max().value<trading::percent>());

            // update
            equity += rand_profit();
            stats.update_equity(equity);
            expect_drawdown.update(equity);
            expect_run_up.update(equity);
            if (equity<expect_min) expect_min = equity;
            if (equity>expect_max) expect_max = equity;
        }
    }

    BOOST_AUTO_TEST_CASE(close_balance_test)
    {
        amount_t init_balance{10'000}, expect_min{init_balance}, expect_max{init_balance}, close_balance{init_balance};
        trading::drawdown_tracker expect_drawdown{init_balance};
        trading::run_up_tracker expect_run_up{init_balance};
        trading::statistics stats{init_balance};
        trading::random::real_interval_generator<amount_t> rand_profit{-1'000, 1'000};

        for (std::size_t i{0}; i<100; i++) {
            // check
            BOOST_REQUIRE_EQUAL(stats.min_close_balance(), expect_min);
            BOOST_REQUIRE_EQUAL(stats.max_close_balance(), expect_max);
            BOOST_REQUIRE_EQUAL(stats.max_close_balance_drawdown<trading::amount>(),
                    expect_drawdown.max().value<trading::amount>());
            BOOST_REQUIRE_EQUAL(stats.max_close_balance_drawdown<trading::percent>(),
                    expect_drawdown.max().value<trading::percent>());
            BOOST_REQUIRE_EQUAL(stats.max_close_balance_run_up<trading::amount>(),
                    expect_run_up.max().value<trading::amount>());
            BOOST_REQUIRE_EQUAL(stats.max_close_balance_run_up<trading::percent>(),
                    expect_run_up.max().value<trading::percent>());

            // update
            close_balance += rand_profit();
            stats.update_close_balance(close_balance);
            expect_drawdown.update(close_balance);
            expect_run_up.update(close_balance);
            if (close_balance<expect_min) expect_min = close_balance;
            if (close_balance>expect_max) expect_max = close_balance;
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_STATISTICS_HPP
