module Tseytin where

import System.Random
import Structs.Essentials.Prop

getRanCharOut :: [Char] -> Char
getRanCharOut []         = fst $ randomR ('A','Z') $ mkStdGen 23
getRanCharOut chs@(ch:_) = foo ch chs $ mkStdGen 23
    where
        foo ch chs stdGen
            | ch `elem` chs = let (nc,gen) = randomR ('A','Z') stdGen 
                              in foo nc chs gen
            | otherwise = ch

bla bar (x:xs) p q acc vs = if x `elem` acc then bar (p:q:xs) acc vs else bar (p:q:xs) (x:acc) vs

bar                  [] acc vs = (acc,vs)
bar  l@(x@(p :&: q):xs) acc vs = bla bar l p q acc vs
bar  l@(x@(p :|: q):xs) acc vs = bla bar l p q acc vs
bar  l@(x@(p :>: q):xs) acc vs = bla bar l p q acc vs
bar  l@(x@(p :=: q):xs) acc vs = bla bar l p q acc vs
bar l@(x@(Neg  p  ):xs) acc vs = if x `elem` acc then bar (p:xs) acc vs else bar (p:xs) (x:acc) vs
bar  (x@(Stmnt v  ):xs) acc vs = bar (xs) acc vars -- if x `elem` acc then bar (xs)     acc else bar (xs)     (x:acc)
    where
        vars = if v `elem` vs then vs else (v:vs)

textSust :: Eq a => Prop a -> Prop a -> Prop a
textSust   a@(p :&: q) s@(x :=: y) = if a == y then x else ((textSust p s) :&: (textSust q s))
textSust   a@(p :|: q) s@(x :=: y) = if a == y then x else ((textSust p s) :|: (textSust q s))
textSust   a@(p :>: q) s@(x :=: y) = if a == y then x else ((textSust p s) :>: (textSust q s))
textSust   a@(p :=: q) s@(x :=: y) = if a == y then x else ((textSust p s) :=: (textSust q s))
textSust  a@(Neg  p  ) s@(x :=: y) = if a == y then x else (Neg  (textSust p s))
textSust a@(Stmnt v  ) s@(x :=: y) = if a == y then x else a

-- baz :: ([Prop Char],[Char]) -> [Prop Char]
baz (ps,vs) = pop ps vs [] -- map doe ps
    where
        pop [] _ acc = acc
        pop (x:xs) vs acc =
            let
                c = getRanCharOut vs
                p = (Stmnt c) :=: x
            in
                pop xs (c:vs) (p:acc)

-- dam :: [Prop a] -> [Prop a]
dam [] acc = acc -- map nnf acc
dam (p:ps) acc = dam ps $ (foo p ps):acc
    where
        foo = foldl textSust 

-- tseitin :: Prop a -> Prop a
tseytin p = foldl1 (:&:) $ equivalent subformulas
    where
        subformulas@((x :=: _):_) = baz $ bar [p] [] []
        equivalent = flip dam $ [x]