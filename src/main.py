import numpy as np
import pandas as pd
import os
from datetime import datetime
import matplotlib.pyplot as plt
import warnings
from widgets import CandlestickWidget
np.set_printoptions(precision=4, suppress=True)


def get_series(csv_path, sep=',', time_col="time"):
    series = pd.read_csv(csv_path, sep)
    series.drop(index=0)
    series[time_col] = series[time_col].apply(lambda x: datetime.utcfromtimestamp(x))
    series.set_index(time_col, inplace=True)
    return series


def main():
    out_dir = "data/out/brute force/56-doge"
    series_dir = os.path.join(out_dir, "best-series")
    in_dir = "./data/in"
    candles_path = os.path.join(in_dir, "ohlcv-eth-usdt-1-min.csv")

    column_names = ["time", "open", "high", "low", "close"]
    candle_series = pd.read_csv(candles_path, '|', names=column_names, usecols=range(5), header=None)
    candle_series.iloc[:, 0] = candle_series.iloc[:, 0].apply(lambda x: datetime.utcfromtimestamp(x))
    candle_series.set_index("time", inplace=True)

    # get series
    open_order_series = get_series(os.path.join(series_dir, "open-order-series.csv"))
    close_order_series = get_series(os.path.join(series_dir, "close-order-series.csv"))
    exit_indic_series = get_series(os.path.join(series_dir, "exit-indic-series.csv"))
    entry_indic_series = get_series(os.path.join(series_dir, "entry-indic-series.csv"))

    widget = CandlestickWidget(candle_series, entry_indic_series, exit_indic_series, open_order_series,
                               close_order_series, "title")
    widget.show()


if __name__ == "__main__":
    main()
