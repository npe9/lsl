ChoiceBag(E): trait
  % A bag with a weakly-specified choose operator
  includes Bag
  introduces
    choose:  Bag[E] -> E
    rest:    Bag[E] -> Bag[E]
    isEmpty: Bag[E] -> Bool
  asserts \forall e, e1: E, b: Bag[E]
    b \neq {} => choose(b) \in b;
    b \neq {} => rest(b) = delete(choose(b), b);
    isEmpty(b) <=> b = {}
  implies
    Container(E, Bag[E], choose for head, 
              rest for tail, {} for empty)
    sort Bag[E] partitioned by choose, rest, isEmpty
    \forall e: E, b: Bag[E]
      b \neq {} => b = insert(choose(b), rest(b))
