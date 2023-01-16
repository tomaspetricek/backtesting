//
// Created by Tomáš Petříček on 02.11.2022.
//

#ifndef BACKTESTING_MOTION_TRACKER_HPP
#define BACKTESTING_MOTION_TRACKER_HPP

#include <trading/types.hpp>

namespace trading {
    struct motion {
        amount_t peak;
        amount_t trough;

        explicit motion(amount_t peak, amount_t trough)
                :peak(peak), trough(trough)
        {
            assert(peak>=trough);
        }

        motion() = default;
    };

    struct drawdown : public motion {
        // peak happens before trough
        explicit drawdown(amount_t peak, amount_t trough)
                :motion(peak, trough) { }

        drawdown() = default;

        template<class T>
        requires std::same_as<T, amount>
        amount_t value() const
        {
            assert(trough-peak<=0.0);
            return trough-peak;
        }

        template<class T>
        requires std::same_as<T, percent>
        percent_t value() const
        {
            return ((trough-peak)/peak)*100;
        }
    };

    struct run_up : public motion {
        // note: trough happens before peak
        explicit run_up(amount_t trough, amount_t peak)
                :motion(peak, trough) { }

        run_up() = default;

        template<class T>
        requires std::same_as<T, amount>
        amount_t value() const
        {
            assert(peak-trough>=0.0);
            return peak-trough;
        }

        template<class T>
        requires std::same_as<T, percent>
        percent_t value() const
        {
            return ((peak-trough)/trough)*100;
        }
    };

    class drawdown_tracker {
        drawdown max_;
        drawdown curr_;

    public:
        typedef drawdown motion_type;

        explicit drawdown_tracker(amount_t init)
                :max_(init, init), curr_(init, init) { }

        drawdown_tracker() = default;

        void update(amount_t val)
        {
            // going up -> drawdown is zero
            // moves in opposite direction -> goes with it
            if (val>curr_.peak) {
                curr_.peak = val;
                curr_.trough = val;
            }
            else if (val<curr_.trough) {
                curr_.trough = val;
            }

            if (curr_.value<amount>()<max_.value<amount>())
                max_ = curr_;
        }

        const drawdown& max() const
        {
            return max_;
        }
        const drawdown& current() const
        {
            return curr_;
        }
    };

    class run_up_tracker {
        run_up max_;
        run_up curr_;

    public:
        typedef run_up motion_type;

        explicit run_up_tracker(const amount_t init)
                :max_(init, init), curr_(init, init) { }

        run_up_tracker() = default;

        void update(amount_t val)
        {
            // going down -> run up is zero
            // moves in opposite direction -> goes with it
            if (val<curr_.trough) {
                curr_.peak = val;
                curr_.trough = val;
            }
            else if (val>curr_.peak) {
                curr_.peak = val;
            }

            if (curr_.value<amount>()>max_.value<amount>())
                max_ = curr_;
        }

        const run_up& max() const
        {
            return max_;
        }
        const run_up& current() const
        {
            return curr_;
        }
    };
}

#endif //BACKTESTING_MOTION_TRACKER_HPP
