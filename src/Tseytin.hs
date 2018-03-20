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

bar [                      ] acc vs = (acc,vs)
bar l@(x@(And       p q):xs) acc vs = bla bar l p q acc vs
bar l@(x@(Or        p q):xs) acc vs = bla bar l p q acc vs
bar l@(x@(Then      p q):xs) acc vs = bla bar l p q acc vs
bar l@(x@(Eq        p q):xs) acc vs = bla bar l p q acc vs
bar l@(x@(Negation  p  ):xs) acc vs = if x `elem` acc then bar (p:xs) acc vs else bar (p:xs) (x:acc) vs
bar   (x@(Statement v  ):xs) acc vs = bar (xs) acc vars -- if x `elem` acc then bar (xs)     acc else bar (xs)     (x:acc)
    where
        vars = if v `elem` vs then vs else (v:vs)

textSust :: Eq a => Prop a -> Prop a -> Prop a
textSust a@(And       p q) s@(Eq x y) = if a == y then x else (And       (textSust p s) (textSust q s))
textSust a@(Or        p q) s@(Eq x y) = if a == y then x else (Or        (textSust p s) (textSust q s))
textSust a@(Then      p q) s@(Eq x y) = if a == y then x else (Then      (textSust p s) (textSust q s))
textSust a@(Eq        p q) s@(Eq x y) = if a == y then x else (Eq        (textSust p s) (textSust q s))
textSust a@(Negation  p  ) s@(Eq x y) = if a == y then x else (Negation  (textSust p s))
textSust a@(Statement v  ) s@(Eq x y) = if a == y then x else a

-- baz :: ([Prop Char],[Char]) -> [Prop Char]
baz (ps,vs) = pop ps vs [] -- map doe ps
    where
        pop [] _ acc = acc
        pop (x:xs) vs acc =
            let
                c = getRanCharOut vs
                p = Eq (Statement c) x
            in
                pop xs (c:vs) (p:acc)

-- dam :: [Prop a] -> [Prop a]
dam [] acc = acc -- map nnf acc
dam (p:ps) acc = dam ps $ (foo p ps):acc
    where
        foo = foldl textSust 

-- tseitin :: Prop a -> Prop a
tseytin p = foldl1 And $ equivalent subformulas
    where
        subformulas@((Eq x _):_) = baz $ bar [p] [] []
        equivalent = flip dam $ [x]