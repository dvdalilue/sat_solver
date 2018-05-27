-- {-# LANGUAGE DataKinds #-}

module SAT where

import Structs.Essentials.Prop
import Structs.CNF
import Structs.NNF
import Structs.BDD
import Structs.ANF
import Tseytin

import DPLL.CNF

import Data.List
-- import Data.Pattern
import Data.Time.Clock.POSIX
import System.Random
import Data.Functor
import Control.DeepSeq
import System.CPUTime

import Prelude hiding (and,or)

p = (Stmnt 'P')
q = (Stmnt 'Q')
r = (Stmnt 'R')

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

aa1 = Stmnt ('A','A',1)
ab1 = Stmnt ('A','B',1)
ac1 = Stmnt ('A','C',1)
ad1 = Stmnt ('A','D',1)
ba1 = Stmnt ('B','A',1)
bb1 = Stmnt ('B','B',1)
bc1 = Stmnt ('B','C',1)
bd1 = Stmnt ('B','D',1)
ca1 = Stmnt ('C','A',1)
cb1 = Stmnt ('C','B',1)
cc1 = Stmnt ('C','C',1)
cd1 = Stmnt ('C','D',1)
da1 = Stmnt ('D','A',1)
db1 = Stmnt ('D','B',1)
dc1 = Stmnt ('D','C',1)
dd1 = Stmnt ('D','D',1)
aa2 = Stmnt ('A','A',2)
ab2 = Stmnt ('A','B',2)
ac2 = Stmnt ('A','C',2)
ad2 = Stmnt ('A','D',2)
ba2 = Stmnt ('B','A',2)
bb2 = Stmnt ('B','B',2)
bc2 = Stmnt ('B','C',2)
bd2 = Stmnt ('B','D',2)
ca2 = Stmnt ('C','A',2)
cb2 = Stmnt ('C','B',2)
cc2 = Stmnt ('C','C',2)
cd2 = Stmnt ('C','D',2)
da2 = Stmnt ('D','A',2)
db2 = Stmnt ('D','B',2)
dc2 = Stmnt ('D','C',2)
dd2 = Stmnt ('D','D',2)
aa3 = Stmnt ('A','A',3)
ab3 = Stmnt ('A','B',3)
ac3 = Stmnt ('A','C',3)
ad3 = Stmnt ('A','D',3)
ba3 = Stmnt ('B','A',3)
bb3 = Stmnt ('B','B',3)
bc3 = Stmnt ('B','C',3)
bd3 = Stmnt ('B','D',3)
ca3 = Stmnt ('C','A',3)
cb3 = Stmnt ('C','B',3)
cc3 = Stmnt ('C','C',3)
cd3 = Stmnt ('C','D',3)
da3 = Stmnt ('D','A',3)
db3 = Stmnt ('D','B',3)
dc3 = Stmnt ('D','C',3)
dd3 = Stmnt ('D','D',3)
aa4 = Stmnt ('A','A',4)
ab4 = Stmnt ('A','B',4)
ac4 = Stmnt ('A','C',4)
ad4 = Stmnt ('A','D',4)
ba4 = Stmnt ('B','A',4)
bb4 = Stmnt ('B','B',4)
bc4 = Stmnt ('B','C',4)
bd4 = Stmnt ('B','D',4)
ca4 = Stmnt ('C','A',4)
cb4 = Stmnt ('C','B',4)
cc4 = Stmnt ('C','C',4)
cd4 = Stmnt ('C','D',4)
da4 = Stmnt ('D','A',4)
db4 = Stmnt ('D','B',4)
dc4 = Stmnt ('D','C',4)
dd4 = Stmnt ('D','D',4)
-- aa5 = Stmnt ('A','A',5)
-- ab5 = Stmnt ('A','B',5)
-- ac5 = Stmnt ('A','C',5)
-- ad5 = Stmnt ('A','D',5)
-- ba5 = Stmnt ('B','A',5)
-- bb5 = Stmnt ('B','B',5)
-- bc5 = Stmnt ('B','C',5)
-- bd5 = Stmnt ('B','D',5)
-- ca5 = Stmnt ('C','A',5)
-- cb5 = Stmnt ('C','B',5)
-- cc5 = Stmnt ('C','C',5)
-- cd5 = Stmnt ('C','D',5)
-- da5 = Stmnt ('D','A',5)
-- db5 = Stmnt ('D','B',5)
-- dc5 = Stmnt ('D','C',5)
-- dd5 = Stmnt ('D','D',5)
-- aa6 = Stmnt ('A','A',6)
-- ab6 = Stmnt ('A','B',6)
-- ac6 = Stmnt ('A','C',6)
-- ad6 = Stmnt ('A','D',6)
-- ba6 = Stmnt ('B','A',6)
-- bb6 = Stmnt ('B','B',6)
-- bc6 = Stmnt ('B','C',6)
-- bd6 = Stmnt ('B','D',6)
-- ca6 = Stmnt ('C','A',6)
-- cb6 = Stmnt ('C','B',6)
-- cc6 = Stmnt ('C','C',6)
-- cd6 = Stmnt ('C','D',6)
-- da6 = Stmnt ('D','A',6)
-- db6 = Stmnt ('D','B',6)
-- dc6 = Stmnt ('D','C',6)
-- dd6 = Stmnt ('D','D',6)
-- aa7 = Stmnt ('A','A',7)
-- ab7 = Stmnt ('A','B',7)
-- ac7 = Stmnt ('A','C',7)
-- ad7 = Stmnt ('A','D',7)
-- ba7 = Stmnt ('B','A',7)
-- bb7 = Stmnt ('B','B',7)
-- bc7 = Stmnt ('B','C',7)
-- bd7 = Stmnt ('B','D',7)
-- ca7 = Stmnt ('C','A',7)
-- cb7 = Stmnt ('C','B',7)
-- cc7 = Stmnt ('C','C',7)
-- cd7 = Stmnt ('C','D',7)
-- da7 = Stmnt ('D','A',7)
-- db7 = Stmnt ('D','B',7)
-- dc7 = Stmnt ('D','C',7)
-- dd7 = Stmnt ('D','D',7)
-- aa8 = Stmnt ('A','A',8)
-- ab8 = Stmnt ('A','B',8)
-- ac8 = Stmnt ('A','C',8)
-- ad8 = Stmnt ('A','D',8)
-- ba8 = Stmnt ('B','A',8)
-- bb8 = Stmnt ('B','B',8)
-- bc8 = Stmnt ('B','C',8)
-- bd8 = Stmnt ('B','D',8)
-- ca8 = Stmnt ('C','A',8)
-- cb8 = Stmnt ('C','B',8)
-- cc8 = Stmnt ('C','C',8)
-- cd8 = Stmnt ('C','D',8)
-- da8 = Stmnt ('D','A',8)
-- db8 = Stmnt ('D','B',8)
-- dc8 = Stmnt ('D','C',8)
-- dd8 = Stmnt ('D','D',8)
-- aa9 = Stmnt ('A','A',9)
-- ab9 = Stmnt ('A','B',9)
-- ac9 = Stmnt ('A','C',9)
-- ad9 = Stmnt ('A','D',9)
-- ba9 = Stmnt ('B','A',9)
-- bb9 = Stmnt ('B','B',9)
-- bc9 = Stmnt ('B','C',9)
-- bd9 = Stmnt ('B','D',9)
-- ca9 = Stmnt ('C','A',9)
-- cb9 = Stmnt ('C','B',9)
-- cc9 = Stmnt ('C','C',9)
-- cd9 = Stmnt ('C','D',9)
-- da9 = Stmnt ('D','A',9)
-- db9 = Stmnt ('D','B',9)
-- dc9 = Stmnt ('D','C',9)
-- dd9 = Stmnt ('D','D',9)

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
rows (Stmnt (r,c,v)) =
    [Stmnt (r,nc,nv) | nc <- ['A'..'D'], nv <- [1..4], nc /= c, nv /= v]

cols :: CellValue -> [CellValue]
cols (Stmnt (r,c,v)) =
    [Stmnt (nr,c,nv) | nr <- ['A'..'D'], nv <- [1..4], nr /= r, nv /= v]

quad :: CellValue -> [CellValue]
quad (Stmnt (r,c,v)) =
    [Stmnt (nr,nc,nv) |
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

-- benchmark 3 colors: 30391471000000 picoseconds (20 sec)
graph1 = Graph 4 4 [[2,4],[1,3],[2,4],[1,3]] :: Graph Int

graph2 = Graph 10 10 [[2],[1,3],[2,5],[2,5,6,7,8],[3,4,6,7],[4,5,7,10],[4,6,8,9,10],[4,7],[5,7],[6,7]] :: Graph Int

-- benchmark 3 colors: 1311235211000000 picoseconds (20 min)
graph3 = Graph 5 5 [[3,5],[3,4,5],[1,2,4],[2,3,5],[1,2,4]] :: Graph Int

possibleColorings v nc = [Stmnt (n,c) | n <- [1..v], c <- take nc ['A'..'Z']]

-- constraints :: Prop (a,b) -> Graph a -> [Prop (a,b)]
constraints g colors p@(Stmnt (n,c)) =
    (p :>: (SAT.and $ [Neg (Stmnt (x,c)) | x <- ((adjList g) !! (n-1))])) :&:
    (p :=: (SAT.and $ [Neg (Stmnt (n,x)) | x <- take colors ['A'..'Z'], x /= c]))

graph_coloring g colors = SAT.and $ map (constraints g colors) (possibleColorings (nVertex g) colors)

n1a = Stmnt (1,'A') :: Prop (Int, Char)
n1b = Stmnt (1,'B')
n2a = Stmnt (2,'A')
n2b = Stmnt (2,'B')
n3a = Stmnt (3,'A')
n3b = Stmnt (3,'B')
n4a = Stmnt (4,'A')
n4b = Stmnt (4,'B')

gc =
    (n1a :>: Neg n2a :&: Neg n4a) :&:
    (n1b :>: Neg n2b :&: Neg n4b) :&:

    (n2a :>: Neg n1a :&: Neg n3a) :&:
    (n2b :>: Neg n1b :&: Neg n3b) :&:

    (n3a :>: Neg n2a :&: Neg n4a) :&:
    (n3b :>: Neg n2b :&: Neg n4b) :&:

    (n4a :>: Neg n1a :&: Neg n3a) :&:
    (n4b :>: Neg n1b :&: Neg n3b) :&:

    (n1a :=: Neg n1b) :&:
    (n2a :=: Neg n2b) :&:
    (n3a :=: Neg n3b) :&:
    (n4a :=: Neg n4b)

{-
    Utilities
-}

or :: [Prop a] -> Prop a
or = foldl1 (:|:) 

and :: [Prop a] -> Prop a
and = foldl1 (:&:)

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