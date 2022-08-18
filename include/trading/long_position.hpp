//
// Created by Tomáš Petříček on 09.08.2022.
//

#ifndef EMASTRATEGY_LONG_POSITION_HPP
#define EMASTRATEGY_LONG_POSITION_HPP

#include <trading/position.hpp>

namespace trading {
    class long_position : public position {
    protected:
        long_position(amount_t sold, const amount_t bought, const price_t& price,
                const boost::posix_time::ptime& created)
                :position{sold, bought, price, created} { }

    public:
        static long_position
        create_close(amount_t sold, const trading::price_t& price, const boost::posix_time::ptime& created)
        {
            assert(sold>amount_t{0});
            return {sold, amount_t{value_of(sold)*value_of(price)}, price, created};
        }

        static long_position
        create_open(amount_t sold, const trading::price_t& price, const boost::posix_time::ptime& created)
        {
            assert(sold>amount_t{0});
            return {sold, amount_t{value_of(sold)/value_of(price)}, price, created};
        }
    };

}

#endif //EMASTRATEGY_LONG_POSITION_HPP
