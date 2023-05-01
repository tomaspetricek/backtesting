//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef BACKTESTING_EXCEPTION_HPP
#define BACKTESTING_EXCEPTION_HPP

#include <exception>

namespace trading {
    class not_ready : public std::logic_error {
    public:
        explicit not_ready(const std::string& msg)
                :std::logic_error{msg} { }

        ~not_ready() noexcept override = default;

        const char* what() const noexcept override
        {
            return logic_error::what();
        }
    };

    class bad_formatting : public std::runtime_error {
    public:
        explicit bad_formatting(const std::string& msg)
                :runtime_error(msg) { }

        ~bad_formatting() noexcept override = default;

        const char* what() const noexcept override
        {
            return runtime_error::what();
        }
    };

    class insufficient_funds : public std::runtime_error {
    public:
        explicit insufficient_funds(const std::string& msg = "Not enough funds to perform operation")
                :std::runtime_error(msg) { }

        ~insufficient_funds() noexcept override = default;

        const char* what() const noexcept override
        {
            return runtime_error::what();
        }
    };

    // src: https://en.cppreference.com/w/cpp/error/nested_exception
    void print_exception(const std::exception& e, int level = 0)
    {
        std::cerr << std::string(level, '\t') << "exception: " << e.what() << '\n';

        try {
            std::rethrow_if_nested(e);
        }
        catch (const std::exception& nested_exception) {
            print_exception(nested_exception, level+1);
        }
        catch (...) { }
    }
}

#endif //BACKTESTING_EXCEPTION_HPP
