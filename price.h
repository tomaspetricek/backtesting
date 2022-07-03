//
// Created by Tomáš Petříček on 03.07.2022.
//

#ifndef EMASTRATEGY_PRICE_H
#define EMASTRATEGY_PRICE_H

namespace trading {
    class price {
        double val_;

        static double validate(double val)
        {
            if (val<0.0)
                throw std::invalid_argument("Price has to be greater than 0");

            return val;
        }

    public:
        explicit price(double value=0.0)
                :val_(validate(value)) { }

        bool operator<(const price& rhs) const
        {
            return val_<rhs.val_;
        }

        bool operator>(const price& rhs) const
        {
            return rhs<*this;
        }

        bool operator<=(const price& rhs) const
        {
            return !(rhs<*this);
        }

        bool operator>=(const price& rhs) const
        {
            return !(*this<rhs);
        }

        price operator+(double val) const
        {
            return price{val_+val};
        }

        price operator-(double val) const
        {
            return price{val_-val};
        }

        price operator*(double val) const
        {
            return price{val_*val};
        }

        price operator/(double val) const
        {
            return price{val_/val};
        }

        explicit operator double() const {
            return val_;
        }
    };
}

#endif //EMASTRATEGY_PRICE_H
