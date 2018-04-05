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

module Structs.NNF where

import Structs.Essentials.Prop

-- | Transform a proposition to an equivalent one without implications. 
impl_free :: Prop a -- ^ Input 'Prop' without guarantee of implication absences
          -> Prop a -- ^ Output 'Prop' without any 'Then' constructor
impl_free (p :&: q)       = (impl_free p) :&: (impl_free q)
impl_free (p :|: q)       = (impl_free p) :|: (impl_free q)
impl_free (p :>: q)       = (Negation (impl_free p)) :|: (impl_free q)
impl_free (p :=: q)       = impl_free $ (p :>: q) :&: (q :>: p)
impl_free (Negation  p  ) = Negation (impl_free p)
impl_free x@(Statement _) = x

-- | Transform a proposition into negative normal form (NNF).
nnf :: Prop a -- ^ Input 'Prop' in any form
    -> Prop a -- ^ Output 'Prop' in negative normal form
nnf = nnf_aux . impl_free
-- The proposition has to lack of implications to be transformed into NNF
    where
        nnf_aux (Negation (Negation p)) = nnf_aux p
        nnf_aux (p :&: q) = (nnf_aux p) :&: (nnf_aux q)
        nnf_aux (p :|: q) = (nnf_aux p) :|: (nnf_aux q)
        nnf_aux (Negation (p :&: q)) = (nnf_aux (Negation p)) :|: (nnf_aux (Negation q))
        nnf_aux (Negation (p :|: q)) = (nnf_aux (Negation p)) :&: (nnf_aux (Negation q))
        nnf_aux p = p