#define NOMINMAX
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <string>
#include <chrono>
#include <filesystem>
#include <windows.h> 

#include "EvaluationSystem/EvaluationSystem.h"
#include "App.h"

// #define TEST
//#define GENERATE

void generateSaves(int maxDepth = 5, int maxDeg = 5, float errorThreshold = 0.00005f, int gpuWorkerCount = 20, bool writeLookupTests = false)
{
	auto param = df::Sample::FLAGS::IMGUI_VIEWPORTS | df::Sample::FLAGS::WINDOW_RESIZABLE |
		(df::IS_THIS_DEBUG ? df::Sample::FLAGS::RENDERDOC : df::Sample::FLAGS::NONE);
	df::Sample sam("Non-uniform DSDF", 720, 480, param);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::cout << "_,.-'~'-.,__,.-'~'-., Octree Generator ,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_" << std::endl;
	std::cout << "Multiple types of octrees will be generated, each saved in non-uniform-dsdf/Saves folder, if it does not exist please create it!" << std::endl;
	std::cout << "Generation times will be saved to times.txt." << std::endl;
	std::cout << "File that includes test for all generated saves is test.txt." << std::endl;
	std::cout << "_,.-'~'-.,__,.-'~'-., ________________ ,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_" << std::endl;
	std::cout << "GPU improvements at once: " << gpuWorkerCount << std::endl;
	std::cout << "Error threshold: " << errorThreshold << std::endl;
	std::cout << "Max deg: " << maxDeg << std::endl;
	std::cout << "Max depth: " << maxDepth << std::endl;
	std::cout << "_,.-'~'-.,__,.-'~'-., Octree Generator ,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_" << std::endl << std::endl;
	std::cout << "PRESS ENTER TO CONTINUE" << std::endl << std::endl;
	std::getchar();

	namespace fs = std::filesystem;
	std::ofstream times("times.txt");
	std::ofstream testFile("test.txt");

	if (!times.is_open())
	{
		std::cerr << "Error while opening times.txt" << std::endl;
		return;
	}

	if (!testFile.is_open())
	{
		std::cerr << "Error while opening test.txt" << std::endl;
		return;
	}

	auto writeToTestFile = [&testFile, writeLookupTests](std::string& inProjectPath)
	{
		// do not use lookup, different sphere traces
		testFile << "..\\" << inProjectPath << " 3 3 3 2 0" << std::endl;
		testFile << "..\\" << inProjectPath << " 3 3 3 1 0" << std::endl;
		testFile << "..\\" << inProjectPath << " 3 3 3 0 0" << std::endl;
		if (writeLookupTests)
		{
			// use lookup, different sphere traces
			testFile << "..\\" << inProjectPath << " 3 3 3 2 1" << std::endl;
			testFile << "..\\" << inProjectPath << " 3 3 3 1 1" << std::endl;
			testFile << "..\\" << inProjectPath << " 3 3 3 0 1" << std::endl;
		}

		testFile.flush();
	};

	auto saveSDF = [&times, &writeToTestFile](std::string inProjectPath, int sdfIndex, int approxIndex, OctreeGenerator::ConstructionParameters& params)
	{
		std::string path = fs::current_path().parent_path().string() + "\\" + inProjectPath;
		try {
			std::cout << "Starting " << path << std::endl;
			auto timeStart = std::chrono::high_resolution_clock::now();
			auto octree = SaveableOctree(sdfIndex, approxIndex, params);
			auto interval = std::chrono::high_resolution_clock::now() - timeStart;

			times << path << std::endl << std::to_string((interval.count() / 1000000000.0f)) << " s" << std::endl;
			times.flush();

			octree.saveTo(std::wstring(path.begin(), path.end()));
			std::cout << "Done in " << (interval.count() / 1000000000.0f) << " secs" << std::endl << std::endl;

			writeToTestFile(inProjectPath);
		}
		catch (std::exception e)
		{
			std::cerr << "Could not finish " << path << std::endl;
			std::cerr << e.what() << std::endl << std::endl;
		}
	};

	fs::create_directories(fs::current_path().parent_path().string() + "\\Saves\\ConstDegs");
	std::cout << "Generating octrees with constant degrees (only h improvement)" << std::endl;
	for (int currMaxDeg = 0; currMaxDeg < maxDeg; currMaxDeg++)
	{
		OctreeGenerator::ConstructionParameters params = {
			glm::vec3(0.0f),
			1.0f,
			currMaxDeg,
			maxDepth,
			errorThreshold,
			true,
			false,
			gpuWorkerCount,
			currMaxDeg
		};
		for (int sdfIndex = 3; sdfIndex < sdfs.size(); sdfIndex++)
		{
			if (sdfIndex == 4) continue;
			for (int approxIndex = 0; approxIndex < 5; approxIndex++)
			{
				if (approxIndex >= 3 && currMaxDeg == 0) continue;
				if (approxIndex == 2) continue;

				std::string inProjectPath = "Saves\\ConstDegs\\" + sdfs[sdfIndex]->name() + approxTypes[approxIndex].shortUniqueName + std::to_string(maxDeg);
				
				saveSDF(inProjectPath, sdfIndex, approxIndex, params);
			}
		}
	}

	{
		OctreeGenerator::ConstructionParameters params = {
			glm::vec3(0.0f),
			1.0f,
			maxDeg,
			maxDepth,
			errorThreshold,
			true,
			true,
			gpuWorkerCount,
			maxDeg
		};
		std::cout << "_,.-'~'-.,__,.-'~'-., Octree Generator ,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_" << std::endl;
		std::cout << "Generating octrees with no restrictions" << std::endl << std::endl;
		fs::create_directories(fs::current_path().parent_path().string() + "\\Saves\\NoRestriction");
		for (int sdfIndex = 3; sdfIndex < sdfs.size(); sdfIndex++)
		{
			if (sdfIndex == 4) continue; 
			for (int approxIndex = 0; approxIndex < 5; approxIndex++)
			{
				std::string inProjectPath = "Saves\\NoRestriction\\" + sdfs[sdfIndex]->name() + approxTypes[approxIndex].shortUniqueName;

				saveSDF(inProjectPath, sdfIndex, approxIndex, params);
			}
		}
	}
}

int main(int argc, char* args[])
{
#ifdef GENERATE
	//maxDepth, maxDeg, errorThreshold, gpuWorkerCount, writeLookupTests
	generateSaves(2, 2, 0.005f, 20, false);
	return 0;
#endif
#ifdef TEST
	Evaluation::EvaluationSystem::evaluate("test.txt", "output.txt", 100);
	return 0;
#else

	auto param = df::Sample::FLAGS::IMGUI_VIEWPORTS | df::Sample::FLAGS::WINDOW_RESIZABLE | 
		(df::IS_THIS_DEBUG ? df::Sample::FLAGS::RENDERDOC : df::Sample::FLAGS::NONE);
	df::Sample sam("Non-uniform DSDF", 720, 480, param);
	sam.AddHandlerClass<df::ImGuiHandler>(10);	// static handle functions only
	
	App app(sam);
	sam.AddHandlerClass(app, 5);

	sam.Run([&](float deltaTime) //delta time in ms
		{
			app.Update();
			app.Render();
			app.RenderGUI();

			GL_CHECK;
		}
	);
#endif
	return 0;
}
