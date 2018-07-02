{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE FlexibleContexts #-}

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

infixl 3 :&:
infixl 2 :|:
infixr 1 :>:
infix  0 :=:

-- | Recursive type of logical propositions with polymorphic statements
data Prop a
    = (Prop a) :&: (Prop a) -- ^ Logical conjunction operator (':&:')
    | (Prop a) :|: (Prop a) -- ^ Logical discjuntion operator (':|:')
    | (Prop a) :>: (Prop a) -- ^ Logical implecation operator. Representing "if p ':>:' q" without "if"
    | (Prop a) :=: (Prop a) -- ^ Logical equivalence operator (':=:')
    | Neg (Prop a) -- ^ Logical negation unary operator ('Neg')
    | Stmnt a -- ^ Terminal 'Stmnt' of a logical expression
    deriving (Eq,Ord)

-- Just pretty printing the propositions
-- | @since 0.1
-- instance Show a => Show (Prop a) where
--     show p = go p $ -1
--         where
--             go (lhs :=: rhs) pre = shouldParent pre 0 $ (go lhs 0) ++ " ↔ " ++ (go rhs 0)
--             go (lhs :>: rhs) pre = shouldParent pre 1 $ (go lhs 1) ++ " → " ++ (go rhs 1)
--             go (lhs :|: rhs) pre = shouldParent pre 2 $ (go lhs 2) ++ " ∨ " ++ (go rhs 2)
--             go (lhs :&: rhs) pre = shouldParent pre 3 $ (go lhs 3) ++ " ∧ " ++ (go rhs 3)
--             go (Neg  p) pre = "¬" ++ (go p 4)
--             go (Stmnt p) pre = show p

--             shouldParent a b s = if a >= b then "(" ++ s ++ ")" else s

instance  Show (Prop (Int,Int)) where
    show p = go p
        where
            go (lhs :=: rhs) = "new_bin(EQUAL, " ++ (go lhs) ++ ", " ++ (go rhs) ++ ")"
            go (lhs :>: rhs) = "new_bin(IMPLIE, " ++ (go lhs) ++ ", " ++ (go rhs) ++ ")"
            go (lhs :|: rhs) = "new_bin(OR, " ++ (go lhs) ++ ", " ++ (go rhs) ++ ")"
            go (lhs :&: rhs) = "new_bin(AND, " ++ (go lhs) ++ ", " ++ (go rhs) ++ ")"
            go (Neg  p) = "new_neg(" ++ (go p) ++ ")"
            go (Stmnt (v,c)) = "STM(" ++ show (v + 23 * c) ++ ")"