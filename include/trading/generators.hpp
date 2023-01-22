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

namespace trading {
    template<std::size_t n_sizes>
    class sizes_generator {
    protected:
        static_assert(n_sizes>1);
        std::array<fraction_t, n_sizes> sizes_;
        std::size_t n_fracs_;
        std::size_t max_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (!n_unique)
                throw std::invalid_argument{"The number of unique fractions has to be greater than zero"};
            return n_unique;
        }

    public:
        explicit sizes_generator(std::size_t n_unique)
                :n_fracs_(n_sizes+validate_n_unique(n_unique)-1), max_(n_fracs_-n_sizes+1) { }
    };

    template<std::size_t n_levels>
    class levels_generator {
        static_assert(n_levels>0);
    protected:
//        constexpr static fraction_t default_max{1.0};
        std::array<fraction_t, n_levels> levels_;
        std::size_t n_fracs_;

        std::size_t validate_n_unique(std::size_t n_unique)
        {
            if (n_unique<n_levels)
                throw std::invalid_argument{
                        "The number of unique fractions must be greater than or equal to the number of levels"};
            return n_unique;
        }

    public:
        explicit levels_generator(size_t n_unique = n_levels)
                :n_fracs_(validate_n_unique(n_unique)+1) { }
    };

    namespace random {
        template<std::size_t n_levels>
        class levels_generator : trading::levels_generator<n_levels> {
            std::mt19937 gen_;
            std::vector<fraction_t> nums_;

        public:
            using return_type = std::array<fraction_t, n_levels>;

            explicit levels_generator(size_t n_unique = n_levels)
                    :trading::levels_generator<n_levels>(n_unique), gen_{std::random_device{}()}
            {
                nums_.reserve(n_unique);
                for (std::size_t i{0}; i<n_unique; i++)
                    nums_.emplace_back(static_cast<double>(i+1)/this->n_fracs_);
            }

            const return_type& operator()()
            {
                std::shuffle(nums_.begin(), nums_.end(), gen_);
                std::sort(nums_.begin(), nums_.begin()+static_cast<int>(n_levels), std::greater<>());
                std::copy(nums_.begin(), nums_.begin()+static_cast<int>(n_levels), this->levels_.begin());
                return this->levels_;
            }
        };

        template<std::size_t n_sizes>
        class sizes_generator : public trading::sizes_generator<n_sizes> {
            std::mt19937 gen_;
            std::uniform_int_distribution<std::size_t> distrib_;
            std::array<std::size_t, n_sizes> indices_;

        public:
            using return_type = std::array<fraction_t, n_sizes>;

            explicit sizes_generator(size_t n_unique)
                    :trading::sizes_generator<n_sizes>(n_unique), gen_(std::random_device{}()),
                     distrib_(1, this->max_)
            {
                for (std::size_t i{0}; i<indices_.size(); i++)
                    indices_[i] = i;
            }

            const return_type& operator()()
            {
                std::size_t remaining{this->n_fracs_};
                std::size_t num, curr_max;
                std::shuffle(indices_.begin(), indices_.end(), gen_);

                for (std::size_t i{0}; i<n_sizes-1; i++) {
                    curr_max = (remaining>this->max_) ? this->max_ : remaining-1;
                    distrib_.param(std::uniform_int_distribution<std::size_t>::param_type{1, curr_max});
                    num = distrib_(gen_);
                    this->sizes_[indices_[i]] = static_cast<fraction_t>(num)/this->n_fracs_;
                    remaining -= num;
                }
                this->sizes_[indices_.back()] = static_cast<fraction_t>(remaining)/this->n_fracs_;
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
            using return_type = cppcoro::recursive_generator<std::array<fraction_t, n_levels>>;

            explicit levels_generator(size_t n_unique = n_levels)
                    :trading::levels_generator<n_levels>(n_unique) { }

            return_type operator()()
            {
                co_yield generate<0>(this->n_fracs_);
            }

        private:
            template<std::size_t depth = n_levels>
            requires (depth==n_levels)
            return_type generate(std::size_t) { co_yield this->levels_; }

            template<std::size_t depth = 0>
            requires (depth<n_levels)
            return_type generate(std::size_t prev_num)
            {
                for (std::size_t num{--prev_num}; num>n_levels-depth-1; num--) {
                    std::get<depth>(this->levels_) = static_cast<double>(num)/this->n_fracs_;
                    co_yield generate<depth+1>(num);
                }
            }
        };

        // generates fractions that add up to 1.0
        template<std::size_t n_sizes>
        class sizes_generator : public trading::sizes_generator<n_sizes> {
            using return_type = cppcoro::recursive_generator<std::array<fraction_t, n_sizes>>;

            template<std::size_t depth = n_sizes>
            requires (depth+1==n_sizes)
            return_type generate(std::size_t remaining)
            {
                std::get<depth>(this->sizes_) = static_cast<double>(remaining)/this->n_fracs_;
                co_yield this->sizes_;
            }

            template<std::size_t depth = 0>
            requires (depth+1<n_sizes)
            return_type generate(std::size_t remaining)
            {
                std::size_t max = (remaining>this->max_) ? this->max_ : remaining-1;
                for (std::size_t size{1}; size<=max; size++) {
                    std::get<depth>(this->sizes_) = static_cast<double>(size)/this->n_fracs_;
                    co_yield generate<depth+1>(remaining-size);
                }
            }

        public:
            explicit sizes_generator(size_t n_unique)
                    :trading::sizes_generator<n_sizes>(n_unique) { }

            return_type operator()()
            {
                co_yield generate<0>(this->n_fracs_);
            }
        };
    }
}

#endif //BACKTESTING_GENERATORS_HPP
