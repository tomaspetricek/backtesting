//
// Created by Tomáš Petříček on 02.08.2022.
//

#ifndef EMASTRATEGY_CLOSE_POSITION_HPP
#define EMASTRATEGY_CLOSE_POSITION_HPP

#include <trading/position.hpp>

namespace trading {
    template<currency::crypto base, currency::crypto quote>
    class close_position : public position<quote, base, quote> {
    public:
        explicit close_position(const amount<base>& sold, const price<quote>& price,
                const boost::posix_time::ptime& created)
                :position<quote, base, quote>(sold, price, created)
        {
            this->bought_ = amount<quote>(static_cast<double>(this->sold_)*static_cast<double>(this->price_));
        }
    };
}
#endif //EMASTRATEGY_CLOSE_POSITION_HPP
