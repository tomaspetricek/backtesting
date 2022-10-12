//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_WALLET_HPP
#define BACKTESTING_WALLET_HPP

#include <trading/amount_t.hpp>
#include <trading/exception.hpp>

namespace trading {
    class wallet {
    protected:
        amount_t balance_{0.0};

        static const amount_t& validate_balance(const amount_t& balance)
        {
            if (value_of(balance)<0)
                throw std::invalid_argument("Balance has to be greater than 0");

            return balance;
        }

    public:
        explicit wallet(const amount_t& balance)
                :balance_(validate_balance(balance)) { }

        wallet() = default;

        void withdraw(const amount_t& amount)
        {
            if (value_of(amount)<=0)
                throw std::invalid_argument{"Cannot withdraw nothing"};

            if (amount>balance_)
                throw insufficient_funds{"Too much to withdraw"};

            balance_ -= amount;
        }

        void deposit(const amount_t& amount)
        {
            if (value_of(amount)<=0)
                throw std::invalid_argument{"Cannot withdraw nothing"};

            balance_ += amount;
        }

        const amount_t& balance() const
        {
            return balance_;
        }
    };
}

#endif //BACKTESTING_WALLET_HPP
