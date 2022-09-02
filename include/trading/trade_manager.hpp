//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef EMASTRATEGY_TRADE_MANAGER_HPP
#define EMASTRATEGY_TRADE_MANAGER_HPP

#include <optional>

#include <trading/data_point.hpp>

namespace trading {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<class Trade, class Market, class ConcreteTradeManager>
    class trade_manager {
        void save_closed_active_trade()
        {
            assert(active_);
            assert(active_->is_closed());
            closed_.emplace_back(*active_);
            active_ = std::nullopt;
            static_cast<ConcreteTradeManager*>(this)->reset_state_impl();
        }

    protected:
        std::optional<Trade> active_;
        std::vector<Trade> closed_;
        typename Market::wallet_type wallet_;

        explicit trade_manager() = default;

        explicit trade_manager(const typename Market::wallet_type& wallet)
                :wallet_(wallet) { }

    protected:

        void buy(const price_point& point)
        {
            amount_t buy_amount = static_cast<ConcreteTradeManager*>(this)->get_buy_amount();

            if (!this->active_) {
                auto pos = Market::template create_open_position<typename Trade::position_type>(wallet_, buy_amount,
                        point.data, point.time);
                this->active_ = std::make_optional(long_trade(pos));
            }
            else {
                auto pos = Market::template create_open_position<typename Trade::position_type>(wallet_, buy_amount, point.data,
                        point.time);
                this->active_->add_opened(pos);
            }
        }

        void sell(const price_point& point)
        {
            assert(active_);
            amount_t sell_amount = static_cast<ConcreteTradeManager*>(this)->get_sell_amount();
            auto pos = Market::template create_close_position<typename Trade::position_type>(wallet_, sell_amount,
                    point.data, point.time);
            this->active_->add_closed(pos);
            if (active_->is_closed()) save_closed_active_trade();
        }

        void sell_all(const price_point& point)
        {
            assert(active_);
            auto pos = Market::template create_close_position<typename Trade::position_type>(wallet_,
                    this->active_->size(), point.data, point.time);
            this->active_->add_closed(pos);
            save_closed_active_trade();
        }

    public:
        // it closes active trade, if there is one
        void try_closing_active_trade(const price_point& point)
        {
            if (active_) sell_all(point);
        }

        std::vector<Trade> retrieve_closed_trades()
        {
            return std::move(closed_);
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_HPP
