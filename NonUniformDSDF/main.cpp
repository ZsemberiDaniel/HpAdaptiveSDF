#define NOMINMAX
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <string>
#include <chrono>

#include "EvaluationSystem/EvaluationSystem.h"
#include "App.h"

// #define TEST

/* File desc: 
 * path_to_save_file   camera_pos    trace_type (0 - default, 1 - relaxed, 2 - enhanced)    useLookup (0 or 1)
*/
int main(int argc, char* args[])
{

#ifdef TEST
	Evaluation::EvaluationSystem::evaluate("constantDegTests.txt", "output.txt", 100);
#else

	auto param = df::Sample::FLAGS::IMGUI_VIEWPORTS | df::Sample::FLAGS::WINDOW_RESIZABLE | 
		(df::IS_THIS_DEBUG ? df::Sample::FLAGS::RENDERDOC : df::Sample::FLAGS::NONE);
	df::Sample sam("Non-uniform DSDF", 720, 480, param);
//	df::Camera cam;								// Implements a camera event class with handles
//	sam.AddHandlerClass(cam, 5);				// class callbacks will be called to change its state
	sam.AddHandlerClass<df::ImGuiHandler>(10);	// static handle functions only
	
	App app(sam);
	sam.AddHandlerClass(app, 5);

	sam.Run([&](float deltaTime) //delta time in ms
		{
			/*std::ofstream times("C:\\Coding\\non-uniform-dsdf\\Saves\\times.txt");

			if (!times.is_open())
			{
				std::cerr << "Error while opening generation times txt" << std::endl;
				sam.Quit();
				return;
			}

			for (int maxDeg = 0; maxDeg < 5; maxDeg++) 
			{
				OctreeGenerator::ConstructionParameters params = {
					glm::vec3(0.0f),
					1.0f,
					maxDeg,
					5,
					0.0000005f,
					true,
					false,
					10,
					maxDeg
				};
				for (int i = 5; i < sdfs.size(); i++)
				{
					for (int k = 0; k < 5; k++)
					{
						if (k >= 3 && maxDeg == 0) continue;
						if (k == 2) continue;

						std::string path = "C:\\Coding\\non-uniform-dsdf\\Saves\\" + sdfs[i]->name() + approxTypes[k].shortUniqueName + std::to_string(maxDeg);
						try {
							std::cout << "Starting " << path << std::endl;
							auto timeStart = std::chrono::high_resolution_clock::now();
							auto octree = SaveableOctree(i, k, params);
							auto interval = std::chrono::high_resolution_clock::now() - timeStart;

							times << path << std::endl << std::to_string(interval.count()) << std::endl;
							times.flush();

							octree.saveTo(std::wstring(path.begin(), path.end()));
							std::cout << "Done " << path << std::endl;
						}
						catch (std::exception e)
						{
							std::cerr << "Could not finish " << path << std::endl;
							std::cerr << e.what() << std::endl;
						}
					}
				}
			}
			sam.Quit();
			return;*/

//			cam.Update();
			app.Update();
			app.Render();
			app.RenderGUI();

			GL_CHECK;
		}
	);
#endif
	return 0;
}
