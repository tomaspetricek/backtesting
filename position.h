//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <ostream>

#include "point.h"
#include "formula.h"
#include "currency.h"
#include "price.h"

namespace trading {
    template<typename CurrencyType>
    class position {
    private:
        point validate_exit(const point& exit)
        {
            if (entry_.get_created()>=exit.get_created())
                throw std::invalid_argument("Exit time has higher than entry time");

            return exit;
        }

    protected:
        point entry_;
        point exit_{};
        bool closed_ = false;
        double size_;
        double percent_gain_{};
        currency::pair<CurrencyType> pair_;

    public:
        position(const point& entry, double size, const currency::pair<CurrencyType>& pair)
                :entry_(entry), size_(size), pair_(pair) { }

        virtual ~position() = default;

        virtual double calculate_percent_gain(const price& curr) = 0;

        const point& get_entry() const
        {
            return entry_;
        }

        const point& get_exit() const
        {
            if (!closed_)
                throw not_ready("Position not closed yet");

            return exit_;
        }

        double get_percent_gain() const
        {
            if (!closed_)
                throw not_ready("Position not closed yet");

            return percent_gain_;
        }

        void set_exit(const point& exit)
        {
            validate_exit(exit);
            closed_ = true;
            exit_ = exit;
            percent_gain_ = calculate_percent_gain(exit_.get_price());
        }
    };

    template<typename CurrencyType>
    class long_position : public position<CurrencyType> {
    public:
        long_position(const point& entry, double size, const currency::pair<CurrencyType>& pair)
                :position<CurrencyType>(entry, size, pair) { }

        ~long_position() override = default;

        double calculate_percent_gain(const price& curr) override
        {
            return formula::percent_gain(this->entry_.get_price(), curr);
        }
    };

    template<typename CurrencyType>
    class short_position : public position<CurrencyType> {
    public:
        short_position(const point& entry, double size, const currency::pair<CurrencyType>& pair)
                :position<CurrencyType>(entry, size, pair) { }

        ~short_position() override = default;

        double calculate_percent_gain(const price& curr) override
        {
            return formula::percent_gain(curr, this->entry_.get_price());
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
