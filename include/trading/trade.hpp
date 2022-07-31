//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_TRADE_HPP
#define EMASTRATEGY_TRADE_HPP

#include <vector>
#include <cmath>

#include <trading/position.hpp>
#include <trading/fraction.hpp>

namespace trading {
    class trade {
        std::vector<position> opened_;
        std::vector<position> closed_;
        std::size_t size_ = 0;
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

            size_ += pos.size();
            opened_.emplace_back(pos);
        }

        void add_closed(const position& pos)
        {
            if (pos.size()>size_)
                throw std::logic_error("Cannot add closed position. Position size is greater than trade size.");

            size_ -= pos.size();
            closed_.emplace_back(pos);

            // check if closed
            if (size_==0) is_opened_ = false;
        }

        std::size_t size() const
        {
            return size_;
        }

        template<unsigned int denom>
        std::size_t calculate_position_size(const fraction<denom>& trade_frac) const
        {
            return std::round(size_*static_cast<double>(trade_frac));
        }

        bool is_closed() const
        {
            return !is_opened_;
        }
    };
}

#endif //EMASTRATEGY_TRADE_HPP