# Backtesting
The purpose of backtesting is to find the optimal configuration for a given strategy and given historical prices.
A class hierarchy was created using namespaces, which are used to distinguish classes of the same name  (`bazooka::long_strategy`, `triple_ema::long_strategy`).
Other naming conventions were inspired by the `Standard Library` and `Boost Libraries`. 
One of the goals of the design was that class names should make sense without their namespace.
All classes and functions share a common namespace called `trading`.

## Trader
A trader needs two things to function and that is a __strategy__ to decide when to create_open_order or create_close_order and a __manager__ to manage the trades.
The `trading::trader` class inherits from a strategy and a trade manager bringing their functionalities together.
Both strategy and trade manager are represented by abstract classes (`trading::strategy`, `trading::manager`) from which all concrete classes should inherit.
They use *static polymorphism* implemented using __CRTP__ (Curiously recurring template pattern) to avoid use of virtual methods which can be slower.

### Strategy
A strategy decides, based on the historical prices provided, whether to create_open_order, create_close_order, create_close_order all or do nothing.
Concrete strategies should inherit from the `trading::strategy` class and implement methods defined by static polymorphism (`should_open_impl`, `should_close_impl`, ...).
Internally, it uses trading indicators and makes decisions based on their values.
Each strategy should have its own namespace like `trading::triple_ema` and `trading::bazooka`.
Strategies can be divided into long and short strategies, such as `triple_ema::long_strategy` and `triple_ema::short_strategy`.
When using a long strategy, a trader tries to create_open_order low and create_close_order high. It is the exact opposite with short strategies.

### Trade Manager
A trade manager goes hand in hand with trading strategy.
It takes care of active trade: creating opening and closing positions.
It inherits from the `trading::manager` abstract class which provides 3 main public methods `create_open_order`, `create_close_order`, `create_close_all_order`.
A trade is initiated by creating an open position.
Additional open positions can be added.
The trade is closed once all purchased assets are sold.
Assets are sold by creating close positions.
There are currently 2 specific trade managers: `trading::const_size::manager`, which creates positions of the same size, and `trading::varying_size::manager`, which creates positions of different sizes based on their order.

## Optimizer
Optimizers are a set of algorithms whose purpose is to find the best possible configuration for a given strategy and given historical data.
There is currently a single optimizer implemented `optimizer::parallel::brute_force`, which goes through all possible states in a given search space and finds the best one.
Search space is implemented using `cppcoro::generator` (from [CppCoro](https://github.com/lewissbaker/cppcoro) library) which generates all possible states.
It is a __coroutine__ that came with the C++20 standard and allows you to create functions whose execution can be interrupted and resumed later.
Functions retain their state and do not use static variables for it.
It also takes advantage of task parallelism created by using the *OpenMP library*.

## Formulas
There are two types of formulas and they differ only in the return type, so function overloading cannot be used and an additional namespace had to be created.
One group of formulas returns an amount and the other a percentage, so the `amount::formula` and `percent::formula` namespaces were created accordingly.
Percent and amount are both of type double, to ensure type safety more, new strong types `amount_t` and `percent_t` were created using the __strong_type.hpp__ header from the Bjorn Fahller's [github repository](https://github.com/rollbear/strong_type).

## IO
The CSV file reader `io::csv::reader` and writer `io::csv::writer` were created to read candles and save data points for visualization.
The reader parses lines and uses a parser to create the input.
Writer, on the other hand, uses a serializer to produce output.

## Python: Matplotlib visualization widget
A `CandlestickWidget` was created to visually check the strategy.
Creates a candlestick chart that can be scrolled from right to left and can be zoomed.
Uses [mplfinance](https://github.com/matplotlib/mplfinance) to plot the chart.
It is used in the Jupyter Notebook script *src/plot.ipynb* which loads the data points and runs (keeps alive) the widget.

