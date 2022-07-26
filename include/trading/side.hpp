//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_SIDE_H
#define EMASTRATEGY_SIDE_H

namespace trading {
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
}

#endif //EMASTRATEGY_SIDE_H
