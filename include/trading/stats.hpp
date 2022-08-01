//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_STATS_HPP
#define EMASTRATEGY_STATS_HPP

namespace trading {
    class stats {
    protected:
        double max_profit_ = 0.0;
        double cumulative_profit_ = 0.0;
        std::size_t n_closed_trades_ = 0;

    public:
        explicit stats(size_t n_closed_trades)
                :n_closed_trades_(n_closed_trades) { }

        double max_profit() const
        {
            return max_profit_;
        }

        double cumulative_profit() const
        {
            return cumulative_profit_;
        }

        size_t n_closed_trades() const
        {
            return n_closed_trades_;
        }
    };
}

#endif //EMASTRATEGY_STATS_HPP
