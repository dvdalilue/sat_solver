{-# LANGUAGE GADTs #-}
{-# LANGUAGE StandaloneDeriving #-}

---------------------------------------------------------------
-- |
-- Module      :  Structs.BDD
-- Copyright   :  (c) David Lilue, 2018
-- License     :  see LICENSE
--
-- Maintainer  :  dvdalilue@gmail.com
-- Stability   :  experimental
-- Portability :  portable
--
-- Binary decision diagram (BDD) implementation using Prop data
-- type.
--
---------------------------------------------------------------

module Structs.BDD(
    BDD(..),
    neg,
    eq,
    and,
    or,
    implies,
    fromProp,
) where

import Structs.Essentials.Prop
import Structs.Essentials.Helper

import Data.Tree -- To print BDD
import Prelude hiding (and,or)

-- | Recursive data type of a binary decision diagram. Storing
-- a proposition as identifier. The polymorphic type requires
-- having an instance of the class 'Ord'.
data BDD a where
    Decision :: Ord a => Prop a -> BDD a -> BDD a -> BDD a
    Yes :: Ord a => BDD a
    No :: Ord a => BDD a
deriving instance Eq (BDD a)

-- Just pretty printing the BDD with 'Data.Tree'
-- | @since 0.1
-- instance Show a => Show (BDD a) where
--     show bdd = drawTree $ toDataTree bdd
--         where
--             toDataTree Yes = Node "Yes" []
--             toDataTree No = Node "No" []
--             toDataTree (Decision v y n) =
--                 Node (show v) [toDataTree y, toDataTree n]

-- printBDD :: BDD -> Int -> String -> String
-- printBDD (Decision v y n) d i =
--     let 
--         space = if d /= 0 then (replicate (2 ** d) ' ') else ""
--     in
--         space ++ show v ++ space ++ "\n" ++
--         (fst (split space)) ++ "/" ++ i ++ "\\" ++ (snd (split space)) ++
--         (printBDD y (d-1) (i++"  ")) ++ i ++ (printBDD n (d-1) (i++"  ")) ++ "\n"

-- | Apply negation operation to a BDD
neg :: BDD a -> BDD a
neg No = Yes
neg Yes = No
neg (Decision v y n) = Decision v (neg y) (neg n)

-- | Apply a binary function (operator) to two operands.
-- Preserving the orden in the BDD and reducing if possible,
-- resulting in a reduced ordered binary decision diagram
-- (ROBDD). Taking in consideration that both operands are
-- ROBDDs.
apply :: (BDD a -> BDD a -> BDD a) -> BDD a -> BDD a -> BDD a
apply op bdd1@(Decision x1 y1 n1) bdd2@(Decision x2 y2 n2)
    | x1 < x2 =
        let
            ldd = op y1 bdd2
            rdd = op n1 bdd2
        in
            if ldd == rdd then ldd else Decision x1 ldd rdd
    | x1 > x2 = 
        let
            ldd = op y2 bdd1
            rdd = op n2 bdd1
        in
            if ldd == rdd then ldd else Decision x2 ldd rdd
    | otherwise =
        let
            ldd = (op y1 y2)
            rdd = (op n1 n2)
        in
            if ldd == rdd then ldd else Decision x1 ldd rdd

-- | Equivalence operation between two BDDs. Commutative.
eq :: BDD a -> BDD a -> BDD a
eq No d = neg d
eq d No = neg d
eq Yes d = d
eq d Yes = d
eq bdd1 bdd2 = apply eq bdd1 bdd2

-- | Conjunction operation between two BDDs. Commutative.
and :: BDD a -> BDD a -> BDD a
and No _ = No
and _ No = No
and Yes d = d
and d Yes = d
and bdd1 bdd2 = apply and bdd1 bdd2

-- | Disjunction operation between two BDDs. Commutative.
or :: BDD a -> BDD a -> BDD a
or Yes _ = Yes
or _ Yes = Yes
or No d = d
or d No = d
or bdd1 bdd2 = apply or bdd1 bdd2

-- | Implication operation between two BDDs. Noncommutative.
implies :: BDD a -> BDD a -> BDD a
implies No  _ = Yes
implies Yes d = d
implies bdd1 bdd2 = or (neg bdd1) bdd2

-- | Transform a proposition into a BDD.
fromProp :: Ord a => Prop a -> BDD a
fromProp   (p :&: q) = and     (fromProp p) (fromProp q)
fromProp   (p :|: q) = or      (fromProp p) (fromProp q)
fromProp   (p :>: q) = implies (fromProp p) (fromProp q)
fromProp   (p :=: q) = eq      (fromProp p) (fromProp q)
fromProp  (Neg  p  ) = neg     (fromProp p)
fromProp x@(Stmnt _) = Decision x Yes No
