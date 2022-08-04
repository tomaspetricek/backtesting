//
// Created by Tomáš Petříček on 02.08.2022.
//

#ifndef EMASTRATEGY_MONEY_HPP
#define EMASTRATEGY_MONEY_HPP

#include <trading/currency.hpp>
#include <trading/exception.hpp>

namespace trading {

    // source: https://github.com/mariusbancila/moneycpp/blob/master/include/money.h
    class money {
        double amount_;
        currency::crypto currency_;

    public:
        constexpr money(double amount, currency::crypto currency) noexcept
                :amount_(amount), currency_(currency) { }

        explicit operator double() const
        {
            return amount_;
        }

        double amount() const
        {
            return amount_;
        }

        currency::crypto currency() const
        {
            return currency_;
        }

        money& operator+=(const money& rhs)
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            amount_ += rhs.amount_;
            return *this;
        }

        money& operator-=(const money& rhs)
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            amount_ -= rhs.amount_;
            return *this;
        }

        money operator+(const money& rhs) const
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            return money{amount_+rhs.amount_, currency_};
        }

        money operator-(const money& rhs) const
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            return money{amount_+rhs.amount_, currency_};
        }

        bool operator==(const money& rhs) const
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            return amount_==rhs.amount_;
        }

        bool operator!=(const money& rhs) const
        {
            return !(rhs==*this);
        }

        bool operator<(const money& rhs) const
        {
            if (currency_!=rhs.currency_) throw currency_mismatch_error{};
            return amount_<rhs.amount_;
        }

        bool operator>(const money& rhs) const
        {
            return rhs<*this;
        }

        bool operator<=(const money& rhs) const
        {
            return !(rhs<*this);
        }

        bool operator>=(const money& rhs) const
        {
            return !(*this<rhs);
        }
    };
}

#endif //EMASTRATEGY_MONEY_HPP
