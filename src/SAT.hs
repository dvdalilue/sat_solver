-- {-# LANGUAGE DataKinds #-}

module SAT where

import Structs.Essentials.Prop
import Structs.CNF
import Structs.NNF
import Structs.BDD
import Structs.ANF
import Tseytin

import Data.List
-- import Data.Pattern
import Data.Time.Clock.POSIX
import System.Random
import Data.Functor
import Control.DeepSeq
import System.CPUTime

import Prelude hiding (and,or)

p = (Statement 'P')
q = (Statement 'Q')
r = (Statement 'R')

modus_ponen = (p :>: q) :&: p :>: q

{-

     a b c d
    +-+-+-+-+
   a| | | | |
    +-+-+-+-+
   b| | | | |
    +-+-+-+-+
   c| | | | |
    +-+-+-+-+
   d| | | | |
    +-+-+-+-+

  aa1 = up-left cell with value '1'
  da3 = dowon-left cell with value '3'

-}

type CellValue = Prop (Char,Char,Integer)

aa1 = Statement ('A','A',1)
ab1 = Statement ('A','B',1)
ac1 = Statement ('A','C',1)
ad1 = Statement ('A','D',1)
ba1 = Statement ('B','A',1)
bb1 = Statement ('B','B',1)
bc1 = Statement ('B','C',1)
bd1 = Statement ('B','D',1)
ca1 = Statement ('C','A',1)
cb1 = Statement ('C','B',1)
cc1 = Statement ('C','C',1)
cd1 = Statement ('C','D',1)
da1 = Statement ('D','A',1)
db1 = Statement ('D','B',1)
dc1 = Statement ('D','C',1)
dd1 = Statement ('D','D',1)
aa2 = Statement ('A','A',2)
ab2 = Statement ('A','B',2)
ac2 = Statement ('A','C',2)
ad2 = Statement ('A','D',2)
ba2 = Statement ('B','A',2)
bb2 = Statement ('B','B',2)
bc2 = Statement ('B','C',2)
bd2 = Statement ('B','D',2)
ca2 = Statement ('C','A',2)
cb2 = Statement ('C','B',2)
cc2 = Statement ('C','C',2)
cd2 = Statement ('C','D',2)
da2 = Statement ('D','A',2)
db2 = Statement ('D','B',2)
dc2 = Statement ('D','C',2)
dd2 = Statement ('D','D',2)
aa3 = Statement ('A','A',3)
ab3 = Statement ('A','B',3)
ac3 = Statement ('A','C',3)
ad3 = Statement ('A','D',3)
ba3 = Statement ('B','A',3)
bb3 = Statement ('B','B',3)
bc3 = Statement ('B','C',3)
bd3 = Statement ('B','D',3)
ca3 = Statement ('C','A',3)
cb3 = Statement ('C','B',3)
cc3 = Statement ('C','C',3)
cd3 = Statement ('C','D',3)
da3 = Statement ('D','A',3)
db3 = Statement ('D','B',3)
dc3 = Statement ('D','C',3)
dd3 = Statement ('D','D',3)
aa4 = Statement ('A','A',4)
ab4 = Statement ('A','B',4)
ac4 = Statement ('A','C',4)
ad4 = Statement ('A','D',4)
ba4 = Statement ('B','A',4)
bb4 = Statement ('B','B',4)
bc4 = Statement ('B','C',4)
bd4 = Statement ('B','D',4)
ca4 = Statement ('C','A',4)
cb4 = Statement ('C','B',4)
cc4 = Statement ('C','C',4)
cd4 = Statement ('C','D',4)
da4 = Statement ('D','A',4)
db4 = Statement ('D','B',4)
dc4 = Statement ('D','C',4)
dd4 = Statement ('D','D',4)
-- aa5 = Statement ('A','A',5)
-- ab5 = Statement ('A','B',5)
-- ac5 = Statement ('A','C',5)
-- ad5 = Statement ('A','D',5)
-- ba5 = Statement ('B','A',5)
-- bb5 = Statement ('B','B',5)
-- bc5 = Statement ('B','C',5)
-- bd5 = Statement ('B','D',5)
-- ca5 = Statement ('C','A',5)
-- cb5 = Statement ('C','B',5)
-- cc5 = Statement ('C','C',5)
-- cd5 = Statement ('C','D',5)
-- da5 = Statement ('D','A',5)
-- db5 = Statement ('D','B',5)
-- dc5 = Statement ('D','C',5)
-- dd5 = Statement ('D','D',5)
-- aa6 = Statement ('A','A',6)
-- ab6 = Statement ('A','B',6)
-- ac6 = Statement ('A','C',6)
-- ad6 = Statement ('A','D',6)
-- ba6 = Statement ('B','A',6)
-- bb6 = Statement ('B','B',6)
-- bc6 = Statement ('B','C',6)
-- bd6 = Statement ('B','D',6)
-- ca6 = Statement ('C','A',6)
-- cb6 = Statement ('C','B',6)
-- cc6 = Statement ('C','C',6)
-- cd6 = Statement ('C','D',6)
-- da6 = Statement ('D','A',6)
-- db6 = Statement ('D','B',6)
-- dc6 = Statement ('D','C',6)
-- dd6 = Statement ('D','D',6)
-- aa7 = Statement ('A','A',7)
-- ab7 = Statement ('A','B',7)
-- ac7 = Statement ('A','C',7)
-- ad7 = Statement ('A','D',7)
-- ba7 = Statement ('B','A',7)
-- bb7 = Statement ('B','B',7)
-- bc7 = Statement ('B','C',7)
-- bd7 = Statement ('B','D',7)
-- ca7 = Statement ('C','A',7)
-- cb7 = Statement ('C','B',7)
-- cc7 = Statement ('C','C',7)
-- cd7 = Statement ('C','D',7)
-- da7 = Statement ('D','A',7)
-- db7 = Statement ('D','B',7)
-- dc7 = Statement ('D','C',7)
-- dd7 = Statement ('D','D',7)
-- aa8 = Statement ('A','A',8)
-- ab8 = Statement ('A','B',8)
-- ac8 = Statement ('A','C',8)
-- ad8 = Statement ('A','D',8)
-- ba8 = Statement ('B','A',8)
-- bb8 = Statement ('B','B',8)
-- bc8 = Statement ('B','C',8)
-- bd8 = Statement ('B','D',8)
-- ca8 = Statement ('C','A',8)
-- cb8 = Statement ('C','B',8)
-- cc8 = Statement ('C','C',8)
-- cd8 = Statement ('C','D',8)
-- da8 = Statement ('D','A',8)
-- db8 = Statement ('D','B',8)
-- dc8 = Statement ('D','C',8)
-- dd8 = Statement ('D','D',8)
-- aa9 = Statement ('A','A',9)
-- ab9 = Statement ('A','B',9)
-- ac9 = Statement ('A','C',9)
-- ad9 = Statement ('A','D',9)
-- ba9 = Statement ('B','A',9)
-- bb9 = Statement ('B','B',9)
-- bc9 = Statement ('B','C',9)
-- bd9 = Statement ('B','D',9)
-- ca9 = Statement ('C','A',9)
-- cb9 = Statement ('C','B',9)
-- cc9 = Statement ('C','C',9)
-- cd9 = Statement ('C','D',9)
-- da9 = Statement ('D','A',9)
-- db9 = Statement ('D','B',9)
-- dc9 = Statement ('D','C',9)
-- dd9 = Statement ('D','D',9)

sudokuValues = [
    aa1,
    ab1,
    ac1,
    ad1,
    ba1,
    bb1,
    bc1,
    bd1,
    ca1,
    cb1,
    cc1,
    cd1,
    da1,
    db1,
    dc1,
    dd1,
    aa2,
    ab2,
    ac2,
    ad2,
    ba2,
    bb2,
    bc2,
    bd2,
    ca2,
    cb2,
    cc2,
    cd2,
    da2,
    db2,
    dc2,
    dd2,
    aa3,
    ab3,
    ac3,
    ad3,
    ba3,
    bb3,
    bc3,
    bd3,
    ca3,
    cb3,
    cc3,
    cd3,
    da3,
    db3,
    dc3,
    dd3,
    aa4,
    ab4,
    ac4,
    ad4,
    ba4,
    bb4,
    bc4,
    bd4,
    ca4,
    cb4,
    cc4,
    cd4,
    da4,
    db4,
    dc4,
    dd4]

case1 = [ad3,bb3,bc2,cb4,cc1,da1]

sud = SAT.and $ map foo sudokuValues
    -- where
foo x = SAT.and [(x :>: SAT.or (rows x)),(x :>: SAT.or (cols x)),(x :>: SAT.or (quad x))]
bar x = SAT.and [(SAT.or (rows x)),(SAT.or (cols x)),(SAT.or (quad x))]

rows :: CellValue -> [CellValue]
rows (Statement (r,c,v)) =
    [Statement (r,nc,nv) | nc <- ['A'..'D'], nv <- [1..4], nc /= c, nv /= v]

cols :: CellValue -> [CellValue]
cols (Statement (r,c,v)) =
    [Statement (nr,c,nv) | nr <- ['A'..'D'], nv <- [1..4], nr /= r, nv /= v]

quad :: CellValue -> [CellValue]
quad (Statement (r,c,v)) =
    [Statement (nr,nc,nv) |
        nr <- (quad_range r),
        nc <- (quad_range c),
        (nr /= r || nc /= c),
        nv <- [1..4], nv /= v
    ]
        where
            quad_range i
                | i <= 'B'  = ['A','B']
                | otherwise = ['C','D']

---------------------------------------------------------------
----------------------- Graph coloring ------------------------
---------------------------------------------------------------

type AdjacencyList a = [[a]]

data Graph a = Graph {
    nVertex :: Int,
    nEdge :: Int,
    adjList :: AdjacencyList a
} deriving Show

{-
randFoo = do
    seed <- round . (*1000) <$> getPOSIXTime
    return seed
-}

graph0 = Graph 3 2 [[2],[1,3],[2]] :: Graph Int

-- benchmark 3 colors: 30391471000000 picoseconds
graph1 = Graph 4 4 [[2,4],[1,3],[2,4],[1,3]] :: Graph Int

graph2 = Graph 10 10 [[2],[1,3],[2,5],[2,5,6,7,8],[3,4,6,7],[4,5,7,10],[4,6,8,9,10],[4,7],[5,7],[6,7]] :: Graph Int

-- benchmark 3 colors: 1311235211000000 picoseconds
graph3 = Graph 5 5 [[3,5],[3,4,5],[1,2,4],[2,3,5],[1,2,4]] :: Graph Int

possibleColorings v nc = [Statement (n,c) | n <- [1..v], c <- take nc ['A'..'Z']]

-- constraints :: Prop (a,b) -> Graph a -> [Prop (a,b)]
constraints g colors p@(Statement (n,c)) =
    (p :>: (SAT.and $ [Negation (Statement (x,c)) | x <- ((adjList g) !! (n-1))])) :&:
    (p :=: (SAT.and $ [Negation (Statement (n,x)) | x <- take colors ['A'..'Z'], x /= c]))

graph_coloring g colors = SAT.and $ map (constraints g colors) (possibleColorings (nVertex g) colors)

n1a = Statement (1,'A') :: Prop (Int, Char)
n1b = Statement (1,'B')
n2a = Statement (2,'A')
n2b = Statement (2,'B')
n3a = Statement (3,'A')
n3b = Statement (3,'B')
n4a = Statement (4,'A')
n4b = Statement (4,'B')

gc =
    (n1a :>: Negation n2a :&: Negation n4a) :&:
    (n1b :>: Negation n2b :&: Negation n4b) :&:

    (n2a :>: Negation n1a :&: Negation n3a) :&:
    (n2b :>: Negation n1b :&: Negation n3b) :&:

    (n3a :>: Negation n2a :&: Negation n4a) :&:
    (n3b :>: Negation n2b :&: Negation n4b) :&:

    (n4a :>: Negation n1a :&: Negation n3a) :&:
    (n4b :>: Negation n1b :&: Negation n3b) :&:

    (n1a :=: Negation n1b) :&:
    (n2a :=: Negation n2b) :&:
    (n3a :=: Negation n3b) :&:
    (n4a :=: Negation n4b)

{-
    Utilities
-}

or :: [Prop a] -> Prop a
or = foldl1 (:|:) 

-- or :: [CellValue] -> CellValue
-- or [] = error "Cannot construct cell from nothing"
-- or (c:cs) = or_aux cs c
--     where
--         or_aux [] acc = acc
--         or_aux (c:cs) acc = or_aux cs (acc :|: c)

and :: [Prop a] -> Prop a
and = foldl1 (:&:)

-- and :: [CellValue] -> CellValue
-- and [] = error "Cannot construct cell from nothing"
-- and (c:cs) = and_aux cs c
--     where
--         and_aux [] acc = acc
--         and_aux (c:cs) acc = and_aux cs (acc :&: c)

benchmark :: Graph Int -> Int -> IO Integer
benchmark g c = do
    start <- getCPUTime
    let r = Structs.ANF.fromProp $ nnf $ graph_coloring g c
    end <- r `deepseq` getCPUTime
    return (end - start)

-- fib = scanl1 (+) $ 0:1:fib

-- class Distributable a where
--     add, mul :: a -> a -> a

--     -- aux_f c a b = add (SAT.distr a c) (SAT.distr b c)
--     -- aux_s c a b = add (SAT.distr c a) (SAT.distr c b)

--     distr :: a -> a -> a
--     distr m c = match m $ (add var var) ->> (aux_f c)
--         where
--             aux_f c a b :: Fun '[a] -> a
--             aux_f c a b = add (SAT.distr a c) (SAT.distr b c)
--     distr c m = match m $ (add var var) ->> (aux_s c)
--         where aux_s c a b = add (SAT.distr c a) (SAT.distr c b)
--     distr a b = mul a b