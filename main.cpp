#include <iostream>
#include <vector>
#include <typeinfo>

#include "exceptions.h"
#include "indicator.h"

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

void run()
{
    std::vector<double> prices{10, 11, 11.5, 10.75, 12, 11.75, 12.25, 14, 16, 17, 15.6,
                               15.75, 16, 14, 16.5, 17, 17.25, 18, 18.75, 20};

    // create indicator
    int period{5};
    ema indicator{period};

    if (prices.size()<period+1)
        throw std::logic_error("Number of prices has to be greater than period + 1");

    std::vector<double> vals;
    vals.reserve(prices.size()-(period-1));

    // collect indicator values
    for (const auto& price : prices) {
        try {
            vals.emplace_back(indicator(price));
        }
        catch (const not_ready& ex) {
            std::cerr << "Cannot calculate ema value" << std::endl
                      << ex.what() << std::endl;
        }
        catch (...) {
            std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
        }
    }

    // display indicator values
    for (const auto& val : vals)
        std::cout << val << " ";
    std::cout << std::endl;
}

int main()
{
    try {
        run();
    }
    catch (const std::exception& e) {
        print_exception(e);
    }

    return 0;
}