//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_CANDLE_H
#define EMASTRATEGY_CANDLE_H

namespace trading {
    class candle {
        std::time_t created_;
        double open_;
        double high_;
        double low_;
        double close_;

        static double validate_price(double price)
        {
            if (price<0)
                throw std::invalid_argument("Price has to greater than 0");

            return price;
        }

        static std::time_t validate_time(std::time_t time)
        {
            if (time<0)
                throw std::invalid_argument("Time has to be greater than 0");

            return time;
        }

    public:
        candle(time_t created, double open, double high, double low, double close)
                :created_(validate_time(created)), open_(validate_price(open)), high_(validate_price(high)),
                 low_(validate_price(low)), close_(validate_price(close)) { }

        std::time_t get_created() const
        {
            return created_;
        }
        double get_open() const
        {
            return open_;
        }
        double get_high() const
        {
            return high_;
        }
        double get_low() const
        {
            return low_;
        }
        double get_close() const
        {
            return close_;
        }
    };
}

#endif //EMASTRATEGY_CANDLE_H
