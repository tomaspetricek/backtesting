//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <ostream>

#include "point.h"
#include "formula.h"

namespace trading {
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
        double percent_gain_;

        position(const point& entry, double size)
                :entry_(entry), size_(size) { }

    public:
        virtual ~position() = default;

        virtual double calculate_percent_gain(double curr_price) = 0;

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

    class long_position : public position {
    public:
        long_position(const point& entry, double amount)
                :position(entry, amount) { }

        ~long_position() override = default;

        double calculate_percent_gain(double curr_price) override
        {
            return formula::calculate_percent_gain(entry_.get_price(), curr_price);
        }
    };

    class short_position : public position {
    public:
        short_position(const point& entry, double amount)
                :position(entry, amount) { }

        ~short_position() override = default;

        double calculate_percent_gain(double curr_price) override
        {
            return formula::calculate_percent_gain(curr_price, entry_.get_price());
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
