//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_STATS_HPP
#define EMASTRATEGY_STATS_HPP

namespace trading {
    class stats {
    protected:
        double min_profit_ = 0.0;
        double max_profit_ = 0.0;
        std::size_t n_closed_trades_ = 0;

        explicit stats(size_t n_closed_trades)
                :n_closed_trades_(n_closed_trades) { }

    public:
        double min_profit() const
        {
            return min_profit_;
        }

        double max_profit() const
        {
            return max_profit_;
        }

        size_t n_closed_trades() const
        {
            return n_closed_trades_;
        }
    };
}

#endif //EMASTRATEGY_STATS_HPP
