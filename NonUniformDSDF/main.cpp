#include <Dragonfly/editor.h>		 //inlcludes most features
#include <string>

#include "App.h"

#define TEST

/* File desc: path_to_save_file   camera_pos    trace_type (0 - default, 1 - relaxed, 2 - enhanced)
*/
void runTests(const std::string& testFilePath, const std::string& outPath, int frameCount)
{
	std::ifstream istream(testFilePath);
	if (!istream.is_open())
	{
		std::cerr << "Could not open test file at " << testFilePath << std::endl;
		return;
	}

	std::ofstream ostream(outPath);
	if (!ostream.is_open())
	{
		std::cerr << "Could not open output test file at " << outPath << std::endl;
		return;
	}

	std::string pathToSaveFile;
	glm::vec3 camPos;
	int traceType;
	while (istream >> pathToSaveFile >> camPos.x >> camPos.y >> camPos.z >> traceType)
	{
		auto param = df::Sample::FLAGS::WINDOW_BORDERLESS;
		df::Sample sam("Non-uniform DSDF TESTS", 1920, 1080, param);

		sam.AddHandlerClass<df::ImGuiHandler>(10);
		App app(sam, frameCount, SaveableOctree::loadFrom(std::wstring(pathToSaveFile.begin(), pathToSaveFile.end())));
		sam.AddHandlerClass(app, 5);

		app.EnterTestMode(camPos, traceType);

		int count = 0;
		sam.Run([&](float deltaTime) //delta time in ms
			{
				app.Update();
				app.Render();
				app.RenderGUI();

				if (++count >= frameCount + 10)
				{
					sam.Quit();
				}
			}
		);

		std::string eval = app.octreeEvaluationToString(true);
		ostream << "RESULT FOR " << pathToSaveFile << " Trace type " << traceType << std::endl;
		ostream << eval << std::endl;
	}
	istream.close();
	ostream.close();
}

int main(int argc, char* args[])
{
#ifdef TEST
	runTests("constantDegTests.txt", "output.txt", 100);
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
