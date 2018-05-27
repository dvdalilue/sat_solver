---------------------------------------------------------------
-- |
-- Module      :  DPLL.CNF
-- Copyright   :  (c) David Lilue, 2018
-- License     :  see LICENSE
--
-- Maintainer  :  dvdalilue@gmail.com
-- Stability   :  experimental
-- Portability :  portable
--
-- Davis-Putnam-Logemann–Loveland (DPLL) algorithm
-- implementation using the Prop data and CNF representation
-- to resolve SAT.
--
---------------------------------------------------------------

module DPLL.CNF(sat) where

import Structs.CNF
import Structs.Essentials.Prop

import Data.List
import Data.Maybe
import Control.Applicative(pure)

-- | SAT solver using DPLL and CNF

sat :: Eq a => Prop a -> Bool
sat = uncurry (dpll []) . flatCNFStms

-- | Taking adventage of the lazy evaluation, the implementation of backtracking
-- only needs the disjunction operator. Using a decision hash, a list of
-- statements and a list of proposition in CNF.

dpll :: Eq a => [(a, Bool)] -> [a] -> [Prop a] -> Bool
dpll   _      _   [] = True
dpll hsh     [] prps = maybe False (\_ -> True) $ filterEval hsh prps
dpll hsh (s:ss) prps = case filterEval hsh prps of
                        Just nps -> dpll ((s,True):hsh) ss nps ||
                                    dpll ((s,False):hsh) ss nps
                        Nothing -> False

-- | Receives a decision hash and a list of propositions, returning Nothing if
-- at least one fails. Otherwise, returns those propositions that need more
-- information.

filterEval :: Eq a => [(a, Bool)] -> [Prop a] -> Maybe [Prop a]
filterEval hsh ps = ck [] $ zip ps $ map ((orWith hsh (Just False)) . pure) ps
    where
        ck acc                    [] = Just acc
        ck acc ((_,(Just False)):ts) = Nothing
        ck acc      ((p,Nothing):ts) = ck (p:acc) ts
        ck acc            ((_,_):ts) = ck acc ts

-- | Takes a list of 2-tuple (Map) with many statements and its corresponding
-- decision, a default value if the patterns in case expressions never match and
-- a list of proposition with just ':|:', 'Neg' and 'Stmnt' (CNF). Returning
-- Nothing if the decisions are insufficients and Just otherwise.

orWith :: Eq a => [(a, Bool)] -> Maybe Bool -> [Prop a] -> Maybe Bool
orWith  [] _                       _ = Nothing
orWith   _ b                      [] = b
orWith hsh b       ((p :|: q):stack) = orWith hsh b (p:q:stack)
orWith hsh b       ((Stmnt p):stack) = case lookup p hsh of
                                        Just True -> Just True
                                        Nothing -> orWith hsh Nothing stack
                                        otherwise -> orWith hsh b stack
orWith hsh b ((Neg (Stmnt p)):stack) = case lookup p hsh of
                                        Just False -> Just True
                                        Nothing -> orWith hsh Nothing stack
                                        otherwise -> orWith hsh b stack
