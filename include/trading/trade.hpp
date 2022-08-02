//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_TRADE_HPP
#define EMASTRATEGY_TRADE_HPP

#include <vector>
#include <cmath>

#include <trading/position.hpp>
#include <trading/fraction.hpp>
#include <trading/open_position.hpp>
#include <trading/close_position.hpp>

namespace trading {
    template<currency::crypto base, currency::crypto quote>
    class trade {
        std::vector<open_position<base, quote>> opened_;
        std::vector<close_position<base, quote>> closed_;
        amount<base> size_{0};
        amount<quote> total_sold_{0};
        amount<quote> total_bought_{0};
        bool is_opened_ = true;

    public:
        explicit trade(const open_position<base, quote>& pos)
        {
            add_opened(pos);
        }

        void add_opened(const open_position<base, quote>& pos)
        {
            if (!is_opened_)
                throw std::runtime_error("Cannot add position to closed trade");

            size_ += pos.bought();
            total_sold_ += pos.sold();
            opened_.emplace_back(pos);
        }

        void add_closed(const close_position<base, quote>& pos)
        {
            if (pos.sold()>size_)
                throw std::logic_error("Cannot add closed position. Position size is greater than trade size.");

            size_ -= pos.sold();
            total_bought_ += pos.bought();
            closed_.emplace_back(pos);

            // check if closed
            if (size_==0) is_opened_ = false;
        }

        const amount<base>& size() const
        {
            return size_;
        }

//        template<unsigned int denom>
//        double calculate_position_size(const fraction<denom>& trade_frac) const
//        {
//            return size_*static_cast<double>(trade_frac);
//        }

        bool is_closed() const
        {
            return !is_opened_;
        }

        const amount<quote>& total_sold() const
        {
            return total_sold_;
        }
        const amount<quote>& total_bought() const
        {
            return total_bought_;
        }
    };
}

#endif //EMASTRATEGY_TRADE_HPP
