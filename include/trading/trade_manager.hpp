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
    template<typename Trade, typename ConcreteTradeManager>
    class trade_manager {
        void save_closed_active_trade()
        {
            assert(active_);
            assert(active_->is_closed());

            save_closed_trade(*active_);
            active_ = std::nullopt;
            static_cast<ConcreteTradeManager*>(this)->reset_state_impl();
        }

        void sell_all(const price_point& point)
        {
            auto pos = Trade::create_close_position(this->active_->size(), point.data, point.time);
            this->active_->add_closed(pos);
        }

    protected:
        std::optional<Trade> active_;
        std::vector<Trade> closed_;

        explicit trade_manager() = default;

        void save_closed_trade(const Trade& closed)
        {
            closed_.emplace_back(closed);
        }

    public:
        void update_active_trade(const action& action, const price_point& point)
        {
            // act
            switch (action) {
            case action::buy:
                static_cast<ConcreteTradeManager*>(this)->buy_impl(point);
                break;
            case action::sell:
                assert(active_);
                static_cast<ConcreteTradeManager*>(this)->sell_impl(point);
                if (active_->is_closed()) save_closed_active_trade();
                break;
            case action::sell_all:
                assert(active_);
                sell_all(point);
                save_closed_active_trade();
                break;
            case action::do_nothing:
                break;
            }
        }

        // it closes active trade, if there is one
        void try_closing_active_trade(const price_point& point)
        {
            if (active_) {
                sell_all(point);
                save_closed_active_trade();
            }
        }

        std::vector<Trade> retrieve_closed_trades()
        {
            return std::move(closed_);
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_HPP
