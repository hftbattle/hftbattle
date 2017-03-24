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

ctypedef Order* OrderPtr

ctypedef microseconds Microseconds

cdef extern from "base/common_enums.h" namespace "hftbattle::Dir":
  cdef enum Dir "hftbattle::Dir":
    BUY = 0
    BID = 0
    SELL = 1
    ASK = 1
    UNKNOWN = 3


cdef extern from "base/common_enums.h" namespace "hftbattle::OrderStatus":
  cdef enum OrderStatus "hftbattle::OrderStatus":
    Adding
    Active
    Deleting
    Deleted


cdef extern from "base/common_enums.h" namespace "hftbattle::ChartYAxisType":
  cdef enum ChartYAxisType "hftbattle::ChartYAxisType":
    Left = 0
    Right = 1


cdef extern from "<chrono>" namespace "std::chrono":
  cdef cppclass microseconds:
    microseconds(microseconds&&) except +
    uint64_t count() const


cdef extern from "base/decimal.h" namespace "hftbattle":
  cdef cppclass Decimal:
    Decimal(double) except +
    Decimal(Decimal&&) except +

    double get_double() const
    int32_t get_int() const
    int64_t get_numerator() const

    @staticmethod
    Decimal from_numerator(int64_t)


cdef extern from "order.h" namespace "hftbattle":
  cdef cppclass Order:
    Order(Order&&) except +

    Microseconds server_time () const
    Id id () const
    Dir dir () const
    Amount amount () const
    OrderStatus status () const
    Amount amount_rest () const
    Price price () const


cdef extern from "<array>" namespace "std" nogil:
  cdef cppclass array_order_2 "std::array<Order*, 2>":
    array_order_2() except+
    OrderPtr& operator[](size_t)
    const OrderPtr& operator[](long) const


cdef extern from "deal.h" namespace "hftbattle":
  cdef cppclass Deal:
    Amount amount () const
    const array_order_2 orders () const
    Microseconds server_time () const
    Price price () const
    Dir aggressor_side () const


cdef extern from "quote.h" namespace "hftbattle":
  cdef cppclass Quote:
    Amount volume () const
    Microseconds server_time () const
    Price price () const
    Dir dir () const


cdef extern from "security_orders_snapshot.h" namespace "hftbattle":
  cdef cppclass SecurityOrdersSnapshot:
    size_t active_orders_count (Dir dir) const
    const vector[OrderPtr]& orders_by_dir (Dir dir) const
    size_t size_by_dir (Dir dir) const
    Amount active_orders_volume (Dir dir) const
    Amount deleting_amount_by_dir (Dir dir) const
    Amount volume (Dir dir, Price price) const
    map[Price, vector[OrderPtr]] orders_by_dir_as_map (Dir dir) const


cdef extern from "order_book.h" namespace "hftbattle":
  cdef cppclass OrderBook:
    size_t spread_in_min_steps () const
    Microseconds server_time () const
    Price middle_price () const
    const Quote& quote_by_price (Dir dir, Price price) const
    Amount best_volume (Dir dir) const
    Price price_by_index (Dir dir, size_t index) const
    const Quote& quote_by_index (Dir dir, size_t index) const
    Price best_price (Dir dir) const
    Price min_step () const
    Decimal fee_per_lot () const
    Amount volume_by_price (Dir dir, Price price) const
    size_t depth () const
    Amount volume_by_index (Dir dir, size_t index) const
    size_t book_updates_count () const
    const SecurityOrdersSnapshot& orders () const
    size_t index_by_price (Dir dir, Price price) const
    size_t quotes_count (Dir dir) const


cdef extern from "<time.h>":
  cdef cppclass tm:
    tm() except +
    tm(tm&&) except +
    int tm_min
    int tm_wday
    int tm_yday
    int tm_isdst
    int tm_year
    int tm_mday
    int tm_mon
    int tm_sec
    int tm_hour


cdef extern from "participant_strategy.h" namespace "hftbattle":
  cdef cppclass ParticipantStrategy:
    const OrderBook& trading_book() const
    Amount executed_amount () const
    Microseconds server_time () const
    Amount volume_by_price (Dir dir, Price price) const
    Amount amount_before_order (const OrderPtr order) const
    void add_chart_point (const string& line_name, Decimal value, ChartYAxisType y_axis_type, uint8_t chart_number) const
    void delete_order (OrderPtr order) const
    Decimal current_result () const
    bool add_limit_order (Dir dir, Price price, Amount amount) const
    void set_stop_loss_result (const Decimal stop_loss_result)
    void delete_all_orders_at_dir (Dir dir) const
    tm server_time_tm () const
    bool add_ioc_order (Dir dir, Price price, Amount amount) const
    void set_max_total_amount (const Amount max_total_amount)
    void delete_all_orders_at_price (Dir dir, Price price) const
    bool is_our(const Order* order) const
    bool is_our(const Deal& deal) const
    void fix_moment_in_viewer(const string& name)


cdef extern from "execution_report.h" namespace "hftbattle":
  cdef cppclass ExecutionReport:
    Amount amount () const
    OrderPtr order () const
    Price price () const
    Dir dir () const
