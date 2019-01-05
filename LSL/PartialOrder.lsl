PartialOrder(T): trait
  includes IsPO, DerivedOrders
  implies
    PartialOrder(> for <, < for >,
                 >= for <=, <= for >=),
    StrictPartialOrder(<, T)
