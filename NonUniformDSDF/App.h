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
#include <iomanip>

class App {
public:
	App(df::Sample&);
	~App();

	void Update();
	void Render();
	void RenderGUI();

//	bool HandleKeyUp(const SDL_KeyboardEvent&);
	bool HandleKeyDown(const SDL_KeyboardEvent&);
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
		glm::vec3 SDFTrans = glm::vec3(0);

		df::detail::ClearF<0> clear = df::Clear(0.125f, 0.25f, 0.5f);

		struct RenderSettings {
			glm::vec3 gLightPos = glm::vec3(10, 66, -200);

			int gAOMaxIters = 5;
			float gAOStepSize = 0.4f;
			float gAOStrength = 0.7f;

			glm::vec3 gAmbient = glm::vec3(0.01f, 0.01f, 0.011f);
			glm::vec3 gDiffuse = glm::vec3(84, 93, 47) / 255.f;
			glm::vec3 gCookRoughness = glm::vec3(31, 31, 31) / 255.f;
			glm::vec3 gCookIOR = glm::vec3(2.0f, 2.0f, 2.0f);

			int maxStep = 40;
			bool refineRoot = false;
		} settings;
	} state;

	gpuTimer timer;
	PerformanceTest perfTest;

	df::ShaderProgramEditorVF cubeWireProgram = "Cube-Wire-Prog";
	df::ShaderProgramEditorVF meshProgram = "Mesh-Prog";
	df::ShaderProgramEditorVGF flatMeshProgram = "FlatMesh-Prog";

	df::ShaderProgramEditorVF sdfProgram = "SDF-Prog"; // temp
	struct Desc {
		glm::vec3 SDFCorner = glm::vec3(0.2, -1, -1.6);
		glm::vec3 SDFSize = glm::vec3(2);
		glm::vec3 SDFBorder = glm::vec3(0.01f);
	}desc; // temp

	void InitShaders();

	Octree<Cell> octree;
	void constructField(Grid& grid, int maxDegree = 4, int maxLevel = 4, float errorThreshold = 0.00001f);
	void DrawOctree(df::ShaderProgramEditorVF& program, const Octree<Cell>::Node* currentNode, int level = -1);
};