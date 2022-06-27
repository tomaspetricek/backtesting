//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_EXCEPTIONS_H
#define EMASTRATEGY_EXCEPTIONS_H

#include <exception>

class not_ready : public std::logic_error {
public:
    explicit not_ready(const std::string& msg)
            :std::logic_error{msg} { }

    ~not_ready() noexcept override = default;

    const char* what() const _NOEXCEPT override
    {
        return logic_error::what();
    }
};

class bad_formatting : public std::runtime_error {
public:

    explicit bad_formatting(const std::string& string)
            :runtime_error(string) { }

    ~bad_formatting() noexcept override = default;

    const char* what() const _NOEXCEPT override
    {
        return runtime_error::what();
    }
};

#endif //EMASTRATEGY_EXCEPTIONS_H
