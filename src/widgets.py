import numpy as np
import mplfinance as mpf
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from matplotlib.dates import num2date, date2num


class CandlestickWidget:
    def __init__(self, candlestick_points, indic_points, mean_price_points, open_points, closed_points,
                 fig_title, n_points_visible=250):
        self._candlestick_points = candlestick_points
        self._min_time = self._candlestick_points.index[0]
        self._indic_points = indic_points
        self._mean_price_points = mean_price_points
        self._open_points = open_points
        self._closed_points = closed_points
        self._n_points_visible = n_points_visible
        min_time = self._candlestick_points.index[0]
        max_time = self._candlestick_points.index[n_points_visible]
        self._delta_time = max_time - min_time
        self._create_labels()
        self._create_figure(fig_title)
        self._create_slider()

    def _create_figure(self, title):
        # turn off showing of figure after creation
        plt.ioff()

        # create figure
        self._fig = mpf.figure(style='binance', figsize=(10, 6))
        self._fig.suptitle(title)
        self._ax = self._fig.add_subplot(1, 1, 1)
        plt.subplots_adjust(bottom=0.25)  # add space for slider

        # shrink axis to fit legend
        box = self._ax.get_position()
        shrink_ratio = 0.85
        self._ax.set_position([box.x0, box.y0, box.width * shrink_ratio, box.height])

    def _create_labels(self):
        self._labels = [
            'candlestick wick', 'candlestick body',
            *list(self._indic_points), *list(self._mean_price_points),
            "open position", "closed position"
        ]

    def _create_slider(self):
        slider_ax = plt.axes([0.18, 0.05, 0.6, 0.03])
        min_time = self._candlestick_points.index[0]
        slider_min_val = date2num(min_time)
        slider_max_index = len(self._candlestick_points.index) - self._n_points_visible - 1
        slider_max_time = self._candlestick_points.index[slider_max_index]
        slider_max_val = date2num(slider_max_time)
        self._time_slider = Slider(slider_ax, "time", slider_min_val, slider_max_val)

        # add time ticks
        slider_ax.add_artist(slider_ax.xaxis)
        n_ticks = 8
        time_tick_vals = np.linspace(slider_min_val, slider_max_val, n_ticks)
        slider_ax.set_xticks(time_tick_vals, [num2date(s).strftime("%m-%d-%y") for s in time_tick_vals])

        # add update callback
        self._time_slider.on_changed(self._update)

        # make initial update
        self._update(slider_min_val)

    def _plot_data_points(self, min_time, max_time):
        self._ax.clear()
        addons = [
            mpf.make_addplot(self._indic_points.loc[min_time:max_time], ax=self._ax),
            mpf.make_addplot(self._mean_price_points.loc[min_time:max_time], ax=self._ax),
            mpf.make_addplot(self._closed_points.loc[min_time:max_time], ax=self._ax,
                             type='scatter', markersize=200, marker='v'),
        ]

        # add open points - can be more in one time
        for open_points in self._open_points:
            addons.append(mpf.make_addplot(open_points.loc[min_time:max_time], ax=self._ax,
                                           type='scatter', markersize=200, marker='^'))

        mpf.plot(self._candlestick_points.loc[min_time:max_time], ax=self._ax,
                 addplot=addons, type='candle', style='binance')
        self._ax.legend(self._labels, loc='center left', bbox_to_anchor=(1, 0.5))

    def _update(self, min_pos):
        min_time = num2date(min_pos)
        max_time = num2date(min_pos) + self._delta_time
        self._plot_data_points(min_time, max_time)
        self._time_slider.valtext.set_text(min_time.strftime("%m/%d/%Y, %H:%M:%S"))
        self._fig.canvas.draw_idle()

    def show(self):
        self._fig.show()
