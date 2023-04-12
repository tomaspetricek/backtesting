//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_SYSTEMATIC_GENERATORS_HPP
#define BACKTESTING_SYSTEMATIC_GENERATORS_HPP

#include <trading/generators.hpp>
#include <trading/int_range.hpp>

namespace trading::systematic {
    // generates unique subsequent fractions in interval (0, max), such as max is in interval (0.0, 1.0]
    template<std::size_t n_levels>
    class levels_generator : public trading::levels_generator<n_levels> {
        using base_type = trading::levels_generator<n_levels>;

    public:
        using value_type = typename trading::levels_generator<n_levels>::value_type;

        explicit levels_generator(size_t unique_count = n_levels, fraction_t lower_bound = base_type::default_lower_bound)
                :base_type(unique_count, lower_bound) { }

        cppcoro::recursive_generator<value_type> operator()()
        {
            co_yield generate<0>(this->unscaled_denom());
        }

    private:
        template<std::size_t depth = n_levels>
        requires (depth==n_levels)
        cppcoro::recursive_generator<value_type> generate(std::size_t) { co_yield this->levels_; }

        template<std::size_t depth = 0>
        requires (depth<n_levels)
        cppcoro::recursive_generator<value_type> generate(std::size_t prev_num)
        {
            for (std::size_t num{--prev_num}; num>n_levels-depth-1; num--) {
                this->levels_[depth] = this->rescale(num);
                co_yield generate<depth+1>(num);
            }
        }
    };

    // generates fractions that add up to 1.0
    template<std::size_t n_sizes>
    class sizes_generator : public trading::sizes_generator<n_sizes> {
    public:
        explicit sizes_generator(size_t n_unique)
                :trading::sizes_generator<n_sizes>(n_unique) { }

        using value_type = typename trading::sizes_generator<n_sizes>::value_type;

        cppcoro::recursive_generator<value_type> operator()()
        {
            co_yield generate<0>(this->denom_);
        }

    private:
        template<std::size_t depth = n_sizes>
        requires (depth+1==n_sizes)
        cppcoro::recursive_generator<value_type> generate(std::size_t remaining)
        {
            this->sizes_[depth] = fraction_t{remaining, this->denom_};
            co_yield this->sizes_;
        }

        template<std::size_t depth = 0>
        requires (depth+1<n_sizes)
        cppcoro::recursive_generator<value_type> generate(std::size_t remaining)
        {
            std::size_t max = (remaining>this->max_num_) ? this->max_num_ : remaining-1;
            for (std::size_t num{1}; num<=max; num++) {
                this->sizes_[depth] = fraction_t{num, this->denom_};
                co_yield generate<depth+1>(remaining-num);
            }
        }
    };

    class int_range_generator : public trading::int_range {
        using base_type = trading::int_range;

    public:
        using value_type = base_type::value_type;

        int_range_generator(int from, int to, int step)
                :base_type(from, to, step) { }

        cppcoro::generator<value_type> operator()()
        {
            for (int val{this->from_}; val!=this->to_+this->step_; val += this->step_)
                co_yield val;
        }
    };
}

#endif //BACKTESTING_SYSTEMATIC_GENERATORS_HPP
