//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_INTERVAL_H
#define EMASTRATEGY_INTERVAL_H

#include <string_view>

namespace trading::validator {
    template<int min, int max, typename MinComparator, typename MaxComparator>
    class interval {
        static_assert(min<max);
        MinComparator min_comp_{};
        MaxComparator max_comp_{};
        std::string_view min_bracket_;
        std::string_view max_bracket_;

    public:
        constexpr interval(const std::string_view& min_bracket, const std::string_view& max_bracket)
                :min_bracket_(min_bracket), max_bracket_(max_bracket) { }
    public:
        double operator()(double val) const
        {
            if (!min_comp_(val, min) || !max_comp_(val, max)) {
                std::stringstream msg;
                msg << "Value " << val << " is out of interval " << min_bracket_ << min << ", " << max << max_bracket_;
                throw std::invalid_argument(msg.str());
            }

            return val;
        }
    };

    template<int min, int max>
    class open_interval : public interval<min, max, std::greater<double>, std::less<double>> {
    public:
        constexpr open_interval()
                :interval<min, max, std::greater<double>, std::less<double>>("(", ")") { }
    };

    template<int min, int max>
    class closed_interval : public interval<min, max, std::greater_equal<double>, std::less_equal<double>> {
    public:
        constexpr closed_interval()
                :interval<min, max, std::greater_equal<double>, std::less_equal<double>>("[", "]") { }
    };

    // does not contain maximum
    template<int min, int max>
    class right_open_interval : public interval<min, max, std::greater_equal<double>, std::less<double>> {
    public:
        constexpr right_open_interval()
                :interval<min, max, std::greater_equal<double>, std::less<double>>("[", ")") { }
    };

    // does not contain minimum
    template<int min, int max>
    class left_open_interval : public interval<min, max, std::greater<double>, std::less_equal<double>> {
    public:
        constexpr left_open_interval()
                :interval<min, max, std::greater<double>, std::less_equal<double>>("(", "]") { }
    };
}

#endif //EMASTRATEGY_INTERVAL_H
