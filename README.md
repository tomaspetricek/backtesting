# Backtesting
The purpose of backtesting is to find the optimal configuration for a given strategy for given price historical data.
A class hierarchy was created using namespaces, which are used to distinguish classes of the same name  (`bazooka::long_strategy`, `triple_ema::long_strategy`).
One of the goals of the design was that class names should make sense without their namespace.
All classes and functions share a common namespace called `trading`.

## Strategy
A strategy decides, based on the historical price provided, whether to buy (`action::buy`), sell (`action::sell`), sell all (`action::sell_all`) or do nothing (`action::do_nothing`).
Internally, it uses trading indicators and makes decisions based on their values.
Each strategy should have its own namespace like `trading::triple_ema` and `trading::bazooka`.
The `trading::strategy` abstract class should be used to implement a specific strategy.
Strategies can be divided into long and short strategies, such as `triple_ema::long_strategy` and `triple_ema::short_strategy`.
When using a long strategy, a trader tries to buy low and sell high. It is the exact opposite with short strategies.

## Trade Manager
A trade manager goes hand in hand with trading strategy.
Based on a decisions made by a strategy, *positions* are created.
They can be either open or close and together they make a trade.
A trade is initiated by creating an open position.
Additional open positions can be added.
The trade is closed once all purchased assets are sold.
Assets are sold by creating close positions.
Each strategy has its own trade manager such as `triple_ema::trade_manager`.
They should inherit from the `trading::trade_manager` class.

## Optimizer
Optimizers are a set of algorithms whose purpose is to find the best possible configuration for a given strategy and given historical data.
The `optimizer::base` class encapsulates what optimization algorithms have in common. All optimization algorithms should inherit from it.
They all optimize some *objective function* that has some set of parameters, generally called *configurations*, and input arguments that are used to generate the configurations. 

### Brute Force
Brute force optimizers go through all possible states in a given search space.
As of now only two of them are available `cartesian_product::brute_force` and `sliding::brute_force`.
They differ only in the properties of the search space.
Both algorithms could be implemented using nested loops, but since the number of nested loops can vary, they are implemented using recursion.
Cartesian product nested loops are independent of each other, on the other hand sliding space nested loops start with the next value of the previous loop.

## Formulas
There are two types of formulas and they differ only in the return type, so function overloading cannot be used and an additional namespace had to be created.
One group of formulas returns an amount and the other a percentage, so the `amount::formula` and `percent::formula` namespaces were created accordingly.
Percent and amount are both of type double, to ensure more type safety, new strong types `amount_t` and `percent_t` were created using the __strong_type.hpp__ header from the Bjorn Fahller's [github repository](https://github.com/rollbear/strong_type).
