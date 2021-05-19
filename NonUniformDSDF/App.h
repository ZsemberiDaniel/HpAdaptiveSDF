#ifndef APP_H
#define APP_H

#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/vao.h>	 //will be replaced
#include <queue>
#include <fstream>
#include <iomanip>

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
#include "Math/PolynomialBases.h"
#include "Octree/octreeGenerator.h"
#include "SDF/SDFHeader.h"
#include <fstream>
#include <glm/gtx/transform.hpp>
#include "Octree/SaveableOctree.h"
#include "SDF/SDFHeatmapVisualizer.h"
#include "constants.h"
#include "Utils/FileSelector.h"
#include "Utils/ErrorStatistics.h"

// #define USE_0th_ORDER

class App 
{

private:
	df::Sample& sam;
	df::NoVao noVao;

	gpuTimer timer;
	PerformanceTest perfTest;

	df::ShaderProgramEditorVF cubeWireProgram = "Cube-Wire-Prog";
	df::ShaderProgramEditorVF meshProgram = "Mesh-Prog";
	df::ShaderProgramEditorVGF flatMeshProgram = "FlatMesh-Prog";
	df::ShaderProgramEditorVF* sdfProgram = nullptr;

	
	void CompilePreprocess();
	void CompileShaders();

	void CalculateOctreeSendToGPU();
	
#ifdef USE_0th_ORDER
	int sdf0thOrderResolution = 128;
	std::unique_ptr<df::Texture3D<float>> sdf0thOrderCurrOctree;
	df::ShaderProgramEditorVF* sdf0thOrderProgram = nullptr;
	bool show0thOrder = false;
	bool show0thOrderShaderEditor = false;
#endif
	
	std::vector<std::shared_ptr<SaveableOctree>> octreeHistory;
	std::shared_ptr<SaveableOctree> currOctree;
	
	void currOctreeSetFromHistory(int historyIndex)
	{
		if (historyIndex < 0 || historyIndex >= octreeHistory.size()) return;

		octreeHistory[historyIndex].swap(currOctree);
#ifdef USE_0th_ORDER
		sdf0thOrderCurrOctree = currOctree->calculateSdf0thOrderTexture(sdf0thOrderResolution);
#endif
		
		CompileShaders();
	}

	
	void DrawOctree(df::ShaderProgramEditorVF& program, const std::shared_ptr<Octree<Cell>::Node>& currentNode, int level = -1);

	glm::ivec3 errorHeatmapSlice;


	struct ProgramState {
		int activeSDFIndex = 1;
		SDFBase* activeSDF() { return sdfs[activeSDFIndex]; }

		int activeApproxTypeIndex = 0;
		PolynomialBase activeApproxType() { return approxTypes[activeApproxTypeIndex]; }

		df::Camera cam;
		bool useLookupTable = false;
		bool enableHeatmapShaderEditor = false;
		bool enableSDFShadersEditor = false;
		bool printOctree = false;
		bool enableGUI = true;
		bool showNormals = false;
		bool drawOctreeGrid = false;
		int drawOctreeLevel = -1; // which level to draw: -1 for all, 0 for first and increasing for lower levels

		// placement
		float SDFScale() { return activeSDF()->worldSize(); }
		glm::vec3 SDFTrans() { return activeSDF()->worldMinPos();  }

		struct RenderSettings {
			glm::vec3 gLightPos = glm::vec3(10, 66, -200);

			// 0 - basic with refine root
			// 1 - relaxed
			// 2 - enhanced
			int sphereTraceType = 0;
			int gAOMaxIters = 5;
			float gAOStepSize = 0.4f;
			float gAOStrength = 0.7f;

			glm::vec3 gAmbient = glm::vec3(0.01f, 0.01f, 0.011f);
			glm::vec3 gDiffuse = glm::vec3(84, 93, 47) / 255.f;
			glm::vec3 gCookRoughness = glm::vec3(31, 31, 31) / 255.f;
			glm::vec3 gCookIOR = glm::vec3(2.0f, 2.0f, 2.0f);

			float epsilonToSurface = 0.002f;
			float smallestStep = 10e-7f;
			float biggestStep = 100.0f;
			float stepMultiplier = 1.0f;
			int maxStep = 40;
			int refineRoot = 2;
		} settings;

		OctreeGenerator::ConstructionParameters constructionParams = {
			glm::vec3(0),
			1.0f,

			3, // maxDegree
			3, //maxLevel
			0.05f // errorThreshold
		};
		bool inTestMode = false;
	} state;

	void SceneCameraRecenter() { state.cam.SetView(glm::vec3(5), glm::vec3(0), glm::vec3(0, 1, 0)); }
	void SceneLightToCamera() { state.settings.gLightPos = state.cam.GetEye(); }
	void PrintCurrentOctree() { currOctree->printOctree(); }
public:
	App(df::Sample&, int perfTestFrameCount = 50, std::shared_ptr<SaveableOctree> octree = nullptr);
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

	void EnterTestMode(glm::vec3 camPos, int traceType, bool useLookup)
	{
		state.cam.SetView(camPos, glm::vec3(0), glm::vec3(0, 1, 0));
		state.enableGUI = false;
		state.inTestMode = true;
		state.settings.sphereTraceType = traceType;
		state.useLookupTable = useLookup;
	}
	void currOctreeSet(std::shared_ptr<SaveableOctree> newCurrOctree)
	{
		if (currOctree != nullptr) octreeHistory.push_back(currOctree);

		currOctree.swap(newCurrOctree);
#ifdef USE_0th_ORDER
		sdf0thOrderCurrOctree = currOctree->calculateSdf0thOrderTexture(sdf0thOrderResolution);
#endif

		CompileShaders();
	}

	PerformanceTest::Results getPerfTestResults() { return perfTest.getResults(); }
	ErrorStatistics::ErrorStatResult getErrorResults() 
	{
		ErrorStatistics::ErrorStatResult errorResults;

		ErrorStatistics stats;
		ErrorStatistics::StatSettings settings = {
			glm::vec3(0), // border
			true, // calcMedian
			true // calcHistogram
		};

		auto ref = currOctree->sdfFunction()->discreteSDFValuesTexture2D();
		auto data = currOctree->calculateSdf0thOrderTexture2D(ERROR_HEATMAP_SIZE);

		errorResults = stats.CalcStatistics(ref, data, settings);

		return errorResults;
	}

	std::string octreeEvaluationToString(bool writeError = true, bool tabular = false)
	{
		if (currOctree == nullptr) return "";

		std::stringstream ss;

		PerformanceTest::Results results = getPerfTestResults();
		if (tabular)
		{
			ss << "Min time\t Avg time\t Max time" << std::endl;
			ss << results.min << "ms\t" << results.avg << "ms\t" << results.max << "ms" << std::endl;
		}
		else
		{
			ss << "Data of last few frames:\n";
			ss << "Render time min.: " << results.min << "ms\n";
			ss << "Render time max.: " << results.max << "ms\n";
			ss << "Render time avg.: " << results.avg << "ms\n\n";
		}
		if (!writeError) return ss.str();

		ErrorStatistics::ErrorStatResult errorResults = getErrorResults();

		if (tabular)
		{
			ss << "Mean err\tMin err\tMax err\tMedian err\tSd err\tL2 err" << std::endl;
			ss  << errorResults.mean << "\t"
				<< errorResults.min << "\t"
				<< errorResults.max << "\t"
				<< errorResults.median << "\t"
				<< errorResults.sd << "\t"
				<< errorResults.l2 << std::endl;
		}
		else
		{
			ss << "Error:\n";
			ss << "Mean:\t" << errorResults.mean << "\n"
				<< "Min:\t" << errorResults.min << "\n"
				<< "Max:\t" << errorResults.max << "\n"
				<< "Median:\t" << errorResults.median << "\n"
				<< "Sd:\t" << errorResults.sd << "\n"
				<< "L2:\t" << errorResults.l2 << "\n";
		}

		return ss.str();
	}
};

#endif