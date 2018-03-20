module SAT where

import Structs.Essentials.Prop
import Structs.CNF
import Structs.BDD
import Tseytin

p = (Statement 'P')
q = (Statement 'Q')
r = (Statement 'R')

modus_ponen = Then (And (Then p q) p) q