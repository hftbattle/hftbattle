# -*- coding: utf-8 -*-


# Enum class describing a side which to draw Y-axis on (left or right).
# - Left — on the left side.
# - Right — on the right side.
class ChartYAxisType:
    Left = 0
    Right = 1


# The enum class describing possible order statuses:
# - Adding — the order has been created but has not been placed yet.
#   This happens because of the delay between order addition request and order appearance on the market.
#   This delay is called round-trip.
# - Active — the order is placed.
# - Deleting — there had been a request to delete the order, but it is has not been deleted yet.
#   This happens because of the delay between order deletion request and order deletion on the market.
# - Deleted — the order was deleted.
class OrderStatus:
    Adding = 0
    Active = 1
    Deleting = 2
    Deleted = 3


# The enum describing a direction of deals, orders and quotes.
# - BID, BUY, 0 means purchase.
# - ASK, SELL, 1 means selling.
BID = BUY = 0
ASK = SELL = 1


# Takes a direction.
# Returns the opposite direction.
def opposite_dir(dir):
    return 1 - dir


# Takes a direction.
# Returns a direction sign (1 for BID and -1 for ASK).
def dir_sign(dir):
    return dir * (-2) + 1