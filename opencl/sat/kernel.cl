//
//  kernel.cl
//  sat_solver
//
//  Created by David Lilue on 05/06/2018.
//

// Kernel block.
kernel void and_bs_gpu(global char *bs_x,
                       global char *bs_y,
                       global char *bs_out)
{
    size_t i = get_global_id(0);
    bs_out[i] = bs_x[i] | bs_y[i];
}
