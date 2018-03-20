---------------------------------------------------------------
-- |
-- Module      :  Structs.Essentials.Prop
-- Copyright   :  (c) David Lilue, 2018
-- License     :  see LICENSE
--
-- Maintainer  :  dvdalilue@gmail.com
-- Stability   :  experimental
-- Portability :  portable
--
-- Propositional logic abstract syntax implementation.
--
---------------------------------------------------------------

module Structs.Essentials.Prop where

-- | Recursive type of logical propositions with polymorphic statements
data Prop a
    = And (Prop a) (Prop a) -- ^ Logical conjunction operator ('And')
    | Or (Prop a) (Prop a) -- ^ Logical discjuntion operator ('Or')
    | Then (Prop a) (Prop a) -- ^ Logical implecation operator. Representing "if p 'Then' q" without "if"
    | Eq (Prop a) (Prop a) -- ^ Logical equivalence operator ('Eq')
    | Negation (Prop a) -- ^ Logical negation unary operator ('Negation')
    | Statement a -- ^ Terminal 'Statement' of a logical expression
    deriving (Eq,Ord)

-- Just pretty printing the propositions
-- | @since 0.1
instance Show a => Show (Prop a) where
    show p = go p $ -1
        where
            go (Eq lhs rhs) pre   = shouldParent pre 0 $ (go lhs 0) ++ " ↔ " ++ (go rhs 0)
            go (Then lhs rhs) pre = shouldParent pre 1 $ (go lhs 1) ++ " → " ++ (go rhs 1)
            go (Or lhs rhs) pre   = shouldParent pre 2 $ (go lhs 2) ++ " ∨ " ++ (go rhs 2)
            go (And lhs rhs) pre  = shouldParent pre 3 $ (go lhs 3) ++ " ∧ " ++ (go rhs 3)
            go (Negation  p) pre  = "¬" ++ (go p 4)
            go (Statement p) pre  = show p

            shouldParent a b s = if a >= b then "(" ++ s ++ ")" else s