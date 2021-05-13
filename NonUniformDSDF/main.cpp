#define NOMINMAX
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <string>

#include "EvaluationSystem/EvaluationSystem.h"
#include "App.h"

#define TEST

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
