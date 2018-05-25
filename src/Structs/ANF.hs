{-# LANGUAGE FlexibleContexts #-}

module Structs.ANF where
-- (
--     ANF(..),
--     sExOr,
--     ,
--     fromProp
-- )
-- where

import Structs.Essentials.Prop
import Structs.Essentials.Helper

import Data.List
import Control.DeepSeq
import Control.Applicative

data ANF a = AND (ANF a) (ANF a)
           | XOR (ANF a) (ANF a)
           | Variable a
           | One
           | Zero
    deriving (Eq)

instance Ord a => Ord (ANF a) where
    compare Zero Zero = EQ
    compare One One = EQ
    compare Zero a = LT
    compare a Zero = GT
    compare One a = LT
    compare a One = GT
    compare (Variable a) (Variable b) = compare a b
    compare a@(Variable _) b = LT
    compare a b@(Variable _) = GT
    compare x@(AND _ _) y@(AND _ _) =
        case x == y of
            True -> EQ
            False -> deepCompare x y
    compare x@(XOR _ _) y@(XOR _ _) =
        if x /= y then
            case components x < components y of
                True -> LT
                False ->
                    case components x == components y of
                        True -> deepCompare x y
                        False -> GT
        else EQ
        -- where
deepCompare ((XOR a b)) ((XOR c d)) =
    case compare a c of
        LT -> LT
        GT -> GT
        EQ -> compare (b) (d)
deepCompare ((AND a b)) ((AND c d)) =
    case compare a c of
        LT -> LT
        GT -> GT
        EQ -> compare (b) (d)

-- instance Eq a => Eq (ANF a) where
--     (AND a b)    ==    (AND c d) = ((a == c) && (b == d)) || ((a == d) && (b == c))
--     (XOR a b)    ==    (XOR c d) = ((a == c) && (b == d)) || ((a == d) && (b == c))
--     (Variable a) == (Variable b) = (a == b)
--     One          ==          One = True
--     Zero         ==         Zero = True
--     _ == _                       = False

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
adultas. Dejando en claro que entre esos grupos puede
generarse una discusión fluida. Y el padre llevará a su hijo a
tomar una desición que dependerá solo del niño donde el padre
le explica las situaciones en cada caso de manera imparcial.
Así como en cualquier par de personas en un debate.

-}

components :: ANF a -> Int
components = count [] . pure
    where
        count acc ((AND x y):stack) = count (x:acc) (y:stack)
        count acc ((XOR x y):stack) = count (x:acc) (y:stack)
        count acc (x@(Variable _):stack) = count (x:acc) stack
        count acc [] = length acc

sExOr' :: (Ord (ANF a), Ord a, Eq a) => Prop a -> ANF a
sExOr' (p :&: q) = reduce $ distr (sExOr' p) (sExOr' q)
sExOr' (p :|: q) =
    let exOrP = reduce (sExOr' p)
        exOrQ = reduce (sExOr' q)
        opds = sort [exOrP, exOrQ, reduce (distr exOrP exOrQ)]
    in
        foldr1 XOR opds
sExOr' (p :>: q) =
    let exOrP = reduce (sExOr' p)
        exOrQ = reduce (sExOr' q)
        opds = sort [One, exOrP, reduce (distr exOrP exOrQ)]
    in
        foldr1 XOR opds
sExOr' (p :=: q) = sExOr' $ (p :>: q) :&: (q :>: p)
sExOr'  (Neg  p) = XOR One $ reduce (sExOr' p)
sExOr' (Stmnt p) = Variable p

sExOr :: Eq a => Prop a -> ANF a
sExOr (p :&: q) = distr (sExOr p) (sExOr q)
sExOr (p :|: q) =
    let exOrP = (sExOr p)
        exOrQ = (sExOr q)
    in
        XOR exOrP $ XOR exOrQ $ distr exOrP exOrQ
sExOr (p :>: q) =
    let exOrP = (sExOr p)
        exOrQ = (sExOr q)
    in
        XOR One $ XOR exOrP $ distr exOrP exOrQ
sExOr (p :=: q) = sExOr $ (p :>: q) :&: (q :>: p)
sExOr  (Neg  p) = XOR One (sExOr p)
sExOr (Stmnt p) = Variable p

distr :: ANF a -> ANF a -> ANF a
distr p (XOR q r) = XOR (distr p q) (distr p r)
distr (XOR p q) r = XOR (distr p r) (distr q r)
distr p q = AND p q

-- It is assumed that the first parameter is an empty list and
-- the second one is the boolean formula without any remaining
-- possible distribution of AND over XOR.
reduceAnd :: (Ord (ANF a), Ord a, Eq a) => [ANF a] -> [ANF a] -> ANF a
reduceAnd acc ((AND p q):stack) = reduceAnd acc $ q:p:stack
reduceAnd acc (      One:stack) = reduceAnd acc stack
reduceAnd acc (     Zero:stack) = Zero
reduceAnd acc (        x:stack) = reduceAnd (politeCons x acc) stack
reduceAnd acc                [] = if null acc then One else foldr1 AND (sort acc)

reduceOr :: (Ord (ANF a), Ord a, Eq a) => [ANF a] -> [ANF a] -> [ANF a]
reduceOr acc ((XOR p q):stack) = reduceOr acc $ q:p:stack
reduceOr acc (   (Zero):stack) = reduceOr acc stack
reduceOr acc (        x:stack) = reduceOr (x:acc) stack
reduceOr acc                [] = sort . map (reduceAnd [] . pure) $ acc

reduce :: (Ord (ANF a), Ord a, Eq a) => ANF a -> ANF a
reduce = foldr1 XOR . foldr enviousCons [] . reduceOr [] . pure

fromProp :: (Ord (ANF a), Ord a, Eq a) => Prop a -> ANF a
fromProp = sExOr'
-- fromProp = reduce . sExOr