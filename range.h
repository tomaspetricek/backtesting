//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_RANGE_H
#define EMASTRATEGY_RANGE_H

// based on: https://stackoverflow.com/questions/7185437/is-there-a-range-class-in-c11-for-use-with-range-based-for-loops
template<typename Type>
class range {
public:
    class iterator {
        friend class range;
    public:
        Type operator*() const { return i_; }
        const iterator& operator++()
        {
            ++i_;
            return *this;
        }
        iterator operator++(int)
        {
            iterator copy(*this);
            ++i_;
            return copy;
        }

        bool operator==(const iterator& other) const { return i_==other.i_; }
        bool operator!=(const iterator& other) const { return i_!=other.i_; }

    protected:
        explicit iterator(Type start)
                :i_(start) { }

    private:
        unsigned long i_;
    };

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

    range(Type begin, Type end)
            :begin_(begin), end_(end) { }
private:
    iterator begin_;
    iterator end_;
};

#endif //EMASTRATEGY_RANGE_H
