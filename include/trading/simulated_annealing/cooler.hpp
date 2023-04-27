//
// Created by Tomáš Petříček on 10.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_COOLER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_COOLER_HPP

#include <string>
#include <cmath>

namespace trading::simulated_annealing {
    class decayable_cooler {
    protected:
        float decay_;

        explicit decayable_cooler(const float decay)
                :decay_(decay) { }

    public:
        float decay() const
        {
            return decay_;
        }
    };

    class exp_mul_cooler : public decayable_cooler {
        static float validate(const float decay)
        {
            if (decay<0.8 || decay>1.0)
                throw std::invalid_argument("Decay has to be greater than 0.8 and lower than 1.0");
            else return decay;
        }

    public:
        explicit exp_mul_cooler(float decay)
                :decayable_cooler(validate(decay)) { }

        template<class SimulatedAnnealing>
        void operator()(SimulatedAnnealing& optimizer)
        {
            optimizer.current_temperature(optimizer.start_temperature()*std::pow(this->decay_, optimizer.it()));
        }

        static constexpr std::string_view name = "exp mul cooler";
    };

    class log_mul_cooler : public decayable_cooler {
    private:
        static float validate(const float decay)
        {
            if (decay<1.0) throw std::invalid_argument("Decay has to be greater than 1");
            else return decay;
        }

    public:
        explicit log_mul_cooler(float decay)
                :decayable_cooler(validate(decay)) { }

        template<class SimulatedAnnealing>
        void operator()(SimulatedAnnealing& optimizer)
        {
            optimizer.current_temperature(optimizer.start_temperature()/(1+this->decay_*std::log(1+optimizer.it())));
        }

        static constexpr std::string_view name = "log mul cooler";
    };

    struct basic_cooler {
        template<class SimulatedAnnealing>
        void operator()(SimulatedAnnealing& optimizer)
        {
            optimizer.current_temperature(optimizer.start_temperature()/(1+std::log(1+optimizer.it())));
        }

        static constexpr std::string_view name = "basic cooler";
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_COOLER_HPP
