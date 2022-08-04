//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_CURRENCY_HPP
#define EMASTRATEGY_CURRENCY_HPP

namespace trading::currency {
    enum class crypto {
        BTC,
        USDT,
    };

    enum class forex {
        USD,
        EUR,
        GBP,
    };

    // BTC/USDT - base/quote
    class pair {
        crypto base_;
        crypto quote_;

    public:
        pair(crypto base, crypto quote)
                :base_(base), quote_(quote)
        {
            if (base_==quote_)
                throw std::runtime_error("Base and quote currencies has to be different");
        }

        crypto base() const
        {
            return base_;
        }
        crypto quote() const
        {
            return quote_;
        }
    };
}

#endif //EMASTRATEGY_CURRENCY_HPP
