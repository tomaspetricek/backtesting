//
// Created by Tomáš Petříček on 07.07.2022.
//

#ifndef EMASTRATEGY_MOVING_AVERAGE_H
#define EMASTRATEGY_MOVING_AVERAGE_H

namespace trading::indicator {
    class moving_average {
        static int validate_period(int period)
        {
            if (period<=1)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

    protected:
        int period_;

    public:
        explicit moving_average(int period = 1)
                :period_(period) { }

        virtual double operator()(double sample) = 0;

        virtual ~moving_average() = default;

        int period() const
        {
            return period_;
        }
    };
}

#endif //EMASTRATEGY_MOVING_AVERAGE_H
