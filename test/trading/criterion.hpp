//
// Created by Tomáš Petříček on 12.04.2023.
//

#ifndef BACKTESTING_TEST_CRITERION_HPP
#define BACKTESTING_TEST_CRITERION_HPP

#include <boost/test/unit_test.hpp>
#include <trading/criterion.hpp>

BOOST_AUTO_TEST_SUITE(prom_criterion_test)
    struct mock_statistics {
        std::size_t win_count_{0}, loss_count_{0};
        double gross_profit_{0}, gross_loss_{0}, init_balance_;

        std::size_t win_count() const
        {
            return win_count_;
        }

        double gross_profit() const
        {
            return gross_profit_;
        }

        std::size_t loss_count() const
        {
            return loss_count_;
        }

        double gross_loss() const
        {
            return gross_loss_;
        }

        double init_balance() const
        {
            return init_balance_;
        }
    };

    BOOST_AUTO_TEST_CASE(example_1_test)
    {
        auto stats = mock_statistics{};
        stats.gross_profit_ = 25000;
        stats.win_count_ = 49;
        stats.gross_loss_ = 10000;
        stats.loss_count_ = 36;
        stats.init_balance_ = 10'000;
        auto criterion = trading::prom_criterion{};
        BOOST_REQUIRE_CLOSE(criterion(stats), ((21'428.-11'667.)/10'000.)*100, 0.01);
    }

    BOOST_AUTO_TEST_CASE(example_2_test)
    {
        auto stats = mock_statistics{};
        stats.gross_profit_ = 25000;
        stats.win_count_ = 144;
        stats.gross_loss_ = 10000;
        stats.loss_count_ = 121;
        stats.init_balance_ = 10'000;
        auto criterion = trading::prom_criterion{};
        BOOST_REQUIRE_CLOSE(criterion(stats), ((22'916.-10'909.)/10'000.)*100, 0.01);
    }

    BOOST_AUTO_TEST_CASE(example_3_test)
    {
        auto stats = mock_statistics{};
        stats.gross_profit_ = 25000;
        stats.win_count_ = 16;
        stats.gross_loss_ = 10000;
        stats.loss_count_ = 9;
        stats.init_balance_ = 10'000;
        auto criterion = trading::prom_criterion{};
        BOOST_REQUIRE_CLOSE(criterion(stats), ((18'750.-13'333.)/10'000.)*100, 0.01);
    }
BOOST_AUTO_TEST_SUITE_END()
#endif //BACKTESTING_TEST_CRITERION_HPP
