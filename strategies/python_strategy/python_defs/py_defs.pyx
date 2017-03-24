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


# docs.decimal.class
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


# docs.order.class
cdef class Order:
  cdef OrderPtr _this

  @staticmethod
  cdef inline Order _from_this(OrderPtr _this):
    cdef Order result = Order.__new__(Order)
    result._this = _this
    return result

  # docs.order.server_time
  def server_time(self):
    return self._this.server_time().count()

  # docs.order.id
  def id(self):
    return self._this.id()

  # docs.order.dir
  def dir(self):
    return self._this.dir()

  # docs.order.amount
  def amount(self):
    return self._this.amount()

  # docs.order.status
  def status(self):
    return self._this.status()

  # docs.order.amount_rest
  def amount_rest(self):
    return self._this.amount_rest()

  # docs.order.price
  def price(self):
    return Decimal._from_this_tmp(self._this.price())


# docs.deal.class
cdef class Deal:
  cdef defs.Deal* _this

  @staticmethod
  cdef inline Deal _from_this(defs.Deal* _this):
    cdef Deal result = Deal.__new__(Deal)
    result._this = _this
    return result

  # docs.deal.amount
  def amount(self):
    return self._this.amount()

  # docs.deal.orders
  def orders(self):
    return [Order._from_this(self._this.orders()[0]), Order._from_this(self._this.orders()[1])]

  # docs.deal.server_time
  def server_time(self):
    return self._this.server_time().count()

  # docs.deal.price
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # docs.deal.aggressor_side
  def aggressor_side(self):
    return self._this.aggressor_side()


# docs.quote.class
cdef class Quote:
  cdef const defs.Quote* _this

  @staticmethod
  cdef inline Quote _from_this(const defs.Quote* _this):
    cdef Quote result = Quote.__new__(Quote)
    result._this = _this
    return result

  # docs.quote.volume
  def volume(self):
    return self._this.volume()

  # docs.quote.server_time
  def server_time(self):
    return self._this.server_time().count()

  # docs.quote.price
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # docs.quote.dir
  def dir(self):
    return self._this.dir()


# docs.security_orders_snapshot.class
cdef class SecurityOrdersSnapshot:
  cdef const defs.SecurityOrdersSnapshot* _this

  @staticmethod
  cdef inline SecurityOrdersSnapshot _from_this(const defs.SecurityOrdersSnapshot* _this):
    cdef SecurityOrdersSnapshot result = SecurityOrdersSnapshot.__new__(SecurityOrdersSnapshot)
    result._this = _this
    return result

  # docs.security_orders_snapshot.active_orders_count
  def active_orders_count(self, dir):
    return self._this.active_orders_count(dir)

  # docs.security_orders_snapshot.orders_by_dir
  def orders_by_dir(self, dir):
    cdef:
      size_t size = self._this.orders_by_dir(dir).size()
      size_t i
    list_ans = [None] * size
    for i in xrange(size):
      list_ans[i] = Order._from_this(self._this.orders_by_dir(dir)[i])
    return list_ans

  # docs.security_orders_snapshot.size_by_dir
  def size_by_dir(self, dir):
    return self._this.size_by_dir(dir)

  # docs.security_orders_snapshot.active_orders_volume
  def active_orders_volume(self, dir):
    return self._this.active_orders_volume(dir)

  # docs.security_orders_snapshot.deleting_amount_by_dir
  def deleting_amount_by_dir(self, dir):
    return self._this.deleting_amount_by_dir(dir)

  # docs.security_orders_snapshot.volume
  def volume(self, dir, Price price):
    return self._this.volume(dir, deref(price._this))

  # docs.security_orders_snapshot.orders_by_dir_as_map
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


# docs.order_book.class
cdef class OrderBook:
  cdef const defs.OrderBook* _this

  @staticmethod
  cdef inline OrderBook _from_this(const defs.OrderBook* _this):
    cdef OrderBook result = OrderBook.__new__(OrderBook)
    result._this = _this
    return result

  # docs.order_book.spread_in_min_steps
  def spread_in_min_steps(self):
    return self._this.spread_in_min_steps()

  # docs.order_book.server_time
  def server_time(self):
    return self._this.server_time().count()

  # docs.order_book.middle_price
  def middle_price(self):
    return Decimal._from_this_tmp(self._this.middle_price())

  # docs.order_book.quote_by_price
  def quote_by_price(self, dir, Price price):
    return Quote._from_this(&self._this.quote_by_price(dir, deref(price._this)))

  # docs.order_book.best_volume
  def best_volume(self, dir):
    return self._this.best_volume(dir)

  # docs.order_book.price_by_index
  def price_by_index(self, dir, size_t index):
    return Decimal._from_this_tmp(self._this.price_by_index(dir, index))

  # docs.order_book.quote_by_index
  def quote_by_index(self, dir, size_t index):
    return Quote._from_this(&self._this.quote_by_index(dir, index))

  # docs.order_book.best_price
  def best_price(self, dir):
    return Decimal._from_this_tmp(self._this.best_price(dir))

  # docs.order_book.min_step
  def min_step(self):
    return Decimal._from_this_tmp(self._this.min_step())

  # docs.order_book.fee_per_lot
  def fee_per_lot(self):
    return Decimal._from_this_tmp(self._this.fee_per_lot())

  # docs.order_book.volume_by_price
  def volume_by_price(self, dir, Price price):
    return self._this.volume_by_price(dir, deref(price._this))

  # docs.order_book.depth
  def depth(self):
    return self._this.depth()

  # docs.order_book.volume_by_index
  def volume_by_index(self, dir, size_t index):
    return self._this.volume_by_index(dir, index)

  # docs.order_book.book_updates_count
  def book_updates_count(self):
    return self._this.book_updates_count()

  # docs.order_book.orders
  def orders(self):
    return SecurityOrdersSnapshot._from_this(&self._this.orders())

  # docs.order_book.index_by_price
  def index_by_price(self, dir, Price price):
    return self._this.index_by_price(dir, deref(price._this))

  # docs.order_book.quotes_count
  def quotes_count(self, dir):
    return self._this.quotes_count(dir)

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


# docs.participant_strategy.class
cdef class ParticipantStrategy:
  cdef defs.ParticipantStrategy* _this

  @staticmethod
  cdef inline ParticipantStrategy _from_this(defs.ParticipantStrategy* _this):
    cdef ParticipantStrategy result = ParticipantStrategy.__new__(ParticipantStrategy)
    result._this = _this
    return result

  # docs.participant_strategy.trading_book
  def trading_book(self):
    return OrderBook._from_this(&self._this.trading_book())

  # docs.participant_strategy.executed_amount
  def executed_amount(self):
    return self._this.executed_amount()

  # docs.participant_strategy.server_time
  def server_time(self):
    return self._this.server_time().count()

  # docs.participant_strategy.volume_by_price
  def volume_by_price(self, dir, Price price):
    return self._this.volume_by_price(dir, deref(price._this))

  # docs.participant_strategy.amount_before_order
  def amount_before_order(self, Order order):
    return self._this.amount_before_order(order._this)

  # docs.participant_strategy.add_chart_point
  def add_chart_point(self, str line_name, double value, int y_axis_type = 0, uint8_t chart_number = 1):
    return self._this.add_chart_point(line_name, deref(Decimal(value)._this), <defs.ChartYAxisType>(y_axis_type), chart_number)

  def add_chart_point(self, str line_name, Decimal value, int y_axis_type = 0, uint8_t chart_number = 1):
    return self._this.add_chart_point(line_name, deref(value._this), <defs.ChartYAxisType>(y_axis_type), chart_number)

  # docs.participant_strategy.delete_order
  def delete_order(self, Order order):
    return self._this.delete_order(order._this)

  # docs.participant_strategy.current_result
  def current_result(self):
    return Decimal._from_this_tmp(self._this.current_result())

  # docs.participant_strategy.add_limit_order
  def add_limit_order(self, dir, Price price, Amount amount):
    return self._this.add_limit_order(dir, deref(price._this), amount)

  # docs.participant_strategy.set_stop_loss_result
  def set_stop_loss_result(self, Decimal stop_loss_result):
    return self._this.set_stop_loss_result(deref(stop_loss_result._this))

  # docs.participant_strategy.delete_all_orders_at_dir
  def delete_all_orders_at_dir(self, dir):
    return self._this.delete_all_orders_at_dir(dir)

  # docs.participant_strategy.server_time_tm
  def server_time_tm(self):
    return tm._from_this_tmp(self._this.server_time_tm())

  # docs.participant_strategy.add_ioc_order
  def add_ioc_order(self, dir, Price price, Amount amount):
    return self._this.add_ioc_order(dir, deref(price._this), amount)

  # docs.participant_strategy.set_max_total_amount
  def set_max_total_amount(self, Amount max_total_amount):
    return self._this.set_max_total_amount(max_total_amount)

  # docs.participant_strategy.delete_all_orders_at_price
  def delete_all_orders_at_price(self, dir, Price price):
    return self._this.delete_all_orders_at_price(dir, deref(price._this))

  # docs.participant_strategy.is_our.order
  def is_our(self, Order order):
    return self._this.is_our(order._this)

  # docs.participant_strategy.is_our.deal
  def is_our(self, Deal deal):
    return self._this.is_our(deref(deal._this))

  def fix_moment_in_viewer(self, str name):
    return self._this.fix_moment_in_viewer(name)


# docs.execution_report.class
cdef class ExecutionReport:
  cdef const defs.ExecutionReport* _this

  @staticmethod
  cdef inline ExecutionReport _from_this(const defs.ExecutionReport* _this):
    cdef ExecutionReport result = ExecutionReport.__new__(ExecutionReport)
    result._this = _this
    return result

  # docs.execution_report.amount
  def amount(self):
    return self._this.amount()

  # docs.execution_report.order
  def order(self):
    return Order._from_this(self._this.order())

  # docs.execution_report.price
  def price(self):
    return Decimal._from_this_tmp(self._this.price())

  # docs.execution_report.dir
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
