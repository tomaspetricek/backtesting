//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_FUTURES_MARKET_HPP
#define EMASTRATEGY_FUTURES_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/market.hpp>
#include <trading/trade.hpp>
#include <trading/binance/futures/order.hpp>
#include <trading/binance/futures/position.hpp>

namespace trading::binance::futures {
    template<direction direct>
    class market : public trading::market<position<direct>, order, market<direct>> {
        friend class trading::market<position<direct>, order, market<direct>>;

    public:
        using position_type = position<direct>;

        explicit market(const wallet& wallet)
                :trading::market<position<direct>, order, market<direct>>(wallet) { }

        market() = default;

    protected:
        void create_open_trade(const order& order)
        {
            trade open = trade{order.sold, order.price, order.created};

            // add trade
            if (this->active_) {
                assert(this->active_->leverage()==order.leverage);
                this->active_->add_open(open);
            }
            else {
                this->active_ = std::make_optional<position<direct>>(open, order.leverage);
            }
        }
    };

    using long_market = market<direction::long_>;
    using short_market = market<direction::short_>;
}

#endif //EMASTRATEGY_FUTURES_MARKET_HPP
