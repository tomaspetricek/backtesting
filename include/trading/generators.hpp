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

template<class Type, class Comp>
void insert_sorted(Type* arr, std::size_t n, Type val, const Comp& comp)
{
    std::size_t i{n-1};
    for (; !comp(arr[i], val) && i>=0; i--)
        arr[i+1] = arr[i];
    arr[i+1] = val;
}

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
//        constexpr static fraction_t default_max{1.0};
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

    namespace random {
        template<std::size_t n_levels>
        class levels_generator : trading::levels_generator<n_levels> {
            std::mt19937 gen_;
            std::vector<fraction_t> all_options_;
            std::array<std::size_t, n_levels> indices_;

        public:
            using result_type = std::array<fraction_t, n_levels>;

            explicit levels_generator(size_t n_unique = n_levels)
                    :trading::levels_generator<n_levels>(n_unique), gen_{std::random_device{}()}
            {
                all_options_.reserve(n_unique);
                for (std::size_t i{0}; i<n_unique; i++)
                    all_options_.emplace_back(i+1, this->denom_);

                for (std::size_t i{0}; i<n_levels; i++)
                    indices_[i] = i;
            }

            const result_type& operator()()
            {
                std::shuffle(all_options_.begin(), all_options_.end(), gen_);
                std::sort(all_options_.begin(), all_options_.begin()+static_cast<int>(n_levels), std::greater<>());
                std::copy(all_options_.begin(), all_options_.begin()+static_cast<int>(n_levels), this->levels_.begin());
                return this->levels_;
            }

            const result_type& operator()(const result_type& origin, std::size_t change_n)
            {
                assert(change_n<=n_levels);
                std::size_t keep{n_levels-change_n};
                std::shuffle(indices_.begin(), indices_.end(), gen_);
                std::sort(indices_.begin(), indices_.begin()+static_cast<int>(keep));
                std::size_t i;

                for (i = 0; i<keep; i++)
                    this->levels_[i] = origin[indices_[i]];

                std::shuffle(all_options_.begin(), all_options_.end(), gen_);
                auto options_it = all_options_.begin();

                for (; i<n_levels; i++) {
                    while (std::binary_search(this->levels_.begin(), this->levels_.begin()+i, *options_it,
                            std::greater<>())) options_it++;
                    insert_sorted(this->levels_.data(), i, *options_it, std::greater<>());
                }
                return this->levels_;
            }
        };

        template<std::size_t n_sizes>
        class sizes_generator : public trading::sizes_generator<n_sizes> {
            std::mt19937 gen_;
            std::uniform_int_distribution<std::size_t> distrib_;
            std::array<std::size_t, n_sizes> indices_;

        public:
            using result_type = std::array<fraction_t, n_sizes>;

            explicit sizes_generator(size_t n_unique)
                    :trading::sizes_generator<n_sizes>(n_unique), gen_(std::random_device{}()),
                     distrib_(1, this->max_num_)
            {
                for (std::size_t i{0}; i<indices_.size(); i++)
                    indices_[i] = i;
            }

            const result_type& operator()()
            {
                std::size_t num, remaining{this->denom_};
                std::size_t curr_max_num{this->max_num_};
                std::shuffle(indices_.begin(), indices_.end(), gen_);

                for (std::size_t i{0}; i<n_sizes-1; i++) {
                    distrib_.param(std::uniform_int_distribution<std::size_t>::param_type{1, curr_max_num});
                    num = distrib_(gen_);
                    this->sizes_[indices_[i]] = fraction_t{num, this->denom_};
                    remaining -= num;
                    curr_max_num = (curr_max_num==num) ? 1 : curr_max_num-num;
                }
                this->sizes_[indices_.back()] = fraction_t{remaining, this->denom_};
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
    }

    namespace systematic {
        // generates unique subsequent fractions in interval (0, max), such as max is in interval (0.0, 1.0]
        template<std::size_t n_levels>
        class levels_generator : trading::levels_generator<n_levels> {
        public:
            using result_type = cppcoro::recursive_generator<std::array<fraction_t, n_levels>>;

            explicit levels_generator(size_t n_unique = n_levels)
                    :trading::levels_generator<n_levels>(n_unique) { }

            result_type operator()()
            {
                co_yield generate<0>(this->denom_);
            }

        private:
            template<std::size_t depth = n_levels>
            requires (depth==n_levels)
            result_type generate(std::size_t) { co_yield this->levels_; }

            template<std::size_t depth = 0>
            requires (depth<n_levels)
            result_type generate(std::size_t prev_num)
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
            using result_type = cppcoro::recursive_generator<std::array<fraction_t, n_sizes>>;

            template<std::size_t depth = n_sizes>
            requires (depth+1==n_sizes)
            result_type generate(std::size_t remaining)
            {
                std::get<depth>(this->sizes_) = fraction_t{remaining, this->denom_};
                co_yield this->sizes_;
            }

            template<std::size_t depth = 0>
            requires (depth+1<n_sizes)
            result_type generate(std::size_t remaining)
            {
                std::size_t max = (remaining>this->max_num_) ? this->max_num_ : remaining-1;
                for (std::size_t size{1}; size<=max; size++) {
                    std::get<depth>(this->sizes_) = fraction_t{size, this->denom_};
                    co_yield generate<depth+1>(remaining-size);
                }
            }

        public:
            explicit sizes_generator(size_t n_unique)
                    :trading::sizes_generator<n_sizes>(n_unique) { }

            result_type operator()()
            {
                co_yield generate<0>(this->denom_);
            }
        };
    }
}

#endif //BACKTESTING_GENERATORS_HPP
