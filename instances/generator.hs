import System.Environment
import System.Exit

main = readFile "myciel4.col" >>= parse 23

-- colors = 2

setNth :: [a] -> Int -> a -> [a]
setNth xs n newElement = take n xs ++ [newElement] ++ drop (n + 1) xs

edge :: String -> (Int, Int)
edge s = let (_:v1:v2:_) = words s in (read v1, read v2) :: (Int, Int)

parse :: Int -> String -> IO [[Int]]
parse n file = return $ adjList (replicate n []) $ dropWhile (\w -> (head w) /= 'e') $ lines file

adjList :: [[Int]] -> [String] -> [[Int]]
adjList acc [] = map reverse acc
adjList acc (x:xs) =
    let
        (v1, v2) = edge x
        v1_edges = acc !! (v1 - 1)
        v2_edges = acc !! (v2 - 1)
        new_acc = setNth (setNth acc (v1 - 1) (v2:v1_edges)) (v2 - 1) (v1:v2_edges)
    in
        adjList new_acc xs