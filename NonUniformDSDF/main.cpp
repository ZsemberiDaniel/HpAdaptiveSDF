#include <Dragonfly/editor.h>		 //inlcludes most features
#include <string>

#include "App.h"

int main(int argc, char* args[])
{
	df::Sample sam("Non-uniform DSDF", 720, 480, df::Sample::FLAGS::DEFAULT);
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
	return 0;
}
