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

module Structs.CNF(cnf,flatCNF,flatCNFStms) where

import Structs.NNF
import Structs.Essentials.Prop

import Control.Applicative(pure)

-- | Applies recursively the distribution property (or over and) in a given
-- proposition. It's assumed that the first argument is the left operand of a
-- disjunction and the second argument is the right operand.

distr :: Prop a -> Prop a -> Prop a
distr (a :&: b) q = (distr a q) :&: (distr b q)
distr p (a :&: b) = (distr p a) :&: (distr p b)
distr p q = p :|: q

-- | Auxiliary function which change a given proposition into its conjunctive
-- normal form (CNF). The function assumes that the proposition is in NNF. 

cnf_aux :: Prop a -> Prop a
cnf_aux (p :&: q) = (cnf_aux p) :&: (cnf_aux q)
cnf_aux (p :|: q) = distr (cnf_aux p) (cnf_aux q)
cnf_aux p = p

-- | Takes a proposition of type 'Prop' and it's transformed into its
-- CNF. Prior to that, the 'nnf' function is used in order to have the
-- proposition in NNF. This step is necessary.

cnf :: Prop a -> Prop a
cnf = cnf_aux . nnf

-- | Auxiliary function that takes a proposition in CNF return a list of
-- propositions. Using the first argument as accumulator and the second one as a
-- stack, assuming that the initial proposition is the first element in it.

flat_aux :: [Prop a] -> [Prop a] -> [Prop a]
flat_aux acc                [] = acc
flat_aux acc ((p :&: q):stack) = flat_aux acc (p:q:stack)
flat_aux acc         (p:stack) = flat_aux (p:acc) stack

-- | Flat a proposition into a list of the components of its CNF.
flatCNF :: Prop a -> [Prop a]
flatCNF = flat_aux [] . pure . cnf

-- | Similar to the previous function but returns a 2-tuple with the first
-- element as a list of the statements and the second one being a list of the
-- components of its CNF.

flatCNFStms :: Eq a => Prop a -> ([a], [Prop a])
flatCNFStms p = let (as, ifp) = nnf_stms p in (as, inner_flat ifp)
    where inner_flat = flat_aux [] . pure . cnf_aux
