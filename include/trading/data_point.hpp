//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_DATA_POINT_HPP
#define EMASTRATEGY_DATA_POINT_HPP

namespace trading {
    template<typename Data>
    struct data_point {
        std::time_t occurred_;
        Data value_;

    public:
        data_point(time_t occurred, Data value)
                :occurred_(occurred), value_(value) { }

        const price& value() const
        {
            return value_;
        }

        time_t occurred() const
        {
            return occurred_;
        }
    };
}

#endif //EMASTRATEGY_DATA_POINT_HPP
