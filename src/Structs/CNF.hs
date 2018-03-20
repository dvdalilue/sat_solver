module Structs.CNF(cnf) where

import Structs.Essentials.Prop
import Structs.NNF

distr :: Prop a -> Prop a -> Prop a
distr (And a b) q = And (distr a q) (distr b q)
distr p (And a b) = And (distr p a) (distr p b)
distr p q = Or p q

cnf :: Prop a -> Prop a
cnf = cnf_aux . nnf
    where
        cnf_aux (And p q) = And (cnf_aux p) (cnf_aux q)
        cnf_aux (Or p q) = distr (cnf_aux p) (cnf_aux q)
        cnf_aux p = p