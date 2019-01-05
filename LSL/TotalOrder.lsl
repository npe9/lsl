TotalOrder(T): trait
  includes IsTO, DerivedOrders
  implies
    PartialOrder,
    StrictTotalOrder(<, T),
    TotalOrder(>= for <=, <= for >=,
               > for <, < for >)
    sort T partitioned by <
