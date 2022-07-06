//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_INTERVAL_H
#define EMASTRATEGY_INTERVAL_H

namespace trading::validator {
    template<int min, int max, typename MinComparator, typename MaxComparator>
    class interval {
        static_assert(min<max);
        MinComparator min_comp_{};
        MaxComparator max_comp_{};

    public:
        double operator()(double val) const
        {
            if (!min_comp_(val, min) || !max_comp_(val, max))
                throw std::invalid_argument("Value out of interval");

            return val;
        }
    };

    template<int min, int max>
    class open_interval : public interval<min, max, std::greater<double>, std::less<double>> {
    };

    template<int min, int max>
    class closed_interval : public interval<min, max, std::greater_equal<double>, std::less_equal<double>> {
    };

    // does not contain maximum
    template<int min, int max>
    class right_open_interval : public interval<min, max, std::greater_equal<double>, std::less<double>> {
    };

    // does not contain minimum
    template<int min, int max>
    class left_open_interval : public interval<min, max, std::greater<double>, std::less_equal<double>> {
    };
}

#endif //EMASTRATEGY_INTERVAL_H
