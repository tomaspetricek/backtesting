//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_TRADE_HPP
#define EMASTRATEGY_TRADE_HPP

#include <vector>
#include <cmath>

#include <trading/position.hpp>
#include <trading/fraction.hpp>
#include <trading/amount_t.hpp>

namespace trading {
    class trade {
        std::vector<position> opened_;
        std::vector<position> closed_;
        amount_t size_{0.0};
        amount_t total_sold_{0.0};
        amount_t total_bought_{0.0};
        bool is_opened_ = true;

    public:
        explicit trade(const position& pos)
        {
            add_opened(pos);
        }

        void add_opened(const position& pos)
        {
            if (!is_opened_)
                throw std::runtime_error("Cannot add position to closed trade");

            size_ += pos.bought();
            total_sold_ += pos.sold();
            opened_.emplace_back(pos);
        }

        void add_closed(const position& pos)
        {
            if (pos.sold()>size_)
                throw std::logic_error("Cannot add closed position. Position size is greater than trade size.");

            size_ -= pos.sold();
            total_bought_ += pos.bought();
            closed_.emplace_back(pos);

            // check if closed
            if (size_==amount_t{0}) is_opened_ = false;
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
    };
}

#endif //EMASTRATEGY_TRADE_HPP
