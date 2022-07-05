//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <vector>
#include <cmath>

#include "position.h"
#include "formula.h"

namespace trading {
    template<typename Currency>
    class trade {
        std::vector<position> opened_;
        std::vector<position> closed_;
        currency::pair<Currency> pair_;
        std::size_t size_ = 0;

    public:
        explicit trade(const currency::pair<Currency>& pair)
                :pair_(pair) { }

        trade(const currency::pair<Currency>& pair, const position& pos)
                :pair_(pair)
        {
            add_opened(pos);
        }

        void add_opened(const position& pos)
        {
            size_ += pos.size();
            opened_.emplace_back(pos);
        }

        void add_closed(const position& pos)
        {
            if (pos.size()>size_)
                throw std::logic_error("Cannot add closed position. Trade size is smaller than position size");

            size_ -= pos.size();
            closed_.emplace_back(pos);
        }

        std::size_t size() const
        {
            return size_;
        }

        std::size_t calculate_position_size(percentage part)
        {
            if (part<=0.0 || part>1.0)
                throw std::invalid_argument("Part has to be between (0.0, 1.0>");

            return std::round(size_*part);
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
