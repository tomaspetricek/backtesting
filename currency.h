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

    // BTC/USDT - base/quote
    struct crypto_pair {
        crypto base;
        crypto quote;
    };
}

#endif //EMASTRATEGY_CURRENCY_H
