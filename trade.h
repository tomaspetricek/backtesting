//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <vector>

#include "position.h"

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
    };
}

#endif //EMASTRATEGY_TRADE_H
