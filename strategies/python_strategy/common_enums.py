# docs.common_enums.chart_y_axis_type.class
class ChartYAxisType:
    Left = 0
    Right = 1


# docs.common_enums.order_status.class
class OrderStatus:
    Adding = 0
    Active = 1
    Deleting = 2
    Deleted = 3


# docs.common_enums.dir.class
BID = BUY = 0
ASK = SELL = 1


# docs.common_enums.opposite_dir.class
def opposite_dir(dir):
    return 1 - dir


# docs.common_enums.dir_sign.class
def dir_sign(dir):
    return dir * (-2) + 1
