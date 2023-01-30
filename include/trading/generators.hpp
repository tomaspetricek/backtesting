//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_GENERATORS_HPP
#define BACKTESTING_GENERATORS_HPP

#include <array>
#include <numeric>
#include <random>
#include <trading/types.hpp>
#include <cppcoro/recursive_generator.hpp>
#include <etl/flat_set.h>
#include <fmt/format.h>

namespace trading {
    template<std::size_t n_sizes>
    class sizes_generator {
    protected:
        static_assert(n_sizes>1);
        std::array<fraction_t, n_sizes> sizes_;
        std::size_t denom_;
        std::size_t max_num_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (!n_unique)
                throw std::invalid_argument{"The number of unique sizes has to be greater than zero"};
            return n_unique;
        }

    public:
        explicit sizes_generator(std::size_t n_unique)
                :denom_(n_sizes+validate_n_unique(n_unique)-1), max_num_(denom_-n_sizes+1) { }
    };

    template<std::size_t n_levels>
    class levels_generator {
        static_assert(n_levels>0);
    protected:
        std::array<fraction_t, n_levels> levels_;
        std::size_t denom_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (n_unique<n_levels)
                throw std::invalid_argument{
                        "The number of unique fractions must be greater than or equal to the number of levels"};
            return n_unique;
        }

    public:
        explicit levels_generator(size_t n_unique = n_levels)
                :denom_(validate_n_unique(n_unique)+1) { }
    };

    template<typename Type>
    class range {
        static_assert(!std::is_floating_point<Type>::value);
    protected:
        Type from_;
        Type to_;
        Type step_;

        static Type validate_step(Type step)
        {
            if (step==0) throw std::invalid_argument("Step cannot be zero");
            return step;
        }

        range(const Type& from, const Type& to, Type step)
                :from_{from}, to_{to}, step_(validate_step(step))
        {
            if (from==to) {
                throw std::invalid_argument("From and to has to be different");
            }
            else if (from>to) {
                if ((from-to)%step!=0)
                    throw std::invalid_argument("Difference between from and to must be multiple of step");

                if (step>0)
                    throw std::invalid_argument("Step has to be lower than 0 if begin is greater than end");
            }
            else if (from<to) {
                if ((to-from)%step!=0)
                    throw std::invalid_argument("Difference between from and to must be multiple of step");

                if (step<0)
                    throw std::invalid_argument("Step has to be greater than 0 if begin is lower than end");
            }
        }

    public:
        Type step() const
        {
            return step_;
        }

        Type from() const
        {
            return from_;
        }

        Type to() const
        {
            return to_;
        }
    };

    namespace random {
        template<std::size_t n_levels>
        class levels_generator : trading::levels_generator<n_levels> {
            std::mt19937 gen_;
            std::vector<fraction_t> all_options_;
            std::array<std::size_t, n_levels> indices_;
            std::size_t change_n_;

            std::size_t validate_change_n(std::size_t change_n)
            {
                if (change_n==0 || change_n>n_levels)
                    throw std::invalid_argument(fmt::format("Change n has to be in interval [1,{}]", n_levels));
                return change_n;
            }

        public:
            using value_type = std::array<fraction_t, n_levels>;

            explicit levels_generator(size_t n_unique = n_levels, std::size_t change_n = 1)
                    :trading::levels_generator<n_levels>(n_unique), gen_{std::random_device{}()}, change_n_{
                    validate_change_n(change_n)}
            {
                all_options_.reserve(n_unique);
                for (std::size_t i{0}; i<n_unique; i++)
                    all_options_.emplace_back(i+1, this->denom_);

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

            std::size_t validate_change_n(std::size_t change_n)
            {
                if (!(change_n>=min_change_n && change_n<=n_sizes))
                    throw std::invalid_argument(
                            fmt::format("Change n has to be in interval [{},{}]", min_change_n, change_n));
                return change_n;
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
                     distrib_(1, this->max_num_), change_n_{validate_change_n(change_n)}
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
                    rest_num_sum -= (size*this->denom_).numerator();
                }
                std::size_t curr_max_num{rest_num_sum-change_n_+1};
                fill_rest(rest_num_sum, curr_max_num, i);
                return this->sizes_;
            }
        };

        template<class Type, template<class> class Distribution>
        class numeric_range_generator {
        private:
            std::mt19937 _gen;
            Distribution<Type> _distrib;

        public:
            explicit numeric_range_generator(const Type& from, const Type& to)
                    :_gen{std::random_device{}()}, _distrib{from, to} { }

            Type operator()()
            {
                return _distrib(_gen);
            }
        };

        template<class Type>
        using real_range_generator = numeric_range_generator<Type, std::uniform_real_distribution>;

        template<class Type>
        using int_range_generator = numeric_range_generator<Type, std::uniform_int_distribution>;

        template<typename Type>
        class range : public trading::range<Type> {
            std::mt19937 gen_;
            std::uniform_int_distribution<std::size_t> distrib_;

        public:
            using value_type = Type;

            explicit range(Type from, Type to, Type step)
                    :trading::range<Type>(from, to, step)
            {
                distrib_.param(std::uniform_int_distribution<std::size_t>::param_type{from/step, to/step});
            }

            Type operator()()
            {
                return distrib_(gen_)*this->step_;
            }
        };
    }

    namespace systematic {
        // generates unique subsequent fractions in interval (0, max), such as max is in interval (0.0, 1.0]
        template<std::size_t n_levels>
        class levels_generator : trading::levels_generator<n_levels> {
        public:
            using value_type = cppcoro::recursive_generator<std::array<fraction_t, n_levels>>;

            explicit levels_generator(size_t n_unique = n_levels)
                    :trading::levels_generator<n_levels>(n_unique) { }

            value_type operator()()
            {
                co_yield generate<0>(this->denom_);
            }

        private:
            template<std::size_t depth = n_levels>
            requires (depth==n_levels)
            value_type generate(std::size_t) { co_yield this->levels_; }

            template<std::size_t depth = 0>
            requires (depth<n_levels)
            value_type generate(std::size_t prev_num)
            {
                for (std::size_t num{--prev_num}; num>n_levels-depth-1; num--) {
                    std::get<depth>(this->levels_) = fraction_t{num, this->denom_};
                    co_yield generate<depth+1>(num);
                }
            }
        };

        // generates fractions that add up to 1.0
        template<std::size_t n_sizes>
        class sizes_generator : public trading::sizes_generator<n_sizes> {
        public:
            using value_type = cppcoro::recursive_generator<std::array<fraction_t, n_sizes>>;

            explicit sizes_generator(size_t n_unique)
                    :trading::sizes_generator<n_sizes>(n_unique) { }

            value_type operator()()
            {
                co_yield generate<0>(this->denom_);
            }

        private:
            template<std::size_t depth = n_sizes>
            requires (depth+1==n_sizes)
            value_type generate(std::size_t remaining)
            {
                std::get<depth>(this->sizes_) = fraction_t{remaining, this->denom_};
                co_yield this->sizes_;
            }

            template<std::size_t depth = 0>
            requires (depth+1<n_sizes)
            value_type generate(std::size_t remaining)
            {
                std::size_t max = (remaining>this->max_num_) ? this->max_num_ : remaining-1;
                for (std::size_t size{1}; size<=max; size++) {
                    std::get<depth>(this->sizes_) = fraction_t{size, this->denom_};
                    co_yield generate<depth+1>(remaining-size);
                }
            }
        };

        // based on: https://stackoverflow.com/questions/7185437/is-there-a-range-class-in-c11-for-use-with-range-based-for-loops
        template<typename Type>
        class range : public trading::range<Type> {
        public:
            using value_type = cppcoro::generator<Type>;

            range(Type from, Type to, Type step)
                    :trading::range<Type>(from, to, step) { }

            value_type operator()()
            {
                for (Type num{this->from_}; num<=this->to_; num += this->step_)
                    co_yield num;
            }
        };
    }
}

#endif //BACKTESTING_GENERATORS_HPP
