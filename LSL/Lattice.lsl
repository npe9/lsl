Lattice(T): trait
  assumes PartialOrder
  includes Semilattice
  introduces \top: -> T
  asserts \forall x: T
    x <= \top
  implies 
    Lattice(\lub for \glb, \glb for \lub, \top for \bot, \bot for \top,
            <= for >=, >= for <=, < for >, > for <)
