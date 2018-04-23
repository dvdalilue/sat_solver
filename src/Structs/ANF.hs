module Structs.ANF where
-- (
--     ANF(..),
--     sExOr,
--     ,
--     fromProp
-- )
-- where

import Structs.Essentials.Prop

import Data.List
import Control.Applicative
import Control.DeepSeq

data ANF a = AND (ANF a) (ANF a)
           | XOR (ANF a) (ANF a)
           | Variable a
           | One
           | Zero
    -- deriving (Eq)

instance Eq a => Eq (ANF a) where
    (AND a b)    ==    (AND c d) = ((a == c) && (b == d)) || ((a == d) && (b == c))
    (XOR a b)    ==    (XOR c d) = ((a == c) && (b == d)) || ((a == d) && (b == c))
    (Variable a) == (Variable b) = (a == b)
    One          ==          One = True
    Zero         ==         Zero = True
    _ == _                       = False

instance NFData a => NFData (ANF a) where
    rnf a = a `seq` ()

instance Show a => Show (ANF a) where
    show p = go p $ -1
        where
            go (AND lhs rhs) pre =
                shouldParent pre 2 $ (go lhs 2) ++  "."  ++ (go rhs 2)
            go (XOR lhs rhs) pre =
                shouldParent pre 1 $ (go lhs 1) ++ " ⊕ " ++ (go rhs 1)
            go (Variable p)  pre = show p
            go (One)         pre = "1"
            go (Zero)        pre = "0"

            shouldParent a b s = if a > b then "(" ++ s ++ ")" else s

{-

Conversación de un padre con su hijo de con más de 5 años.
La idea considera que entre ellos pueden hablar como si fuese
un debate como se podría encontrar en cualquier intercambio de
palabras dirigido por jovenes, adultos y mayores. Hasta ahora
sus conversaciones han sido con sus padres, dos personas
adultas. Dejando en claro que entre esos grupo puede facilmente
generarse una discusión fluida. Y el padre llevará a su hijo a
tomar una desición que dependerá solo del niño donde el padre
le explica las situaciones en cada caso de manera imparcial.
Así como en cualquier par de personas en un debate.

-}

-- ex_or :: Eq a => Prop a -> ANF a
-- ex_or x@((p :|: q) :&: (Negation (r :&: s))) =
--     if p == r && q == s then
--         XOR (ex_or p) (ex_or q)
--     else if p == s && q == r then
--         XOR (ex_or q) (ex_or p)
--     else
--         sExOr x
-- ex_or x@((Negation (r :&: s)) :&: (p :|: q)) =
--     if p == r && q == s then
--         XOR (ex_or p) (ex_or q)
--     else if p == s && q == r then
--         XOR (ex_or q) (ex_or p)
--     else
--         sExOr x
-- ex_or x = sExOr x

sExOr :: Eq a => Prop a -> ANF a
sExOr (p :&: q)     = distr (sExOr p) (sExOr q)
sExOr (p :|: q)     =
    let exOrP = (sExOr p)
        exOrQ = (sExOr q)
    in
        XOR exOrP $ XOR exOrQ $ distr exOrP exOrQ
sExOr (p :>: q)     =
    let exOrP = (sExOr p) 
        exOrQ = (sExOr q)
    in
        XOR One $ XOR exOrP $ distr exOrP exOrQ
sExOr (p :=: q)     = sExOr $ (p :>: q) :&: (q :>: p)
sExOr (Negation  p) = XOR One (sExOr p)
sExOr (Statement p) = Variable p

distr :: ANF a -> ANF a -> ANF a
distr p (XOR q r) = XOR (distr p q) (distr p r)
distr (XOR p q) r = XOR (distr p r) (distr q r)
distr p q = AND p q

insertIfNotElem :: Eq a => a -> [a] -> [a]
insertIfNotElem x xs
    | x `elem` xs = delete x xs
    | otherwise   = x:xs

insertWOutRepeat :: Eq a => a -> [a] -> [a]
insertWOutRepeat x xs
    | x `elem` xs = xs
    | otherwise   = x:xs

-- It is assumed that the first parameter is an empty list and
-- the second one is the boolean formula without any remaining
-- possible distribution of AND over XOR.
reduceAnd :: Eq a => [ANF a] -> [ANF a] -> ANF a
reduceAnd acc ((AND p q):stack) = reduceAnd acc $ q:p:stack
reduceAnd acc (One      :stack) = reduceAnd acc stack
reduceAnd acc (Zero     :stack) = Zero
reduceAnd acc (x        :stack) = reduceAnd (insertWOutRepeat x acc) stack
reduceAnd acc []                = if null acc then One else foldl1 AND acc

reduceOr :: Eq a => [ANF a] -> [ANF a] -> [ANF a]
reduceOr acc ((XOR p q):stack) = reduceOr acc $ q:p:stack
reduceOr acc ((Zero):stack)    = reduceOr acc stack
reduceOr acc (x:stack)         = reduceOr (x:acc) stack
reduceOr acc []                = map (reduceAnd [] . pure) acc

reduce :: Eq a => ANF a -> ANF a
reduce = foldl1 XOR . foldr insertIfNotElem [] . reduceOr [] . pure

fromProp :: Eq a => Prop a -> ANF a
fromProp = reduce . sExOr

---------------------------------------------------------------
---------------------------------------------------------------
--------------------Development/Unstable-----------------------
---------------------------------------------------------------
--vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv--

-- simple x = if x == simplify x then x else simple (simplify x)

-- simplify :: ANF a -> ANF a
-- simplify (AND One q) = simplify q
-- simplify (AND p One) = simplify p
-- simplify (XOR Zero q) = simplify q
-- simplify (XOR p Zero) = simplify p
-- simplify (AND p q) = AND (simplify p) (simplify q)
-- simplify (XOR p q) = XOR (simplify p) (simplify q)
-- simplify p = p