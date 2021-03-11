#ifndef SDFHEADER_H
#define SDFHEADER_H

#include "SDFBase.h"
#include "SDFPrimitives.h"
#include "SDFTorus.h"
#include "SDFSphere.h"
#include "SDFPlane.h"
#include "SDFModel1.h"
#include "SDFCube.h"

inline std::vector<SDFBase*> sdfs = {
	new SDFTorus(),
	new SDFSphere(),
	new SDFPlane(),
	new SDFModel1(),
	new SDFCube()
};
#endif