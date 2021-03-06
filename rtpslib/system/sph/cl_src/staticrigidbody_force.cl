/****************************************************************************************
* Real-Time Particle System - An OpenCL based Particle system developed to run on modern GPUs. Includes SPH fluid simulations.
* version 1.0, September 14th 2011
* 
* Copyright (C) 2011 Ian Johnson, Andrew Young, Gordon Erlebacher, Myrna Merced, Evan Bollig
* 
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
****************************************************************************************/


#ifndef _NEIGHBORS_CL_
#define _NEIGHBORS_CL_


//These are passed along through cl_neighbors.h
//only used inside ForNeighbor defined in this file
#define ARGS __global float* density,  __global float4* pos, __global float4* vel, __global float4* force, __global float* mass, __global float4* pos_j,float16 rbParams
#define ARGV density, pos, vel, force, mass, pos_j,rbParams


/*----------------------------------------------------------------------*/

#include "cl_sph_macros.h"
#include "cl_sph_structs.h"
//Contains all of the Smoothing Kernels for SPH
#include "cl_sph_kernels.h"


//----------------------------------------------------------------------
inline void ForNeighbor(//__global float4*  vars_sorted,
                        ARGS,
                        PointData* pt,
                        uint index_i,
                        uint index_j,
                        float4 position_i,
                        __constant struct GridParams* gp,
                        __constant struct SPHParams* sphp
                        DEBUG_ARGS
                       )
{
    // get the particle info (in the current grid) to test against
    float4 position_j = pos_j[index_j] * sphp[0].simulation_scale; 
    float4 r = (position_j - position_i); 
    r.w = 0.f; // I stored density in 4th component
    // |r|
    float rlen = length(r);

    // is this particle within cutoff?
    if (rlen <= 2* sphp[0].smoothing_distance)
    {
        // avoid divide by 0 in Wspiky_dr
        rlen = max(rlen, sphp[0].EPSILON);
        float4 norm = r/rlen;
        //need to have a better way of handling stiffness..
        //float massnorm=((mass[index_i]*mass[index_i])/(mass[index_i]+mass[index_i]));
        float massnorm=((sphp[0].mass*sphp[0].mass)/(sphp[0].mass+sphp[0].mass));
        //float stiff = rbParams.s0*massnorm;
        float stiff = rbParams.s0*sphp[0].mass;
        float4 springForce = -stiff*(2.*sphp[0].smoothing_distance-rlen)*(norm);

        float4 relvel = -vel[index_i];

        float4 normVel =dot(relvel,norm)*norm;
        float4 tanVel = relvel-normVel;
        float4 dampeningForce = rbParams.s1*sqrt(stiff*massnorm)*(normVel);
        float4 normalForce=(springForce+dampeningForce); 
        
        //Fixme: we need to stop the particles tangential velocity. How should I accomplish this?
        //float4 tanForce = -(mass[index_i]*tanVel);
        /*
        relvel.w=0.0;
        normalForce.w=0.0;
        //Use Gram Schmidt process to find tangential velocity to the particle
        float4 frictionalForce=0.0f;
        if(length(tangVel)>rbParams.s2)
            frictionalForce = -rbParams.s3*length(normalForce)*(normalize(tangVel));
        else
            frictionalForce = -rbParams.s4*tangVel;
        pt[0].force += (normalForce+frictionalForce);*/

        float dWijlapl = Wvisc_lapl(rlen, sphp[0].smoothing_distance, sphp);
        //float4 visc = (tanVel) * dWijlapl * 1.0f;
        float4 visc = (relvel) * dWijlapl * 1.0f;
        pt[0].viscosity+= visc;//(float)iej;

        pt[0].force+=normalForce;
	//pt[0].force+=normalForce-tanForce;
    }
}

//Contains Iterate...Cells methods and ZeroPoint
#include "cl_sph_neighbors.h"

//--------------------------------------------------------------
// compute forces on particles

__kernel void force_update(
                       //__global float4* vars_sorted,
                       ARGS,
                       __global int*    cell_indexes_start,
                       __global int*    cell_indexes_end,
                       __constant struct GridParams* gp,
                       __constant struct SPHParams* sphp 
                       DEBUG_ARGS
                       )
{
    // particle index
    int num = sphp[0].num;
    //int numParticles = get_global_size(0);
    //int num = get_global_size(0);

    int index = get_global_id(0);
    if (index >= num) return;

    float4 position_i = pos[index] * sphp[0].simulation_scale;

    //debuging
    clf[index] = (float4)(99,0,0,0);
    //cli[index].w = 0;

    // Do calculations on particles in neighboring cells
    PointData pt;
    zeroPoint(&pt);

    //IterateParticlesInNearbyCells(vars_sorted, &pt, num, index, position_i, cell_indexes_start, cell_indexes_end, gp,/* fp,*/ sphp DEBUG_ARGV);
    IterateParticlesInNearbyCells(ARGV, &pt, num, index, position_i, cell_indexes_start, cell_indexes_end, gp,/* fp,*/ sphp DEBUG_ARGV);
    force[index] += pt.force/sphp[0].mass;
    //for the viscosity we square the density under the assumption that the static boundary has equal density as the surrounding fluid.
    force[index] -= sphp[0].mass * (1.0/(density[index]*density[index]))*(sphp[0].viscosity * sphp[0].wvisc_dd_coef * pt.viscosity); 
    clf[index].xyz = pt.force.xyz;
}

/*-------------------------------------------------------------- */
#endif

