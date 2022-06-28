#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "indicator.h"
#include "strategy.h"
#include "read.h"
#include "position.h"

using namespace trading;

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
    // create strategy
    std::shared_ptr<strategy::long_triple_ema> strategy;

    try {
        indicator::ema short_ema{15};
        indicator::ema middle_ema{30};
        indicator::ema long_ema{60};
        strategy = std::make_shared<strategy::long_triple_ema>(short_ema, middle_ema, long_ema);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot create strategy"));
    }

    // read candles
    std::filesystem::path candle_csv("../data/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    std::vector<candle> candles;

    try {
        candles = read_candles(candle_csv, ';', period);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot read candles"));
    }
    
    assert(!candles.empty());

    std::shared_ptr<long_order> order;
    std::shared_ptr<long_position> pos;
    std::vector<std::shared_ptr<long_position>> closed;

    double pos_size = 100;

    // collect positions
    for (const auto& candle : candles) {
        order.reset((*strategy)(candle.get_close()));

        if (order) {
            auto point = trading::point(candle.get_close(), candle.get_created());

            // open position
            if (order->action()==action::buy) {
                pos = std::make_shared<long_position>(point, pos_size);
            }
            // close position
            else if (order->action()==action::sell) {
                pos->set_exit(point);
                closed.push_back(pos);
                std::cout << pos->get_percent_gain() << std::endl;
            }
        }
    }

    std::cout << "done" << std::endl;
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