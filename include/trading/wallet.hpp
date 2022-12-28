//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_WALLET_HPP
#define BACKTESTING_WALLET_HPP

#include <trading/types.hpp>
#include <trading/exception.hpp>

namespace trading {
    class wallet {
    protected:
        amount_t balance_{0.0};

        static amount_t validate_balance(amount_t balance)
        {
            if (balance<0.0)
                throw std::invalid_argument("Balance has to be greater than 0");

            return balance;
        }

    public:
        explicit wallet(amount_t balance)
                :balance_(validate_balance(balance)) { }

        wallet() = default;

        void withdraw(amount_t amount)
        {
            if (amount<=0.0) throw std::invalid_argument{"Cannot withdraw nothing"};
            if (amount>balance_) throw insufficient_funds{"Not enough balance"};
            balance_ -= amount;
        }

        void deposit(amount_t amount)
        {
            if (amount<=0.0) throw std::invalid_argument{"Cannot deposit nothing"};
            balance_ += amount;
        }

        amount_t balance() const
        {
            return balance_;
        }
    };
}

#endif //BACKTESTING_WALLET_HPP
