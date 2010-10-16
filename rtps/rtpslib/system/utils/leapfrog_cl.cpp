
#include "cl_macros.h"
#include "cl_structures.h"

 
__kernel void ge_leapfrog(
		__global int* sort_indices,  
		__global float4* vars_unsorted, 
		__global float4* vars_sorted, 
		// should not be required since part of vars_unsorted
		__global float4* positions,  // for VBO 
		__constant struct SPHParams* params, 
		float dt)
{
    unsigned int i = get_global_id(0);
	int num = get_global_size(0); // for access functions in cl_macros.h

    float4 p = pos(i);
    float4 v = vel(i);
    float4 f = force(i);

    //external force is gravity
    f.z += -9.8f;
	f.w = 0.f;

	// REMOVE FOR DEBUGGING
	// THIS IS REALLY A FORCE, NO?
    float speed = length(f);
    if(speed > 600.0f) //velocity limit, need to pass in as struct
    {
        f *= 600.0f/speed;
    }

	float4 vnext = v + h*f;
	//vnext += 0.5f * xsph[i]; // should be param XSPH factor
    p += dt * vnext;
    p.w = 1.0f; //just in case
	veleval(i) = 0.5f*(v+vnext);


	uint originalIndex = sort_indices[i];

	// writeback to unsorted buffer
	float dens = density(i);
	p /= params->simulation_scale;
	unsorted_pos(originalIndex) = (float4)(p.xyz, dens);
	unsorted_vel(originalIndex) = v;
	unsorted_density(originalIndex) = density(i); // FOR DEBUGGING ONLY
	unsorted_force(originalIndex) = f; // FOR DEBUGGING ONLY
	//positions[originalIndex] = (float4)(p.xyz, dens);  // for plotting
	positions[originalIndex] = (float4)(p.xyz, 1.);  // for plotting
}

