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
#include "Math/GaussPolynomialGenerator.h"
#include "Math/LSQPolyGenerator.h"
#include "Math/PolynomialGenerator.h"
#include "Math/Polynomial.h"
#include "Octree/octreeGenerator.h"
#include "SDF/SDFHeader.h"
#include <fstream>
#include <glm/gtx/transform.hpp>


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

	gpuTimer timer;
	PerformanceTest perfTest;

	df::ShaderProgramEditorVF cubeWireProgram = "Cube-Wire-Prog";
	df::ShaderProgramEditorVF meshProgram = "Mesh-Prog";
	df::ShaderProgramEditorVGF flatMeshProgram = "FlatMesh-Prog";

	df::ShaderProgramEditorVF sdfProgram = "SDF-Prog"; // temp
	struct Desc {
		// TODO: move to SD field attributes, border can be removed
		glm::vec3 SDFCorner = glm::vec3(0);
		glm::vec3 SDFSize = glm::vec3(1);
		glm::vec3 SDFBorder = glm::vec3(0.01f);
	}desc; // temp

	void CompilePreprocess();
	void CompileShaders();

	void CalculateOctreeSendToGPU();
	
	GaussPolynomialGenerator gaussPolyGenerator;
	LSQPolyGenerator lsqGenerator;

	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> branchSSBO;
	GLuint branchSSBOPoi = 0, leavesSSBOPoi = 0;
	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> leavesSSBO;

	// TODO: make it unque pointer
	Octree<Cell> octree;
	int octreeBranchCount;
	void DrawOctree(df::ShaderProgramEditorVF& program, const Octree<Cell>::Node* currentNode, int level = -1);




	struct PolynomialBase
	{
		int id;
		std::string name;
		std::string shaderEvalFunctionName;
		std::function<void(App*)> cpuConstruction;
	};
	struct ProgramState {
		std::vector<SDFBase*> sdfs = {
			new SDFTorus(),
			new SDFSphere(),
			new SDFPlane()
		};
		int activeSDFIndex = 0;

		std::vector<PolynomialBase> approxTypes = {
			PolynomialBase {
				0,
				"Gauss Quadrature - normalized Legendre",
				"evalPolynom_normLagrange",
				 [](App* app)
				 {
					 OctreeGenerator::constructField<GaussPolynomialGenerator>(
						 app->octree, 
						 app->gaussPolyGenerator, 
						 app->state.octreeConstructionParams, 
						 *app->currentSdf());
				 }
			},
			PolynomialBase {
				1,
				"LSQ - Legendre",
				"evalPolynom_lagrange",
				 [](App* app)
				 {
					 OctreeGenerator::constructField<LSQPolyGenerator>(
						 app->octree, 
						 app->lsqGenerator, 
						 app->state.octreeConstructionParams, 
						 *app->currentSdf());
				 }
			}
		};
		int activeApproxTypeIndex = 1;
		PolynomialBase activeApproxType() { return approxTypes[activeApproxTypeIndex]; }

		df::Camera cam;
		bool enableGUI = true;
		bool drawOctreeGrid = false;
		int drawOctreeLevel = -1; // which level to draw: -1 for all, 0 for first and increasing for lower levels

		// placement
		float SDFScale = 1.0f;
		glm::vec3 SDFTrans = glm::vec3(0);

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
			bool refineRoot = true;
		} settings;

		OctreeGenerator::ConstructionParameters octreeConstructionParams = {
			glm::vec3(0), // min corner pos in world DO NOT CHANGE, changed dynamically
			1.0f, // size in world DO NOT CHANGE, changed dynamically

			3, // maxDegree
			2, // maxLevel
			0.005f // errorThreshold
		};
	} state;

	SDFBase* currentSdf() { return state.sdfs[state.activeSDFIndex]; }

	void SceneCameraRecenter() { state.cam.SetView(glm::vec3(5), glm::vec3(0), glm::vec3(0, 1, 0)); }
	void SceneLightToCamera() { state.settings.gLightPos = state.cam.GetEye(); }
};
