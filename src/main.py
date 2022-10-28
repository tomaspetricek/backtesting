import numpy as np


# https://plainenglish.io/blog/how-to-calculate-the-ema-of-a-stock-with-python
def ema(prices, period, smoothing=2):
    vals = [sum(prices[:period]) / period]

    for price in prices[period:]:
        vals.append((price * (smoothing / (1 + period))) + vals[-1] * (1 - (smoothing / (1 + period))))

    return vals


def main():
    samples = np.array([1, 2, 3, 7, 9])
    period = 3
    print(ema(samples, period))


if __name__ == "__main__":
    main()
