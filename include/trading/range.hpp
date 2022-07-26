//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_RANGE_HPP
#define EMASTRATEGY_RANGE_HPP

namespace trading {
// based on: https://stackoverflow.com/questions/7185437/is-there-a-range-class-in-c11-for-use-with-range-based-for-loops
    template<typename Type>
    class range {
    public:
        class iterator {
            friend class range;

        public:
            Type operator*() const { return val_; }
            const iterator& operator++()
            {
                val_ += step_;
                return *this;
            }

            iterator operator++(int)
            {
                iterator copy(*this);
                val_ += step_;
                return copy;
            }

            bool operator==(const iterator& other) const { return val_==other.val_; }
            bool operator!=(const iterator& other) const { return val_!=other.val_; }

        public:
            explicit iterator(Type val, Type step)
                    :val_(val), step_(step) { }

        private:
            Type val_;
            Type step_;
        };

        iterator begin() const { return begin_; }
        iterator end() const { return end_; }
        Type step() const { return step_; }

        range(const Type& begin, const Type& end, Type step)
                :step_(validate_step(step)), begin_(begin, step), end_(end, step)
        {
            if (begin==end) {
                throw std::invalid_argument("Begin and end has to be different");
            }
            else if (begin>end) {
                if ((begin-end)%step!=0)
                    throw std::invalid_argument("Difference between begin and end must be multiple of step");

                if (step>0)
                    throw std::invalid_argument("Step has to be lower than 0 if begin is greater than end");
            }
            else if (begin<end) {
                if ((end-begin)%step!=0)
                    throw std::invalid_argument("Difference between end and begin must be multiple of step");

                if (step<0)
                    throw std::invalid_argument("Step has to be greater than 0 if begin is lower than end");
            }
        }

    private:
        Type step_;
        iterator begin_;
        iterator end_;

        static Type validate_step(Type step)
        {
            if (step==0)
                throw std::invalid_argument("Step cannot be zero");

            return step;
        }
    };
}

#endif //EMASTRATEGY_RANGE_HPP
