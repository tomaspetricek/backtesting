//
// Created by Tomáš Petříček on 07.07.2022.
//

#ifndef BACKTESTING_MOVING_AVERAGE_HPP
#define BACKTESTING_MOVING_AVERAGE_HPP

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
        bool ready_{false};
        constexpr static size_t min_period{1};

    public:
        explicit ma(size_t period = min_period)
                :period_(validate_period(period)) { }

        std::size_t period() const
        {
            return period_;
        }

        bool is_ready() const
        {
            return ready_;
        }
    };
}

#endif //BACKTESTING_MOVING_AVERAGE_HPP
