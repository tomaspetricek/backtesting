//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_CANDLE_H
#define EMASTRATEGY_CANDLE_H

#include "price.h"
#include "pack.h"

namespace trading {
    class candle {
        std::time_t created_;
        price open_;
        price high_;
        price low_;
        price close_;

        static std::time_t validate_time(time_t time)
        {
            if (time<0)
                throw std::invalid_argument("Time has to be greater than 0");

            return time;
        }

    public:
        candle(time_t created, const price& open, const price& high, const price& low, const price& close)
                :created_(validate_time(created)), open_(open), high_(high), low_(low), close_(close)
        {
            if (high<=low)
                throw std::invalid_argument("High price has to be lower or equal to low price");
        }

        candle(time_t created, double open, double high, double low, double close)
                :candle{created, price{open}, price{high}, price{low}, price{close}} { }

        // mean high and low
        static price hl2(const candle& candle)
        {
            return mean(candle.high_, candle.low_);
        }

        // mean open, high, low, close
        static price hlc3(const candle& candle)
        {
            return mean(candle.high_, candle.low_, candle.close_);
        }

        // mean open, high, low, close
        static price ohlc4(const candle& candle)
        {
            return mean(candle.open_, candle.high_, candle.low_, candle.close_);
        }

        std::time_t created() const
        {
            return created_;
        }

        price open() const
        {
            return open_;
        }

        price high() const
        {
            return high_;
        }

        price low() const
        {
            return low_;
        }

        price close() const
        {
            return close_;
        }
    };
}

#endif //EMASTRATEGY_CANDLE_H
