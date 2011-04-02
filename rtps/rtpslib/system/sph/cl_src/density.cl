#ifndef _NEIGHBORS_CL_
#define _NEIGHBORS_CL_


/* TO BE INCLUDED FROM OTHER FILES. In OpenCL, I believe that all device code
// must be in the same file as the kernel using it. 
*/

/*----------------------------------------------------------------------*/

#include "cl_macros.h"
#include "cl_structs.h"
//Contains all of the Smoothing Kernels for SPH
#include "cl_kernels.h"


//----------------------------------------------------------------------
inline void ForNeighbor(__global float4*  vars_sorted,
                        PointData* pt,
                        uint index_i,
                        uint index_j,
                        float4 position_i,
                        __constant struct GridParams* gp,
                        __constant struct SPHParams* sphp
                        DEBUG_ARGS
                       )
{
    int num = sphp->num;

    // get the particle info (in the current grid) to test against
    float4 position_j = pos(index_j); 
    float4 r = (position_i - position_j); 
    r.w = 0.f; // I stored density in 4th component
    // |r|
    float rlen = length(r);

    // is this particle within cutoff?
    if (rlen <= sphp->smoothing_distance)
    {
        // return density.x for single neighbor
        //float Wij = sphp->wpoly6_coef * Wpoly6(r, sphp->smoothing_distance, sphp);
        float Wij = Wpoly6(r, sphp->smoothing_distance, sphp);

        pt->density.x += sphp->mass*Wij;
        //pt->density.x += sphp->mass*Wij;
    }
}

//Contains Iterate...Cells methods and ZeroPoint
#include "cl_neighbors.h"

//--------------------------------------------------------------
// compute forces on particles

__kernel void density_update(
                       __global float4* vars_sorted,
                       __global int*    cell_indexes_start,
                       __global int*    cell_indexes_end,
                       __constant struct GridParams* gp,
                       __constant struct SPHParams* sphp 
                       DEBUG_ARGS
                       )
{
    // particle index
    int nb_vars = sphp->nb_vars;
    int num = sphp->num;
    //int numParticles = get_global_size(0);
    //int num = get_global_size(0);

    int index = get_global_id(0);
    if (index >= num) return;

#if 1
    float4 position_i = pos(index);

    //debuging
    clf[index] = (float4)(99,0,0,0);
    //cli[index].w = 0;

    // Do calculations on particles in neighboring cells
    PointData pt;
    zeroPoint(&pt);

    IterateParticlesInNearbyCells(vars_sorted, &pt, num, index, position_i, cell_indexes_start, cell_indexes_end, gp,/* fp,*/ sphp DEBUG_ARGV);
    density(index) = sphp->wpoly6_coef * pt.density.x;
    /*
    clf[index].x = pt.density.x * sphp->wpoly6_coef;
    clf[index].y = pt.density.y;
    clf[index].z = sphp->smoothing_distance;
    clf[index].w = sphp->mass;
    */
    clf[index].w = density(index);
#endif
}

/*-------------------------------------------------------------- */
#endif
