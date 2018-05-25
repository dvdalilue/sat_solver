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

import Data.Tree -- To print BDD
import Structs.Essentials.Prop
import qualified Data.Map.Strict as Map -- Maybe deprecated
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
instance Show a => Show (BDD a) where
    show bdd = drawTree $ toDataTree bdd
        where
            toDataTree Yes = Node "Yes" []
            toDataTree No = Node "No" []
            toDataTree (Decision v y n) =
                Node (show v) [toDataTree y, toDataTree n]

-- printBDD :: BDD -> Int -> String -> String
-- printBDD (Decision v y n) d i =
--     let 
--         space = if d /= 0 then (replicate (2 ** d) ' ') else ""
--     in
--         space ++ show v ++ space ++ "\n" ++
--         (fst (split space)) ++ "/" ++ i ++ "\\" ++ (snd (split space)) ++
--         (printBDD y (d-1) (i++"  ")) ++ i ++ (printBDD n (d-1) (i++"  ")) ++ "\n"

-- split :: [a] -> ([a],[a])
-- split [] = ([],[])
-- split [x] = ([x],[])
-- split (x:y:zs) =
--     let
--         (xs,ys) = split zs
--     in
--         ((x:xs),(y:ys))

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

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------

{-
    The next functions are code in evaluation or old ideas. Nor exported
-}

mapLeaves :: BDD a -> (BDD a -> BDD a) -> BDD a
mapLeaves (Decision var y n) f = Decision var (mapLeaves y f) (mapLeaves n f)
mapLeaves x f = f x
-- mapLeaves No f = f No

propToBDD' :: Ord a => Prop a -> BDD a
propToBDD' (p :&: q) =
    let
        bdd_p = propToBDD' p
        bdd_q = propToBDD' q
    in 
        mapLeaves bdd_p $ and bdd_q
propToBDD' (p :|: q) =
    let
        bdd_p = propToBDD' p
        bdd_q = propToBDD' q
    in 
        mapLeaves bdd_p $ or bdd_q
propToBDD' (p :>: q) =
    let
        bdd_p = propToBDD' p
        bdd_q = propToBDD' q
    in 
        mapLeaves bdd_p $ (flip implies) bdd_q
propToBDD' (p :=: q) =
    let
        bdd_p = propToBDD' p
        bdd_q = propToBDD' q
    in
        mapLeaves bdd_p $ eq bdd_q
propToBDD' (Neg p)  =
    let
        bdd = propToBDD' p
    in
        neg bdd
propToBDD' x@(Stmnt _) = Decision x Yes No

robdd :: BDD a -> BDD a
robdd bdd = robdd_aux bdd Map.empty
    where
        robdd_aux :: BDD a -> Map.Map a Bool -> BDD a
        robdd_aux (Decision (Stmnt p) y n) map =
            let
                val = Map.lookup p map
            in
                case val of Nothing ->
                                Decision (Stmnt p)
                                    (robdd_aux y (Map.insert p True map))
                                    (robdd_aux n (Map.insert p False map))
                            (Just True) -> robdd_aux y map
                            (Just False) -> robdd_aux n map
        robdd_aux d map = d