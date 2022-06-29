//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_CURRENCY_H
#define EMASTRATEGY_CURRENCY_H

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
    template<typename CurrencyType>
    class pair {
        CurrencyType base;
        CurrencyType quote;

    public:
        pair(CurrencyType base, CurrencyType quote)
                :base(base), quote(quote)
        {
            if (base==quote)
                throw std::runtime_error("Base and quote currencies has to be different");
        }
    };
}

#endif //EMASTRATEGY_CURRENCY_H
