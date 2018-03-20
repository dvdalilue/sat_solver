module Structs.Essentials.Prop where

data Prop a = And (Prop a) (Prop a)
            | Or (Prop a) (Prop a)
            | Then (Prop a) (Prop a)
            | Eq (Prop a) (Prop a)
            | Negation (Prop a)
            | Statement a
    deriving (Eq,Ord)

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