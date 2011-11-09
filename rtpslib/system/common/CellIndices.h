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


#ifndef RTPS_CELLINDICES_H_INCLUDED
#define RTPS_CELLINDICES_H_INCLUDED

//defines a few handy utility functions
#include <util.h>
//defines useful structs like float3 and float4
#include <structs.h>
//OpenCL API
#include <CLL.h>
#include <Buffer.h>
#include <Kernel.h>
#include <Domain.h>
#include <timer_eb.h>


namespace rtps
{
    class CellIndices 
    {
        public:
            CellIndices() { cli = NULL; timer = NULL; };
            CellIndices(std::string path, CL* cli, EB::Timer* timer);
            int execute(int num,
                    Buffer<unsigned int>& hashes,
                    Buffer<unsigned int>& indices,
                    Buffer<unsigned int>& ci_start,
                    Buffer<unsigned int>& ci_stop,
                    //params
                    //Buffer<SPHParams>& sphp,
                    Buffer<GridParams>& gp,
                    int nb_cells,
                    //debug params
                    Buffer<float4>& clf_debug,
                    Buffer<int4>& cli_debug);

        private:
            CL* cli;
            Kernel k_cellindices;
            EB::Timer* timer;
    };
}

#endif
