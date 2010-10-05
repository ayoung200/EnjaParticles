
#include "cl_macros.h"
#include "cl_structures.h"

        
 
__kernel void ge_euler(
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
    f.z += -9.8f * 0.707;
	f.x += -9.8f * 0.707;

	// REMOVE FOR DEBUGGING
    float speed = length(f);
    if(speed > 600.0f) //velocity limit, need to pass in as struct
    {
        f *= 600.0f/speed;
    }

    v += dt*f;  //    / params->simulation_scale;
    p += dt*v;
    p.w = 1.0f; //just in case

	// REMOVE AFTER DEBUGGED
	#if 0
    vel(i) = v;
    pos(i) = p;
	force(i) = f; // ONLY FOR DEBUGGING
	#endif

        uint originalIndex = sort_indices[i];

        // writeback to unsorted buffer
		unsorted_pos(originalIndex) = p;
		unsorted_vel(originalIndex) = v;
		unsorted_density(originalIndex) = density(i); // FOR DEBUGGING ONLY
		unsorted_force(originalIndex) = f; // FOR DEBUGGING ONLY
		positions[originalIndex] = p; 
}

