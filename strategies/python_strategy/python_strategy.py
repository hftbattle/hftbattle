# -*- coding: utf-8 -*-

from py_defs import *
from py_defs import Decimal as Price
from common_enums import *


def get_current_amount(current_orders):
    return sum(order.amount() for order in current_orders)


def should_strategy_run_from_best_price(dir):
    return False


def delete_soft_second_version(strat, dir, price, current_amount, wanted_amount, current_orders):
    max_after_amount_to_run_ = 42
    trading_book = strat.trading_book()

    if not should_strategy_run_from_best_price(dir):
        manage_amount_on_price(strat, dir, price, wanted_amount, current_orders)
        return

    for order in current_orders:
        if order.status() == OrderStatus.Active:
            amount_after_order = trading_book.best_volume(dir) - strat.amount_before_order(order)
            if (should_strategy_run_from_best_price(dir) and amount_after_order < max_after_amount_to_run_):
                strat.delete_order(order)


def manage_amount_on_best_price(strat, dir, price, wanted_amount, current_orders):
    max_amount_to_run_from_best_ = 20
    max_soft_amount_to_run_from_best_ = 64
    trading_book = strat.trading_book()
    current_amount = get_current_amount(current_orders)

    if trading_book.best_volume(dir) - current_amount < max_amount_to_run_from_best_:
        strat.delete_all_orders_at_price(dir, price)
        return

    if trading_book.best_volume(dir) - current_amount < max_soft_amount_to_run_from_best_:
        delete_soft_second_version(strat, dir, price, current_amount, wanted_amount, current_orders)
        return

    manage_amount_on_price(strat, dir, price, wanted_amount, current_orders)


def manage_amount_on_price(strat, dir, price, wanted_amount, current_orders):
    current_amount = get_current_amount(current_orders)
    amount = wanted_amount - current_amount

    for order in reversed(current_orders):
        if current_amount > wanted_amount:
            current_amount -= order.amount_rest()
            strat.delete_order(order)

    for _ in xrange(amount):
        strat.add_limit_order(dir, price, 1)


def get_wanted_amount(strat, dir, price):
    max_executed_amount_ = 50
    max_amount_at_price_ = 3
    wanted_amount = min(max_executed_amount_ - dir_sign(dir) * strat.executed_amount(), max_amount_at_price_)
    return max(0, wanted_amount)


# docs.participant_strategy.trading_book_update
def trading_book_update(strat, trading_book):
    if strat.server_time_tm().tm_hour() < 0:
        return

    for dir in (BID, ASK):
        for order in trading_book.orders().orders_by_dir(dir):
            if trading_book.index_by_price(dir, order.price()) > 9:
                strat.delete_order(order)
        active_orders = trading_book.orders().orders_by_dir_as_map(dir)

        for idx, quote in enumerate(trading_book.all_quotes(dir)):
            if idx == 5:
                break
            quote_price = quote.price()
            amount = get_wanted_amount(strat, dir, quote_price)
            if quote_price == trading_book.best_price(dir):
                manage_amount_on_best_price(strat, dir, quote_price, amount, active_orders.get(quote_price, []))
            else:
                manage_amount_on_price(strat, dir, quote_price, amount, active_orders.get(quote_price, []))


# docs.participant_strategy.trading_deals_update
def trading_deals_update(strat, deals):
    pass


# docs.participant_strategy.execution_report_update
def execution_report_update(strat, execution_report):
    pass


def init(strat, config):
    strat.set_stop_loss_result(Decimal(-15000.0))
