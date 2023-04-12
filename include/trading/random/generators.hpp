//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_RANDOM_GENERATORS_HPP
#define BACKTESTING_RANDOM_GENERATORS_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/generators.hpp>
#include <trading/int_range.hpp>
#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/indicator.hpp>
#include <boost/assert.hpp>
#include <fmt/format.h>

namespace trading::random {
    template<std::size_t n_levels>
    class levels_generator : public trading::levels_generator<n_levels> {
        using base_type = trading::levels_generator<n_levels>;
        std::mt19937 gen_;
        std::vector<fraction_t> all_options_;
        std::array<std::size_t, n_levels> indices_;
        std::size_t change_n_;

        std::size_t validate_change_count(std::size_t change_n)
        {
            if (change_n==0 || change_n>n_levels)
                throw std::invalid_argument(fmt::format("Change n has to be in interval [1,{}]", n_levels));
            return change_n;
        }

    public:
        using value_type = std::array<fraction_t, n_levels>;

        explicit levels_generator(size_t unique_count = n_levels, std::size_t change_count = 1,
                fraction_t lower_bound = base_type::default_lower_bound)
                :base_type(unique_count, lower_bound), gen_{std::random_device{}()}, change_n_{
                validate_change_count(change_count)}
        {
            all_options_.reserve(unique_count);
            for (std::size_t i{0}; i<unique_count; i++)
                all_options_.emplace_back(this->rescale(i+1));

            for (std::size_t i{0}; i<n_levels; i++)
                indices_[i] = i;
        }

        const value_type& operator()()
        {
            std::shuffle(all_options_.begin(), all_options_.end(), gen_);
            std::sort(all_options_.begin(), all_options_.begin()+static_cast<int>(n_levels), std::greater<>());
            std::copy(all_options_.begin(), all_options_.begin()+static_cast<int>(n_levels), this->levels_.begin());
            return this->levels_;
        }

        const value_type& operator()(const value_type& origin)
        {
            std::size_t keep_n{n_levels-change_n_};
            std::shuffle(indices_.begin(), indices_.end(), gen_);
            std::sort(indices_.begin(), indices_.begin()+static_cast<int>(keep_n));
            etl::flat_set<fraction_t, n_levels, std::greater<>> unique;

            for (std::size_t i{0}; i<keep_n; i++)
                unique.emplace(origin[indices_[i]]);

            std::shuffle(all_options_.begin(), all_options_.end(), gen_);
            auto options_it = all_options_.begin();

            while (!unique.full())
                unique.insert(*options_it++);

            std::copy(unique.begin(), unique.end(), this->levels_.begin());
            return this->levels_;
        }
    };

    template<std::size_t n_sizes>
    class sizes_generator : public trading::sizes_generator<n_sizes> {
        std::mt19937 gen_;
        std::uniform_int_distribution<std::size_t> distrib_;
        std::array<std::size_t, n_sizes> indices_;
        std::size_t change_n_;
        static constexpr std::size_t min_change_n = 2;

        std::size_t validate_change_count(std::size_t change_count)
        {
            if (!(change_count>=min_change_n && change_count<=n_sizes))
                throw std::invalid_argument(
                        fmt::format("Change n has to be in interval [{},{}]", min_change_n, change_count));
            return change_count;
        }

        void fill_rest(std::size_t rest_num_sum, std::size_t curr_max_num, std::size_t first_index = 0)
        {
            std::size_t num;
            for (std::size_t i{first_index}; i<n_sizes-1; i++) {
                distrib_.param(std::uniform_int_distribution<std::size_t>::param_type{1, curr_max_num});
                num = distrib_(gen_);
                this->sizes_[indices_[i]] = fraction_t{num, this->denom_};
                rest_num_sum -= num;
                curr_max_num = (curr_max_num==num) ? 1 : curr_max_num-num+1;
            }
            this->sizes_[indices_.back()] = fraction_t{rest_num_sum, this->denom_};
        }

    public:
        using value_type = std::array<fraction_t, n_sizes>;

        explicit sizes_generator(size_t n_unique = 1, std::size_t change_n = min_change_n)
                :trading::sizes_generator<n_sizes>(n_unique), gen_(std::random_device{}()),
                 distrib_(1, this->max_num_), change_n_{validate_change_count(change_n)}
        {
            for (std::size_t i{0}; i<indices_.size(); i++)
                indices_[i] = i;
        }

        const value_type& operator()()
        {
            std::shuffle(indices_.begin(), indices_.end(), gen_);
            fill_rest(this->denom_, this->max_num_);
            return this->sizes_;
        }

        const value_type& operator()(const value_type& origin)
        {
            std::size_t i, keep_n{n_sizes-change_n_}, rest_num_sum{this->denom_};
            std::shuffle(indices_.begin(), indices_.end(), gen_);

            for (i = 0; i<keep_n; i++) {
                auto idx = indices_[i];
                auto size = origin[idx];
                this->sizes_[idx] = size;
                rest_num_sum -= size.numerator();
            }
            std::size_t curr_max_num{rest_num_sum-change_n_+1};
            fill_rest(rest_num_sum, curr_max_num, i);
            return this->sizes_;
        }
    };

    template<class Type, template<class> class Distribution>
    class numeric_interval_generator {
    private:
        std::mt19937 _gen;
        Distribution<Type> _distrib;

    public:
        explicit numeric_interval_generator(const Type& min, const Type& max)
                :_gen{std::random_device{}()}, _distrib{min, max}
        {
            if (!(min<max)) throw std::invalid_argument("Maximum has  to be greater than minimum");
        }

        Type operator()()
        {
            return _distrib(_gen);
        }
    };

    template<class Type>
    using real_interval_generator = numeric_interval_generator<Type, std::uniform_real_distribution>;

    template<class Type>
    using int_interval_generator = numeric_interval_generator<Type, std::uniform_int_distribution>;

    class int_range_generator : public trading::int_range {
        std::mt19937 gen_;
        std::uniform_int_distribution<int> distrib_;
        std::size_t change_span_;

    public:
        using value_type = int_range::value_type;

        explicit int_range_generator(int from, int to, int step, std::size_t change_span = 1)
                :trading::int_range(from, to, step), change_span_{change_span}
        {
            int max_span = static_cast<int>(n_vals_)/2;
            if (!change_span_ || change_span>max_span)
                throw std::invalid_argument(fmt::format("Change span has to be in interval [1,{}]", max_span));
        }

        int operator()()
        {
            int positive_step = std::abs(step_);
            distrib_.param(
                    std::uniform_int_distribution<int>::param_type{min_/positive_step, max_/positive_step});
            int val = distrib_(gen_)*positive_step;
            assert(val>=min_ && val<=max_);
            return val;
        }

        int operator()(int origin)
        {
            int positive_step = std::abs(step_);
            int curr_min = origin-static_cast<int>(change_span_*positive_step);
            int curr_max = origin+static_cast<int>(change_span_*positive_step);
            distrib_.param(
                    std::uniform_int_distribution<int>::param_type{curr_min/positive_step, curr_max/positive_step});
            int val = distrib_(gen_)*positive_step;
            assert(val>=curr_min && val<=curr_max);

            if (val<min_) val += static_cast<int>(n_vals_*positive_step);
            if (val>max_) val -= static_cast<int>(n_vals_*positive_step);
            assert(val>=min_ && val<=max_);
            return val;
        }
    };

    template<std::size_t n_levels>
    class configuration_generator {
        random::sizes_generator<n_levels> rand_sizes_;
        random::levels_generator<n_levels> rand_levels_;
        random::int_range_generator rand_period_;
        std::mt19937 gen_{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> coin_flip_{0, 1};

    public:
        explicit configuration_generator(const random::sizes_generator<n_levels>& rand_sizes,
                random::levels_generator<n_levels> rand_levels, const random::int_range_generator& rand_period)
                :rand_sizes_(rand_sizes), rand_levels_(std::move(rand_levels)), rand_period_(rand_period) { }

        bazooka::configuration<n_levels> operator()()
        {
            auto ma = (coin_flip_(gen_)) ? bazooka::indicator_tag::sma : bazooka::indicator_tag::ema;
            return bazooka::configuration<n_levels>{ma, static_cast<std::size_t>(rand_period_()), rand_levels_(),
                                                    rand_sizes_()};
        }
    };
}

#endif //BACKTESTING_RANDOM_GENERATORS_HPP
