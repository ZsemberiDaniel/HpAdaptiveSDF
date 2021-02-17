#pragma once

#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/buffer.h> //will be replaced
#include <Dragonfly/detail/vao.h>	 //will be replaced
#include <queue>

#include "Utils/gpuTimer.h"
#include "Utils/PerformanceTest.h"
#include "Octree/octree.h"
#include "vector3d.h"
#include "Structures.h"

#include <vector>
#include <numeric>
#include <cmath>
#include <limits>

class App {
public:
	App(df::Sample&);
	~App();

	void Update();
	void Render();
	void RenderGUI();

//	bool HandleKeyUp(const SDL_KeyboardEvent&);
//	bool HandleKeyDown(const SDL_KeyboardEvent&);
//	bool HandleMouseDown(const SDL_MouseButtonEvent&);
//	bool HandleMouseMotion(const SDL_MouseMotionEvent&);
//	bool HandleMouseUp(const SDL_MouseButtonEvent&);
//	bool HandleMouseWheel(const SDL_MouseWheelEvent&);
//	void HandleResize(int, int);

private:
	df::Sample& sam;
	df::NoVao noVao;

	struct ProgramState {
//		std::vector<SDF?> SDFs;
		int activeSDFIndex = 0;
		df::Camera cam;
		bool enableGUI = true;

		// placement
		float SDFScale = 1.0f;
		glm::vec3 SDFTrans = glm::vec3(0.0f, 0.0f, 0.0f);
	} state;

	gpuTimer timer;
	PerformanceTest perfTest;

	df::ShaderProgramEditorVF cubeWireProgram = "Cube-Wire-Prog";
	df::ShaderProgramEditorVF meshProgram = "Mesh-Prog";
	df::ShaderProgramEditorVGF flatMeshProgram = "FlatMesh-Prog";
	df::ShaderProgramEditorVF sdfProgram = "SDF-Prog";

	void InitShaders();

	Octree<Cell> octree;
	void constructField(Grid& grid, int maxDegree = 4, int maxLevel = 2, float errorThreshold = 1.0f);
	void DrawOctree(df::ShaderProgramEditorVF& program, const Octree<Cell>::Node* currentNode);
};