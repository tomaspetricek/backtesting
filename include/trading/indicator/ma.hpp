//
// Created by Tomáš Petříček on 07.07.2022.
//

#ifndef EMASTRATEGY_MOVING_AVERAGE_HPP
#define EMASTRATEGY_MOVING_AVERAGE_HPP

#include <cstddef>
#include <stdexcept>

namespace trading::indicator {
    class ma {
        static std::size_t validate_period(size_t period)
        {
            if (period<min_period)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

    protected:
        size_t period_;
        bool curr_ready_{false};
        bool prev_ready_{false};
        double prev_val_{0};
        double curr_val_{0};
        constexpr static size_t min_period{1};

    public:
        explicit ma(size_t period = min_period)
                :period_(validate_period(period)) { }

        double current_value() const
        {
            if (!curr_ready_)
                throw not_ready("Current value is not ready");

            return curr_val_;
        }

        double previous_value() const
        {
            if (!prev_ready_)
                throw not_ready("Previous value is not ready");

            return prev_val_;
        }

        size_t period() const
        {
            return period_;
        }

        bool current_ready() const
        {
            return curr_ready_;
        }

        bool previous_ready() const
        {
            return prev_ready_;
        }
    };
}

#endif //EMASTRATEGY_MOVING_AVERAGE_HPP
