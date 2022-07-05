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
        std::vector<position> open_pos_;
        std::vector<position> close_pos_;
        currency::pair<Currency> pair_;
        std::size_t size_ = 0;

    public:
        explicit trade(const currency::pair<Currency>& pair)
                :pair_(pair) { }

        trade(const currency::pair<Currency>& pair, const position& pos)
                :pair_(pair)
        {
            buy(pos);
        }

        void buy(const position& pos)
        {
            size_ += pos.size();
            open_pos_.emplace_back(pos);
        }

        void sell(const position& pos)
        {
            if (pos.size()>size_)
                throw std::logic_error("Cannot sell more than was bought");

            size_ -= pos.size();
            close_pos_.emplace_back(pos);
        }

        void sell_all(const point& created)
        {
            sell(position{size_, created});
        }

        std::size_t size() const
        {
            return size_;
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
