//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef EMASTRATEGY_STRATEGY_HPP
#define EMASTRATEGY_STRATEGY_HPP

namespace trading {
    class strategy {
    protected:
        virtual bool should_buy() = 0;

        virtual bool should_sell() = 0;

        virtual bool should_sell_all() = 0;

    public:
        virtual action operator()(const price_t& curr) = 0;
    };
}

#endif //EMASTRATEGY_STRATEGY_HPP
