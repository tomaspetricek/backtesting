//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_INTERVAL_H
#define EMASTRATEGY_INTERVAL_H

template<int min, int max, typename MinComparator, typename MaxComparator>
class interval {
    static_assert(min<max);

protected:
    double val_;

    static double validate(double val)
    {
        if (!MinComparator()(val, min) || !MaxComparator()(val, max))
            throw std::invalid_argument("Value out of interval");

        return val;
    }

public:
    explicit interval(double val)
            :val_(validate(val)) { }

    explicit operator double() const
    {
        return val_;
    }
};

template<int min, int max>
class open_interval : public interval<min, max, std::greater<double>, std::less<double>> {
public:
    explicit open_interval(double val)
        : interval<min, max, std::greater<double>, std::less<double>>(val) {}
};

template<int min, int max>
class closed_interval : public interval<min, max, std::greater_equal<double>, std::less_equal<double>> {
public:
    explicit closed_interval(double val)
        : interval<min, max, std::greater_equal<double>, std::less_equal<double>>(val) {}
};

// does not contain maximum
template<int min, int max>
class right_open_interval : public interval<min, max, std::greater_equal<double>, std::less<double>> {
public:
    explicit right_open_interval(double val)
        : interval<min, max, std::greater_equal<double>, std::less<double>>(val) {}
};

// does not contain minimum
template<int min, int max>
class left_open_interval : public interval<min, max, std::greater<double>, std::less_equal<double>> {
public:
    explicit left_open_interval(double val)
        : interval<min, max, std::greater<double>, std::less_equal<double>>(val) { }
};

#endif //EMASTRATEGY_INTERVAL_H
