module Structs.NNF where

import Structs.Essentials.Prop

impl_free :: Prop a -> Prop a
impl_free (And       p q) = And (impl_free p) (impl_free q)
impl_free (Or        p q) = Or (impl_free p) (impl_free q)
impl_free (Then      p q) = Or (Negation (impl_free p)) (impl_free q)
impl_free (Eq        p q) = impl_free $ And (Then p q) (Then q p)
impl_free (Negation  p  ) = Negation (impl_free p)
impl_free x@(Statement _) = x

nnf :: Prop a -> Prop a
nnf = nnf_aux . impl_free
    where
        nnf_aux (Negation (Negation p)) = nnf_aux p
        nnf_aux (And p q) = And (nnf_aux p) (nnf_aux q)
        nnf_aux (Or p q) = Or (nnf_aux p) (nnf_aux q)
        nnf_aux (Negation (And p q)) = Or (nnf_aux (Negation p)) (nnf_aux (Negation q))
        nnf_aux (Negation (Or p q)) = And (nnf_aux (Negation p)) (nnf_aux (Negation q))
        nnf_aux p = p