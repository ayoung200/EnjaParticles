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


//#ifdef RTPS_COMMON_H
//#define RTPS_COMMON_H

#if !(defined RTPS_EXPORT)
#ifdef WIN32
    #if (defined rtps_EXPORTS)
        #define RTPS_EXPORT __declspec(dllexport)
		#pragma warning (disable : 4251)
    #elif (defined rtps_DLL)
        #define RTPS_EXPORT __declspec(dllimport)
	#else
		#define RTPS_EXPORT
	#endif
#else
    #define RTPS_EXPORT
#endif
#endif

//#endif
