# -*- coding: utf-8 -*-

from py_defs import *
from py_defs import Decimal as Price
from common_enums import *


class Params:
    pass


def init(strat, config):
    Params.VOLUME = config.get('VOLUME', 1)
    Params.MAX_POS = config.get('MAX_POS', 1)
    Params.OFFSET = Price(config.get('OFFSET', 18))

    strat.set_max_total_amount(Params.MAX_POS)


def amount_available(pos, dir):
    max_volume = min(Params.MAX_POS - dir_sign(dir) * pos, Params.VOLUME)
    return max(max_volume, 0)


def trading_book_update(strat, order_book):
    orders = order_book.orders()
    middle_price = order_book.middle_price()
    pos = strat.executed_amount()

    strat.add_chart_point('middle_price', middle_price)

    for dir in (BID, ASK):
        target_price = middle_price - dir_sign(dir) * Params.OFFSET
        order_amount = amount_available(pos, dir)

        if orders.active_orders_count(dir) == 0:
            if order_amount > 0:
                strat.add_limit_order(dir, target_price, order_amount)
        else:
            current_order = orders.orders_by_dir(dir)[0]
            if current_order.price() != target_price:
                strat.delete_order(current_order)
                if order_amount > 0:
                    strat.add_limit_order(dir, target_price, order_amount)