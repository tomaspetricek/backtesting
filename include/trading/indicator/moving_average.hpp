//
// Created by Tomáš Petříček on 07.07.2022.
//

#ifndef EMASTRATEGY_MOVING_AVERAGE_HPP
#define EMASTRATEGY_MOVING_AVERAGE_HPP

namespace trading::indicator {
    class moving_average {
        static int validate_period(int period)
        {
            if (period<min_period)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

    protected:
        int period_;
        bool ready_ = false;
        constexpr static int min_period = 1;

    public:
        explicit moving_average(int period = min_period)
                :period_(validate_period(period)) { }

        int period() const
        {
            return period_;
        }

        bool is_ready() const
        {
            return ready_;
        }
    };
}

#endif //EMASTRATEGY_MOVING_AVERAGE_HPP
