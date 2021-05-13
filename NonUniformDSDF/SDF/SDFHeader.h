#ifndef SDFHEADER_H
#define SDFHEADER_H

#include "SDFBase.h"
#include "SDFPrimitives.h"
#include "../SDFGPUandCPU/SDF2Torus.h"
#include "../SDFGPUandCPU/SDF2Sphere.h"
#include "../SDFGPUandCPU/SDF2Plane.h"
#include "../SDFGPUandCPU/SDF2Model1.h"
#include "../SDFGPUandCPU/SDF2Model2.h"
#include "../SDFGPUandCPU/SDF2Model3.h"
#include "../SDFGPUandCPU/SDF2Model4.h"
#include "../SDFGPUandCPU/SDF2Model5.h"
#include "../SDFGPUandCPU/SDF2Model6.h"
#include "../SDFGPUandCPU/SDF2Model7.h"
#include "../SDFGPUandCPU/SDF2Cube.h"

inline std::vector<SDFBase*> sdfs = {
	new SDF2Torus(),
	new SDF2Sphere(),
	new SDF2Plane(),
	new SDF2Model1(),
	new SDF2Cube(),
	new SDF2Model2(),
	new SDF2Model3(),
	new SDF2Model4(),
	new SDF2Model5(),
	new SDF2Model6(),
	new SDF2Model7()
};
#endif