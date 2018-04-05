---------------------------------------------------------------
-- |
-- Module      :  Structs.CNF
-- Copyright   :  (c) David Lilue, 2018
-- License     :  see LICENSE
--
-- Maintainer  :  dvdalilue@gmail.com
-- Stability   :  experimental
-- Portability :  portable
--
-- Conjunctive normal form (CNF) implementation using Prop data
-- type and NNF implemntation.
--
---------------------------------------------------------------

module Structs.CNF(cnf) where

import Structs.Essentials.Prop
import Structs.NNF

-- | Applies distribution property (or over and)
distr :: Prop a -- ^ Left disjunction operand 
      -> Prop a -- ^ Right disjunction operand
      -> Prop a -- ^ Recursive distributed proposition
distr (a :&: b) q = (distr a q) :&: (distr b q)
distr p (a :&: b) = (distr p a) :&: (distr p b)
distr p q = p :|: q

-- | Transform a proposition into conjunctive normal form (CNF).
cnf :: Prop a -- ^ Input 'Prop' in any form
    -> Prop a -- ^ Output 'Prop' in conjunctive normal form
cnf = cnf_aux . nnf
-- The proposition has to be in NNF prior the transformation
    where
        cnf_aux (p :&: q) = (cnf_aux p) :&: (cnf_aux q)
        cnf_aux (p :|: q) = distr (cnf_aux p) (cnf_aux q)
        cnf_aux p = p