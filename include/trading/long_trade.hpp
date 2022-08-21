//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_LONG_TRADE_HPP
#define EMASTRATEGY_LONG_TRADE_HPP

#include <vector>
#include <cmath>

#include <strong_type.hpp>
#include <trading/long_position.hpp>
#include <trading/fraction.hpp>
#include <trading/amount_t.hpp>
#include <trading/fraction.hpp>

namespace trading {
    class long_trade {
        std::vector<long_position> opened_;
        std::vector<long_position> closed_;
        amount_t size_{0.0};
        amount_t total_sold_{0.0};
        amount_t total_bought_{0.0};
        bool is_opened_ = true;

    public:
        explicit long_trade(const long_position& pos)
        {
            add_opened(pos);
        }

        void add_opened(const long_position& pos)
        {
            if (!is_opened_)
                throw std::runtime_error("Cannot add position to closed trade");

            size_ += pos.bought();
            total_sold_ += pos.sold();
            opened_.emplace_back(pos);
        }

        void add_closed(const long_position& pos)
        {
            if (pos.sold()>size_)
                throw std::logic_error("Cannot add closed position. Position size is greater than trade size.");

            size_ -= pos.sold();
            total_bought_ += pos.bought();
            closed_.emplace_back(pos);

            // check if closed
            if (size_==amount_t{0}) is_opened_ = false;
        }

        amount_t calculate_position_size(const fraction& trade_frac)
        {
            return amount_t{value_of(size_)*static_cast<double>(trade_frac)};
        }

        static long_position
        create_open_position(amount_t sold, const price_t& price, const boost::posix_time::ptime& created)
        {
            return long_position::create_open(sold, price, created);
        }

        static long_position
        create_close_position(amount_t sold, const price_t& price, const boost::posix_time::ptime& created)
        {
            return long_position::create_close(sold, price, created);
        }

        const amount_t& size() const
        {
            return size_;
        }

        bool is_closed() const
        {
            return !is_opened_;
        }

        const amount_t& total_sold() const
        {
            return total_sold_;
        }

        const amount_t& total_bought() const
        {
            return total_bought_;
        }

        const std::vector<long_position>& get_open_positions() const
        {
            return opened_;
        }

        const std::vector<long_position>& get_closed_positions() const
        {
            return closed_;
        }
    };
}

#endif //EMASTRATEGY_LONG_TRADE_HPP
