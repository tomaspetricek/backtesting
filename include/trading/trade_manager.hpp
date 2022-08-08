//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef EMASTRATEGY_TRADE_MANAGER_HPP
#define EMASTRATEGY_TRADE_MANAGER_HPP

#include <optional>
#include "storage.hpp"
namespace trading {
    class trade_manager {
    protected:
        std::optional<trade> active_;
        storage& storage_;

        explicit trade_manager(storage& storage)
                :storage_(storage) { }

        virtual void buy(const price_point& point) = 0;

        virtual void sell(const price_point& point) = 0;

        virtual void sell_all(const price_point& point) = 0;

    public:
        void update_active_trade(const action& action, const price_point& point)
        {
            // act
            switch (action) {
            case action::buy:
                buy(point);
                break;
            case action::sell:
                sell(point);
                break;
            case action::sell_all:
                sell_all(point);
                break;
            case action::do_nothing:
                break;
            }

            if (active_) {
                // save closed trade
                if (active_->is_closed()) {
                    storage_.save_closed_trade(*active_);
                    active_ = std::nullopt;
                }
            }
        }

        void try_close_active_trade(const price_point& point)
        {
            if (active_) {
                sell_all(point);
                storage_.save_closed_trade(*active_);
                active_ = std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_HPP
