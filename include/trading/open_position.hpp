//
// Created by Tomáš Petříček on 02.08.2022.
//

#ifndef EMASTRATEGY_OPEN_POSITION_HPP
#define EMASTRATEGY_OPEN_POSITION_HPP

#include <trading/position.hpp>
#include <trading/amount.hpp>

namespace trading {
    template<currency::crypto base, currency::crypto quote>
    class open_position : public position<base, quote, quote> {
    public:
        explicit open_position(const amount<quote>& sold, const price<quote>& price,
                const boost::posix_time::ptime& created)
                :position<base, quote, quote>(sold, price, created)
        {
            this->bought_ = amount<base>(static_cast<double>(this->sold_)/static_cast<double>(this->price_));
        }
    };
}

#endif //EMASTRATEGY_OPEN_POSITION_HPP
