module Structs.Essentials.Helper where

import Data.List

enviousCons :: Eq a => a -> [a] -> [a]
enviousCons x xs
    | x `elem` xs = delete x xs
    | otherwise   = x:xs

politeCons :: Eq a => a -> [a] -> [a]
politeCons x xs
    | x `elem` xs = xs
    | otherwise   = x:xs

revConcat :: [a] -> [a] -> [a]
revConcat     [] ys = ys
revConcat (x:xs) ys = revConcat xs (x:ys)

split :: [a] -> ([a],[a])
split [] = ([],[])
split [x] = ([x],[])
split (x:y:zs) =
    let
        (xs,ys) = split zs
    in
        ((x:xs),(y:ys))