//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_ACTION_H
#define EMASTRATEGY_ACTION_H

namespace trading {
    class action {
    public:
        enum value {
            buy,
            sell,
            sell_all,
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
            case sell_all:
                val = "sell all";
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
}

#endif //EMASTRATEGY_ACTION_H
