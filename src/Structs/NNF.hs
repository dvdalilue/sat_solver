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

module Structs.NNF(nnf,nnf_stms) where

import Structs.Essentials.Prop
import Structs.Essentials.Helper

-- | A propposition manipulator that takes a proposition and, going recursively
-- to the leaf statements, change the implication data constructors following
-- the next theorem.
-- 
-- > (p :>: q) == (Neg p) :|: q.
--
-- Returning the equivalent proposition without implications.

impl_free :: Prop a -> Prop a
impl_free x@(Stmnt _) = x
impl_free    (Neg  p) = Neg (impl_free p)
impl_free   (p :&: q) = (impl_free p) :&: (impl_free q)
impl_free   (p :|: q) = (impl_free p) :|: (impl_free q)
impl_free   (p :=: q) = impl_free $ (p :>: q) :&: (q :>: p)
impl_free   (p :>: q) = (Neg (impl_free p)) :|: (impl_free q)

-- | A variation of 'impl_free' which in this case takes a proposition, a
-- polymorphic list (accumulator), goes recursively to the leaf statements and
-- follows the same theorem.
-- 
-- Returning a 2-tuple with the first element of type [a] and the second
-- one being the proposition without implications.

impl_f_stms :: Eq a => Prop a -> [a] -> ([a], Prop a)
impl_f_stms x@(Stmnt s) acc = (s `politeCons` acc, x)
impl_f_stms   (p :=: q) acc = impl_f_stms ((p :>: q) :&: (q :>: p)) acc
impl_f_stms     (Neg p) acc = let (acp, ifp) = impl_f_stms p acc
                                in (acp, Neg ifp)
impl_f_stms   (p :&: q) acc = let (acp, ifp) = impl_f_stms p acc
                                  (acq, ifq) = impl_f_stms q acp
                                    in (acq, ifp :&: ifq)
impl_f_stms   (p :|: q) acc = let (acp, ifp) = impl_f_stms p acc
                                  (acq, ifq) = impl_f_stms q acp
                                    in (acq, ifp :|: ifq)
impl_f_stms   (p :>: q) acc = let (acp, ifp) = impl_f_stms p acc
                                  (acq, ifq) = impl_f_stms q acp
                                    in (acq, (Neg ifp) :|: ifq)

-- | Takes a proposition of type 'Prop' and it's transformed into its negative
-- normal form (NNF). First of all, the implications are removed of the
-- proposition.

nnf :: Prop a -> Prop a
nnf = nnf_aux . impl_free

-- | A variation of 'nnf' which returns a 2-tuple with the first element of type
-- [a] and the second one being the proposition in NNF.

nnf_stms :: Eq a => Prop a -> ([a], Prop a)
nnf_stms p = let (as, ifp) = impl_f_stms p [] in (as, nnf_aux ifp)

-- | Auxiliary function that assumed the imput proposition is without
-- implication and change it into its NNF.

nnf_aux :: Prop a -> Prop a
nnf_aux   (Neg (Neg p)) = nnf_aux p
nnf_aux (Neg (p :&: q)) = (nnf_aux (Neg p)) :|: (nnf_aux (Neg q))
nnf_aux (Neg (p :|: q)) = (nnf_aux (Neg p)) :&: (nnf_aux (Neg q))
nnf_aux       (p :&: q) = (nnf_aux p) :&: (nnf_aux q)
nnf_aux       (p :|: q) = (nnf_aux p) :|: (nnf_aux q)
nnf_aux               p = p
