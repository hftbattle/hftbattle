# !python
# cython: boundscheck=False
# cython: wraparound=False
# cython: initializedcheck=False
# cython: overflowcheck=False
# cython: cdivision=True
# cython: profile=False
# cython: linetrace=False

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool
from libcpp.string cimport string

from libc cimport stdint
ctypedef stdint.uint64_t Id
ctypedef stdint.int32_t Amount

ctypedef stdint.uint64_t uint64_t
ctypedef stdint.uint8_t uint8_t

ctypedef stdint.int64_t int64_t
ctypedef stdint.int32_t int32_t

ctypedef Decimal Price

cimport defs
ctypedef defs.Order* OrderPtr

import traceback

try:
  from strategies.python_strategy.python_strategy import trading_book_update
except:
  pass

try:
  from strategies.python_strategy.python_strategy import trading_deals_update
except:
  pass

try:
  from strategies.python_strategy.python_strategy import execution_report_update
except:
  pass


# Decimal class is used for performing high precision calculations and for price representation (for convenience, it has Price alias).
# You can also get double using get_double() method.
# Note: in C++ you can't print Decimal using cout/cerr/printf, however, you can use our output streams.
# In Python you can use print.
cdef class Decimal:
  cdef const defs.Decimal* _this

  @staticmethod
  cdef inline Decimal _from_this(const defs.Decimal* _this):
    cdef Decimal result = Decimal.__new__(Decimal)
    result._this = _this
    return result

  @staticmethod
  cdef inline Decimal _from_this_tmp(defs.Decimal&& _this):
    return Decimal._from_this(new defs.Decimal(_this))

  @staticmethod
  cdef inline Decimal from_numerator(int64_t num):
    return Decimal._from_this_tmp(defs.Decimal.from_numerator(num))

  def __dealloc__(self):
    del self._this

  def __init__(self, double num=0.0):
    self._this = new defs.Decimal(num)

  def __hash__(self):
    return self._this.get_numerator()

  def __float__(self):
    return self._this.get_double()

  def __str__(self):
    return str(self._this.get_double())

  def __richcmp__(self, rhs, num):
    lhs_numerator = Decimal.count_numerator(self)
    rhs_numerator = Decimal.count_numerator(rhs)
    return {
      0: lambda: lhs_numerator < rhs_numerator,
      2: lambda: lhs_numerator == rhs_numerator,
      4: lambda: lhs_numerator > rhs_numerator,
      1: lambda: lhs_numerator <= rhs_numerator,
      3: lambda: lhs_numerator != rhs_numerator,
      5: lambda: lhs_numerator >= rhs_numerator
    }[num]()

  @staticmethod
  def count_numerator(num):
    if isinstance(num, Decimal):
      return num.get_numerator()
    return Decimal(num).get_numerator()

  @staticmethod
  def count_double(num):
    if isinstance(num, Decimal):
      return num.get_double()
    return num

  def __add__(self, num):
    return Decimal.from_numerator(
      Decimal.count_numerator(self) + Decimal.count_numerator(num))

  def __sub__(self, num):
    return Decimal.from_numerator(
      Decimal.count_numerator(self) - Decimal.count_numerator(num))

  def __mul__(self, num):
    numerator = Decimal.count_numerator(self) * Decimal.count_double(num)
    return Decimal.from_numerator(<int64_t>(numerator + (0.5 if numerator > 0 else -0.5)))

  def __div__(self, num):
    numerator = Decimal.count_numerator(self) / Decimal.count_double(num)
    return Decimal.from_numerator(<int64_t>(numerator + (0.5 if numerator > 0 else -0.5)))

  def __neg__(self):
    return Decimal.from_numerator(-self.get_numerator())

  def __abs__(self):
    numerator = self.get_numerator()
    return self if numerator > 0 else Decimal.from_numerator(-numerator)

  def less(self, rhs):
    return self.get_numerator() < rhs.get_numerator()

  def equal(self, rhs):
    return self.get_numerator() == rhs.get_numerator()

  def get_double(self):
    return self._this.get_double()

  def get_int(self):
    return self._this.get_int()

  def get_numerator(self):
    return self._this.get_numerator()


# Description of the market order.
cdef class Order:
  cdef OrderPtr _this

  @staticmethod
  cdef inline Order _from_this(OrderPtr _this):
    cdef Order result = Order.__new__(Order)
    result._this = _this
    return result

  # Returns the server time, when the order was placed, in microseconds.
  def server_time(self):
    return self._this.server_time().count()

  # Returns the unique numeric identifier of an order, which was received during a simulation.
  # It can be used to aggregate some information about the order.
  def id(self):
    return self._this.id()

  # Returns a direction of the order.
  def dir(self):
    return self._this.dir()

  # Returns an initial volume of the order (the amount of lots).
  def amount(self):
    return self._this.amount()

  # Returns an enum class value — order's status.
  # Possible statuses are: Adding, Active, Deleting, Deleted.
  # Please read more in the OrderStatus class description: <https://docs.hftbattle.com/en/api/CommonEnums.html#orderstatus>.
  def status(self):
    return self._this.status()

  # Returns current volume of the order (the amount of lots which have not been matched with other orders yet).
  def amount_rest(self):
    return self._this.amount_rest()

  # Returns a price of the order.
  def price(self):
    return Decimal._from_this_tmp(self._this.price())


# Description of the deal.
cdef class Deal:
  cdef defs.Deal* _this

  @staticmethod
  cdef inline Deal _from_this(defs.Deal* _this):
    cdef Deal result = Deal.__new__(Deal)
    result._this = _this
    return result

  # Returns a volume of the deal, i.e. lots' volume which were executed in this deal.
  def amount(self):
    return self._this.amount()

  # Returns an array of two pointers to orders, which were matched in this deal.
  def orders(self):
    return [Order._from_this(self._this.orders()[0]), Order._from_this(self._this.orders()[1])]

  # Returns a server time of the deal execution in microseconds.
  def server_time(self):
    return self._this.server_time().count()

  # Returns a price of the deal.
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # Returns a direction of the aggressor order, i.e. of the order which was placed later.
  def aggressor_side(self):
    return self._this.aggressor_side()


# Description of a price level in the order book.
cdef class Quote:
  cdef const defs.Quote* _this

  @staticmethod
  cdef inline Quote _from_this(const defs.Quote* _this):
    cdef Quote result = Quote.__new__(Quote)
    result._this = _this
    return result

  # Returns total volume of lots in orders in this quote.
  def volume(self):
    return self._this.volume()

  # Returns the latest server time, when the quote has been changed, in microseconds.
  def server_time(self):
    return self._this.server_time().count()

  # Returns a price of the quote.
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # Returns a direction of the quote.
  def dir(self):
    return self._this.dir()


# Description of your current orders.
# Only orders with Adding or Active status are taken into account.
# In deleting_amount_by_dir method orders with Deleting status are also counted.
# The snapshot is updated before every update in strategy.
# It remains unchanged during single update processing.
cdef class SecurityOrdersSnapshot:
  cdef const defs.SecurityOrdersSnapshot* _this

  @staticmethod
  cdef inline SecurityOrdersSnapshot _from_this(const defs.SecurityOrdersSnapshot* _this):
    cdef SecurityOrdersSnapshot result = SecurityOrdersSnapshot.__new__(SecurityOrdersSnapshot)
    result._this = _this
    return result

  # Takes a direction.
  # Returns a number of your active orders with given direction, i.e. orders with Active status.
  def active_orders_count(self, dir):
    return self._this.active_orders_count(dir)

  # Takes a direction.
  # Returns a vector of pointers to your orders with given direction.
  def orders_by_dir(self, dir):
    cdef:
      size_t size = self._this.orders_by_dir(dir).size()
      size_t i
    list_ans = [None] * size
    for i in xrange(size):
      list_ans[i] = Order._from_this(self._this.orders_by_dir(dir)[i])
    return list_ans

  # Takes a direction.
  # Returns a number of your current orders with given direction.
  def size_by_dir(self, dir):
    return self._this.size_by_dir(dir)

  # Takes a direction.
  # Returns total volume of your active orders with given direction, i.e. orders with Active status.
  def active_orders_volume(self, dir):
    return self._this.active_orders_volume(dir)

  # Takes a direction.
  # Returns a total volume of your orders with given direction, which had been sent to deletion, but have not been deleted (the ones with Deleting status) yet.
  def deleting_amount_by_dir(self, dir):
    return self._this.deleting_amount_by_dir(dir)

  # Takes a direction and a price.
  # Returns total volume of the orders with given price and direction.
  def volume(self, dir, Price price):
    return self._this.volume(dir, deref(price._this))

  # Takes a direction.
  # Returns a map from price to vector of pointers to your orders with given direction.
  # Note: the map is always ordered by price in ascending order.
  def orders_by_dir_as_map(self, dir):
    cdef:
      map[defs.Decimal, vector[OrderPtr]] cans = self._this.orders_by_dir_as_map(dir)
      map[defs.Decimal, vector[OrderPtr]].iterator begin = cans.begin()
      size_t size = cans.size()
      size_t vector_size, i, _
    ans = {}
    for _ in xrange(size):
      key = Decimal._from_this(new defs.Decimal(deref(begin).first))
      vector_size = deref(begin).second.size()
      value = [None] * vector_size
      for i in xrange(vector_size):
        value[i] = Order._from_this(deref(begin).second[i])
      ans[key] = value
      inc(begin)
    return ans


# This class aggregates all orders for a specific instrument.
# Note: indices are counted from 0, beginning from the best price, i.e. in descending order for BID (Buy) and in ascending order for ASK (Sell).
# **Only non-empty quotes** are taken into account.
# So if a quote has index 3, that doesn't mean that it's price differs in exactly 3 minimum steps from the best price.
cdef class OrderBook:
  cdef const defs.OrderBook* _this

  @staticmethod
  cdef inline OrderBook _from_this(const defs.OrderBook* _this):
    cdef OrderBook result = OrderBook.__new__(OrderBook)
    result._this = _this
    return result

  # Returns a distance between the best buy and best sell prices in minimum steps.
  def spread_in_min_steps(self):
    return self._this.spread_in_min_steps()

  # Returns the latest server time, when the order book has been updated, in microseconds.
  def server_time(self):
    return self._this.server_time().count()

  # Returns a half-sum of the best prices in both directions.
  def middle_price(self):
    return Decimal._from_this_tmp(self._this.middle_price())

  # Takes a direction and a price.
  # Returns a quote with given direction and price.
  def quote_by_price(self, dir, Price price):
    return Quote._from_this(&self._this.quote_by_price(dir, deref(price._this)))

  # Takes a direction.
  # Returns a volume of the quote with the best price by given direction.
  def best_volume(self, dir):
    return self._this.best_volume(dir)

  # Takes a direction and an index.
  # Returns a price of the quote with given direction and index.
  # Note: if the quote doesn't exist, corresponding default_quote_price is returned.
  def price_by_index(self, dir, size_t index):
    return Decimal._from_this_tmp(self._this.price_by_index(dir, index))

  # Takes a direction and an index.
  # Returns a quote with given direction and index.
  def quote_by_index(self, dir, size_t index):
    return Quote._from_this(&self._this.quote_by_index(dir, index))

  # Takes a direction.
  # Returns the best price in order book with given direction.
  # Note: if the quote doesn't exist, corresponding default_quote_price is returned.
  def best_price(self, dir):
    return Decimal._from_this_tmp(self._this.best_price(dir))

  # Returns a minimum price step in the order book (the least possible difference between prices).
  def min_step(self):
    return Decimal._from_this_tmp(self._this.min_step())

  # Returns a fee per one executed lot.
  def fee_per_lot(self):
    return Decimal._from_this_tmp(self._this.fee_per_lot())

  # Takes a direction and a price.
  # Returns a volume of the quote with given direction and price.
  def volume_by_price(self, dir, Price price):
    return self._this.volume_by_price(dir, deref(price._this))

  # Returns a maximum number of visible quote levels in the order book (it's the same for both directions).
  def depth(self):
    return self._this.depth()

  # Takes a direction and an index.
  # Returns a volume of the quote with given direction and index.
  def volume_by_index(self, dir, size_t index):
    return self._this.volume_by_index(dir, index)

  # Returns a number of the order book updates since the beginning of the trading session.
  # Note: research of the change of this value can be used to understand the market activity.
  def book_updates_count(self):
    return self._this.book_updates_count()

  # Returns a reference to a SecurityOrdersSnapshot object containing all your current quotes.
  # Please read more about SecurityOrdersSnapshot here: <https://docs.hftbattle.com/en/api/SecurityOrdersSnapshot.html>.
  def orders(self):
    return SecurityOrdersSnapshot._from_this(&self._this.orders())

  # Takes a direction and a price.
  # Returns an index of the quote with given direction and price.
  # If the quote with given price doesn't exist, maximum size_t value (i.e. `std::numeric_limits<size_t>::max()`) is returned.
  def index_by_price(self, dir, Price price):
    return self._this.index_by_price(dir, deref(price._this))

  # Takes a direction.
  # Returns a number of non-empty quotes with given direction.
  def quotes_count(self, dir):
    return self._this.quotes_count(dir)

  # Takes a direction.
  # Returns all quotes with given direction as a QuotesHolder object.
  # QuotesHolder is a container, which allows you to iterate you through all quotes.
  # Please read more about QuotesHolder here: <https://docs.hftbattle.com/en/api/QuotesHolder.html>.
  def all_quotes(self, dir):
    cdef:
      size_t i = 0
      size_t quotes_count = self.quotes_count(dir)
    for i in xrange(quotes_count):
      yield self.quote_by_index(dir, i)


cdef class tm:
  cdef defs.tm* _this

  @staticmethod
  cdef inline tm _from_this(defs.tm* _this):
    cdef tm result = tm.__new__(tm)
    result._this = _this
    return result

  @staticmethod
  cdef inline tm _from_this_tmp(defs.tm&& _this):
    return tm._from_this(new defs.tm(_this))

  def __dealloc__(self):
    del self._this

  def tm_min(self):
    return self._this.tm_min

  def tm_wday(self):
    return self._this.tm_wday

  def tm_yday(self):
    return self._this.tm_yday

  def tm_isdst(self):
    return self._this.tm_isdst

  def tm_year(self):
    return self._this.tm_year

  def tm_mday(self):
    return self._this.tm_mday

  def tm_mon(self):
    return self._this.tm_mon

  def tm_sec(self):
    return self._this.tm_sec

  def tm_hour(self):
    return self._this.tm_hour


# A wrapper class for your strategy, which interacts with a trading simulator.
cdef class ParticipantStrategy:
  cdef defs.ParticipantStrategy* _this

  @staticmethod
  cdef inline ParticipantStrategy _from_this(defs.ParticipantStrategy* _this):
    cdef ParticipantStrategy result = ParticipantStrategy.__new__(ParticipantStrategy)
    result._this = _this
    return result

  # Your current trading order book.
  # Note: you cannot get trading order book before first `trading_book_update` call.
  def trading_book(self):
    return OrderBook._from_this(&self._this.trading_book())

  # Returns your current position.
  # Only executed orders are taken into account.
  def executed_amount(self):
    return self._this.executed_amount()

  # Returns current server time in microseconds.
  def server_time(self):
    return self._this.server_time().count()

  # Takes a direction and a price.
  # Returns total number of lots in your active orders with given direction and price.
  def volume_by_price(self, dir, Price price):
    return self._this.volume_by_price(dir, deref(price._this))

  # Takes a pointer to your order.
  # Returns total number of lots queued before your order in the quote with given price.
  def amount_before_order(self, Order order):
    return self._this.amount_before_order(order._this)

  # Takes a line_name string (name of the chart), double or Decimal value, y_axis_type — side which Y axis will be drawn on and chart_number — the number of your chart.
  # Adds point to the chart at the current moment with given value.
  # Each two adjacent points are connected.
  # The resulting polygonal chain is your chart.
  # chart_number allows you to create several charts and line_name allows you to draw several lines on the single image.
  def add_chart_point(self, str line_name, double value, int y_axis_type = 0, uint8_t chart_number = 1):
    return self._this.add_chart_point(line_name, deref(Decimal(value)._this), <defs.ChartYAxisType>(y_axis_type), chart_number)

  def add_chart_point(self, str line_name, Decimal value, int y_axis_type = 0, uint8_t chart_number = 1):
    return self._this.add_chart_point(line_name, deref(value._this), <defs.ChartYAxisType>(y_axis_type), chart_number)

  # Takes a pointer to your order.
  # Sends a request to remove this order from the auction.
  # Note: the deletion is not executed instantly.
  # You can read more about restrictions here: <https://docs.hftbattle.com/en/simulator/restrictions.html>.
  def delete_order(self, Order order):
    return self._this.delete_order(order._this)

  # Returns current result of your strategy (your profit).
  # Executed and placed orders are taken into account.
  # When calculating the result, we assume that all active orders are executed at the opposite best price.
  def current_result(self):
    return Decimal._from_this_tmp(self._this.current_result())

  # Takes a direction, a price and an amount of the new order.
  # Places the new limit order.
  # Returns bool value — was the placement successful or not.
  # Note: the order can be rejected if certain restrictions are not met.
  # Please read about it here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  def add_limit_order(self, dir, Price price, Amount amount):
    return self._this.add_limit_order(dir, deref(price._this), amount)

  # Takes non-positive number — desired minimum result.
  # If your strategy reaches this result, simulator automatically liquidates your position and stops your strategy.
  # Note: your position liquidation is not executed instantly, so your result may significantly differ from *stop_loss*.
  # You can read more here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  def set_stop_loss_result(self, Decimal stop_loss_result):
    return self._this.set_stop_loss_result(deref(stop_loss_result._this))

  # Takes a direction.
  # Sends a request to remove all your orders with given direction.
  def delete_all_orders_at_dir(self, dir):
    return self._this.delete_all_orders_at_dir(dir)

  # Returns a server time of struct tm type with seconds precision.
  # Note: server time in this format is useful to determine the time of the day.
  def server_time_tm(self):
    return tm._from_this_tmp(self._this.server_time_tm())

  # Takes a direction, a price and an amount of the new order.
  # Places the new IOC (Immediate-Or-Cancel) order.
  # Returns bool value — was the placement successful or not.
  # Note: the order can be rejected if certain restrictions are not met.
  # Please read about it here: <https://docs.hftbattle.com/en/HFAQ.html#simulator>
  def add_ioc_order(self, dir, Price price, Amount amount):
    return self._this.add_ioc_order(dir, deref(price._this), amount)

  # Takes desired value of maximum position — non-negative number which must not be greater than 100.
  # Sets this position as maximum and doesn't allow your strategy to exceed it.
  def set_max_total_amount(self, Amount max_total_amount):
    return self._this.set_max_total_amount(max_total_amount)

  # Takes a direction and a price.
  # Sends a request to remove all your orders with given direction and price.
  def delete_all_orders_at_price(self, dir, Price price):
    return self._this.delete_all_orders_at_price(dir, deref(price._this))

  # Takes a pointer to the order.
  # Returns bool value — whether this order is yours or not.
  def is_our(self, Order order):
    return self._this.is_our(order._this)

  # Takes a reference to the deal.
  # Returns bool value — whether your order was matched in the deal.
  def is_our(self, Deal deal):
    return self._this.is_our(deref(deal._this))

  # Takes any string you want to be the name of current moment.
  # Saves link to Viewer in the web system after data is written there.
  def fix_moment_in_viewer(self, str name):
    return self._this.fix_moment_in_viewer(name)


# Report on a deal with your order.
cdef class ExecutionReport:
  cdef const defs.ExecutionReport* _this

  @staticmethod
  cdef inline ExecutionReport _from_this(const defs.ExecutionReport* _this):
    cdef ExecutionReport result = ExecutionReport.__new__(ExecutionReport)
    result._this = _this
    return result

  # Returns a volume of the deal.
  def amount(self):
    return self._this.amount()

  # Returns a pointer to your order, that was matched in the deal.
  def order(self):
    return Order._from_this(self._this.order())

  # Returns a price of the deal.
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # Returns a direction of your executed order.
  def dir(self):
    return self._this.dir()


cdef public bool ccheck_for_syntax_errors():
  try:
    import strategies.python_strategy.python_strategy
  except:
    traceback.print_exc()
    return True
  return False


cdef public bool cinit_trading_book_update():
  try:
    from strategies.python_strategy.python_strategy import trading_book_update
  except:
    return False
  return True


cdef public bool cinit_trading_deals_update():
  try:
    from strategies.python_strategy.python_strategy import trading_deals_update
  except:
    return False
  return True


cdef public bool cinit_execution_report_update():
  try:
    from strategies.python_strategy.python_strategy import execution_report_update
  except:
    return False
  return True


cdef public void cinit(defs.ParticipantStrategy* ptr, string config) except *:
  try:
    from strategies.python_strategy.python_strategy import init
  except:
    return

  import json

  cdef ParticipantStrategy strat = ParticipantStrategy._from_this(ptr)
  cdef dict config_dict = json.loads(config)
  try:
    init(strat, config_dict)
  except:
    traceback.print_exc()
    raise


cdef public void ctrading_book_update(defs.ParticipantStrategy* ptr, const defs.OrderBook& order_book) except *:
  cdef:
    ParticipantStrategy strat = ParticipantStrategy._from_this(ptr)
    OrderBook trading_book = strat.trading_book()
  try:
    trading_book_update(strat, trading_book)
  except:
    traceback.print_exc()
    raise


cdef public void ctrading_deals_update(defs.ParticipantStrategy* ptr, vector[defs.Deal]&& deals) except *:
  cdef:
    ParticipantStrategy strat = ParticipantStrategy._from_this(ptr)
    size_t deals_size = deals.size()
    size_t i = 0
  py_deals = [None] * deals_size
  for i in xrange(deals_size):
    py_deals[i] = Deal._from_this(&deals[i])
  try:
    trading_deals_update(strat, py_deals)
  except:
    traceback.print_exc()
    raise


cdef public void cexecution_report_update(defs.ParticipantStrategy* ptr, const defs.ExecutionReport& snapshot) except *:
  cdef:
    ParticipantStrategy strat = ParticipantStrategy._from_this(ptr)
    ExecutionReport execution_report = ExecutionReport._from_this(&snapshot)
  try:
    execution_report_update(strat, execution_report)
  except:
    traceback.print_exc()
    raise