#ifndef SDFHEADER_H
#define SDFHEADER_H

#include "SDFBase.h"
#include "SDFPrimitives.h"
#include "../SDFGPUandCPU/SDF2Torus.h"
#include "../SDFGPUandCPU/SDF2Sphere.h"
#include "../SDFGPUandCPU/SDF2Plane.h"
#include "../SDFGPUandCPU/SDF2Model1.h"
#include "../SDFGPUandCPU/SDF2Cube.h"

inline std::vector<SDFBase*> sdfs = {
	new SDF2Torus(),
	new SDF2Sphere(),
	new SDF2Plane(),
	new SDF2Model1(),
	new SDF2Cube()
};
#endif