//
//  anf_array.h
//  sat
//
//  Created by David Lilue on 23/06/2018.
//

#include "anf_array_lib.h"

ANF_Array* xor_anf_array (ANF_Array *p, ANF_Array *q);
ANF_Array* and_anf_array (ANF_Array *p, ANF_Array *q);
void merge_sort_anf (ANF_Array *p);
ANF_Array* map_anf_bs (ANF_BitString *bs, ANF_Array *p);
ANF_BitString* and_bs (ANF_BitString *x, ANF_BitString *y);
ANF_Array* copy_anf_array (ANF_Array *p);
ANF_Array* anf_array (Proposition *p);
