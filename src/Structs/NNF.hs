---------------------------------------------------------------
-- |
-- Module      :  Structs.NNF
-- Copyright   :  (c) David Lilue, 2018
-- License     :  see LICENSE
--
-- Maintainer  :  dvdalilue@gmail.com
-- Stability   :  experimental
-- Portability :  portable
--
-- Negative normal form (NNF) implementation using Prop data
-- type.
--
---------------------------------------------------------------

module Structs.NNF(nnf,nnf_prps) where

import Structs.Essentials.Prop
import Structs.Essentials.Helper

-- | A propposition manipulator that takes a proposition and, going recursively
-- to the leaf statements, change the implication data constructors following
-- the next theorem.
-- 
-- > (p :>: q) == (Negation p) :|: q.
--
-- Returning the equivalent proposition without implications.

impl_free :: Prop a -> Prop a
impl_free x@(Statement _) = x
impl_free   (Negation  p) = Negation (impl_free p)
impl_free       (p :&: q) = (impl_free p) :&: (impl_free q)
impl_free       (p :|: q) = (impl_free p) :|: (impl_free q)
impl_free       (p :=: q) = impl_free $ (p :>: q) :&: (q :>: p)
impl_free       (p :>: q) = (Negation (impl_free p)) :|: (impl_free q)

-- | A variation of 'impl_free' which in this case takes a proposition, a
-- polymorphic list (accumulator), goes recursively to the leaf statements and
-- follows the same theorem.
-- 
-- Returning a 2-tuple with the first element of type [a] and the second
-- one being the proposition without implications.

impl_f_prps :: Eq a => Prop a -> [a] -> ([a], Prop a)
impl_f_prps x@(Statement s) acc = (s `politeCons` acc, x)
impl_f_prps       (p :=: q) acc = impl_f_prps ((p :>: q) :&: (q :>: p)) acc
impl_f_prps    (Negation p) acc = let (acp, ifp) = impl_f_prps p acc
                                    in (acp, Negation ifp)
impl_f_prps       (p :&: q) acc = let (acp, ifp) = impl_f_prps p acc
                                      (acq, ifq) = impl_f_prps q acp
                                        in (acq, ifp :&: ifq)
impl_f_prps       (p :|: q) acc = let (acp, ifp) = impl_f_prps p acc
                                      (acq, ifq) = impl_f_prps q acp
                                        in (acq, ifp :|: ifq)
impl_f_prps       (p :>: q) acc = let (acp, ifp) = impl_f_prps p acc
                                      (acq, ifq) = impl_f_prps q acp
                                        in (acq, (Negation ifp) :|: ifq)

-- | Takes a proposition of type 'Prop' and it's transformed into its negative
-- normal form (NNF). First of all, the implications are removed of the
-- proposition.

nnf :: Prop a -> Prop a
nnf = nnf_aux . impl_free

-- | A variation of 'nnf' which returns a 2-tuple with the first element of type
-- [a] and the second one being the proposition in NNF.

nnf_prps :: Eq a => Prop a -> ([a], Prop a)
nnf_prps p = let (as, ifp) = impl_f_prps p [] in (as, nnf_aux ifp)

-- | Auxiliary function that assumed the imput proposition is without
-- implication and change it into its NNF.

nnf_aux :: Prop a -> Prop a
nnf_aux (Negation (Negation p)) = nnf_aux p
nnf_aux (Negation (p :&: q)) = (nnf_aux (Negation p)) :|: (nnf_aux (Negation q))
nnf_aux (Negation (p :|: q)) = (nnf_aux (Negation p)) :&: (nnf_aux (Negation q))
nnf_aux (p :&: q) = (nnf_aux p) :&: (nnf_aux q)
nnf_aux (p :|: q) = (nnf_aux p) :|: (nnf_aux q)
nnf_aux p = p
