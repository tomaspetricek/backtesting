//
// Created by Tomáš Petříček on 03.07.2022.
//

#ifndef EMASTRATEGY_PRICE_HPP
#define EMASTRATEGY_PRICE_HPP

#include <trading/currency.hpp>
#include <trading/exception.hpp>

namespace trading {
    class price {
        double val_;

        static double validate(double val)
        {
            if (val<0.0)
                throw std::invalid_argument("Price has to be greater or equal to 0");

            return val;
        }

    public:
        constexpr explicit price(double val)
                :val_(val) { }

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

        price operator+(const price& rhs) const
        {
            return price{val_+rhs.val_};
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

        explicit operator double() const
        {
            return val_;
        }
    };
}

#endif //EMASTRATEGY_PRICE_HPP
