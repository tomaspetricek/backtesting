//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef EMASTRATEGY_TRADE_MANAGER_HPP
#define EMASTRATEGY_TRADE_MANAGER_HPP

#include <optional>

#include <trading/storage.hpp>

namespace trading {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<typename Trade, typename ConcreteTradeManager>
    class trade_manager {
        void try_saving_active_trade()
        {
            if (active_) {
                if (active_->is_closed()) {
                    storage_.save_closed_trade(*active_);
                    active_ = std::nullopt;
                }
            }
        }

    protected:
        std::optional<Trade> active_;
        storage& storage_;

        explicit trade_manager(storage& storage)
                :storage_(storage) { }

    public:
        void update_active_trade(const action& action, const price_point& point)
        {
            // act
            switch (action) {
            case action::buy:
                static_cast<ConcreteTradeManager*>(this)->buy_impl(point);
                break;
            case action::sell:
                static_cast<ConcreteTradeManager*>(this)->sell_impl(point);
                try_saving_active_trade();
                break;
            case action::sell_all:
                static_cast<ConcreteTradeManager*>(this)->sell_all_impl(point);
                try_saving_active_trade();
                break;
            case action::do_nothing:
                break;
            }
        }

        void try_closing_active_trade(const price_point& point)
        {
            if (active_) {
                static_cast<ConcreteTradeManager*>(this)->sell_all_impl(point);
                storage_.save_closed_trade(*active_);
                active_ = std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_HPP
