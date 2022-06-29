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
    struct pair {
        CurrencyType base;
        CurrencyType quote;
    };
}

#endif //EMASTRATEGY_CURRENCY_H
