//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <ostream>
namespace trading {
    class action {
    public:
        enum value {
            buy,
            sell,
            do_nothing
        };

        action() = default;
        constexpr action(value act)
                :val_(act) { }

        constexpr explicit operator value() const { return val_; }
        explicit operator bool() const = delete;

        constexpr bool operator==(action act) const { return val_==act.val_; }
        constexpr bool operator==(action::value val) const { return val_==val; }
        constexpr bool operator!=(action act) const { return val_!=act.val_; }
        constexpr bool operator!=(action::value val) const { return val_!=val; }

        friend std::ostream& operator<<(std::ostream& os, const action& action)
        {
            std::string val;

            switch (action.val_) {
            case buy:
                val = "buy";
                break;
            case sell:
                val = "sell";
                break;
            case do_nothing:
                val = "do nothing";
                break;
            }
            os << val;
            return os;
        }

    private:
        value val_;
    };

    class side {
    public:
        enum value {
            short_,
            long_,
            none
        };

        side() = default;
        constexpr side(value act)
                :val_(act) { }

        constexpr explicit operator value() const { return val_; }
        explicit operator bool() const = delete;

        constexpr bool operator==(side oth) const { return val_==oth.val_; }
        constexpr bool operator==(side::value val) const { return val_==val; }
        constexpr bool operator!=(side oth) const { return val_!=oth.val_; }
        constexpr bool operator!=(side::value val) const { return val_!=val; }

        friend std::ostream& operator<<(std::ostream& os, const side& side)
        {
            std::string val;

            switch (side.val_) {
            case short_:
                val = "short";
                break;
            case long_:
                val = "long";
                break;
            case none:
                val = "none";
                break;
            }
            os << val;
            return os;
        }

    private:
        value val_;
    };

    struct order {
        trading::action action;
        trading::side side;

        explicit order(trading::action action, trading::side side)
                :action(action), side(side) { }

        friend std::ostream& operator<<(std::ostream& os, const order& order)
        {
            os << "action: " << order.action << ", side: " << order.side;
            return os;
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
