//
//  anf.h
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include "anf_rpn.h"

ANF* anf (Proposition *p);
ANF* anf_it (Proposition *p);
void destroy_anf (ANF *p);
