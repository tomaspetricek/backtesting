//
// Created by Tomáš Petříček on 02.08.2022.
//

#ifndef EMASTRATEGY_AMOUNT_HPP
#define EMASTRATEGY_AMOUNT_HPP

#include <trading/currency.hpp>

namespace trading {
    template<typename currency::crypto curr>
    class amount {
        double val_;

        static double validate(double val)
        {
            if (val<0.0)
                throw std::invalid_argument("Amount has to be greater than 0");

            return val;
        }

    public:
        explicit amount(double value)
                :val_(value) { }

        explicit operator double() const
        {
            return val_;
        }

        amount<curr>& operator+=(const amount<curr>& rhs)
        {
            *this = amount<curr>{val_+rhs.val_};
            return *this;
        }

        amount<curr>& operator-=(const amount<curr>& rhs)
        {
            *this = amount<curr>{val_-rhs.val_};
            return *this;
        }

        amount<curr> operator+(const amount<curr>& rhs) const
        {
            return amount<curr>{val_+rhs.val_};
        }

        amount<curr> operator-(const amount<curr>& rhs) const
        {
            return amount<curr>{val_-rhs.val_};
        }

        bool operator==(const amount& rhs) const
        {
            return val_==rhs.val_;
        }

        bool operator==(double val) const
        {
            return val_==val;
        }

        bool operator!=(const amount& rhs) const
        {
            return !(rhs==*this);
        }

        bool operator<(const amount& rhs) const
        {
            return val_<rhs.val_;
        }

        bool operator>(const amount& rhs) const
        {
            return rhs<*this;
        }

        bool operator<=(const amount& rhs) const
        {
            return !(rhs<*this);
        }

        bool operator>=(const amount& rhs) const
        {
            return !(*this<rhs);
        }
    };
}

#endif //EMASTRATEGY_AMOUNT_HPP
