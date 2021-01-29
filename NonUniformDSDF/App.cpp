#include "App.h"

#include <glm/gtx/transform.hpp>

using namespace df;

App::App(df::Sample& s) : sam(s), noVao(GL_TRIANGLES, 3)
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitShaders();

	sam.AddHandlerClass(state.cam);

	GL_CHECK;
}

App::~App()
{
}

void App::Update()
{
	state.cam.Update();
}

void App::Render()
{
	// Clear backbuffer
	Backbuffer << (const df::detail::ClearF<0>&)state.clear; // Dragonfly pls... if it is not const, it thinks it is a program

	glm::mat4 mvp = state.cam.GetViewProj()
		* glm::translate(desc.SDFCorner + state.SDFTrans)
		* glm::scale(state.SDFScale * desc.SDFSize);

	// Draw bounding box
	Backbuffer
		<< cubeWireProgram
		<< "MVP" << mvp
		<< "color" << glm::vec3(1, 1, .2);
	cubeWireProgram << df::NoVao(GL_LINES, 24, 0);

	// Draw SDF
	auto& prog = sdfProgram;
	auto& cam = state.cam;
	auto& settings = state.settings;

	glm::vec3 modelTrans = desc.SDFCorner + state.SDFScale * desc.SDFBorder + state.SDFTrans;
	glm::vec3 modelScale = state.SDFScale * (desc.SDFSize - 2.0f * desc.SDFBorder);
	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;
	Backbuffer << prog
		<< "viewProj" << cam.GetViewProj()
		<< "modelTrans" << modelTrans
		<< "modelScale" << modelScale
		<< "planeDist" << planeDist
		<< "gInverseViewProj" << cam.GetInverseViewProj()
		<< "gTanPixelAngle" << cam.GetTanPixelFow()
		<< "gCameraPos" << cam.GetEye()
		<< "gCameraDir" << cam.GetDir()
		<< "gLightPos" << settings.gLightPos
		<< "gAOMaxIters" << settings.gAOMaxIters
		<< "gAOStepSize" << settings.gAOStepSize
		<< "gAOStrength" << settings.gAOStrength
		<< "gAmbient" << settings.gAmbient
		<< "gDiffuse" << settings.gDiffuse
		<< "gCookRoughness" << settings.gCookRoughness
		<< "gCookIOR" << settings.gCookIOR
		<< "gNormEps" << 0.01f
		<< "maxStep" << settings.maxStep
		<< "sdfTexSize" << desc.SDFSize
		<< "sdfTexCorner" << desc.SDFCorner
		<< "sdfTexBorder" << desc.SDFBorder
		<< "sTranslation" << state.SDFTrans
		<< "sScale" << state.SDFScale
		<< "refineRoot" << (settings.refineRoot ? 1 : 0)
		<< "param1" << glm::vec3(0.7, 0, 0)
		<< "param2" << glm::vec3(0, 0, 0);

	glm::vec3 dir = cam.GetDir();
	glm::vec3 frontVertex = glm::vec3((dir.x < 0 ? modelScale.x : 0), (dir.y < 0 ? modelScale.y : 0), (dir.z < 0 ? modelScale.z : 0));
	frontVertex += modelTrans;
	if (glm::dot(frontVertex, dir) < planeDist) // the bounding box' corner is clipped
		// clip
		prog << df::NoVao(GL_TRIANGLE_FAN, 6, 3);
	// bounding box
	prog << df::NoVao(GL_TRIANGLE_STRIP, 14, 9);


	GL_CHECK;
}

void App::RenderGUI()
{
	if (!state.enableGUI) return;
	sdfProgram.Render();
	state.cam.RenderUI();

	ImGui::SliderFloat3("corner", &desc.SDFCorner.x, -5, 5);
	ImGui::SliderFloat3("size", &desc.SDFSize.x, .5, 5);
}

bool App::HandleKeyDown(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_F2:
		// toggle GUI
		state.enableGUI = !state.enableGUI;
		return true;
	}
	return false;
}

void App::InitShaders()
{
	std::cout << "Compiling cube wire program...  ";
	cubeWireProgram
		<< "Shaders/cube.vert"_vs
		<< "Shaders/cube.frag"_fs
		<< LinkProgram;
	std::cout << cubeWireProgram.GetErrors();

	std::cout << "Compiling cube mesh program...  ";
	meshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << meshProgram.GetErrors();

	std::cout << "Compiling mesh program...  ";
	flatMeshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/flatMesh.geom"_geom
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << flatMeshProgram.GetErrors();

	std::cout << "Compiling sdf trace program...  ";
	sdfProgram
		<< "Shaders/Math/box_plane_intersection.glsl"_vs
		<< "Shaders/cube_solid.vert"_vs

		<< "Shaders/render_main.frag"_fs
		<< "Shaders/uniforms.glsl"_fs
		<< "Shaders/common.glsl"_fs
		<< "Shaders/Math/box_ray_intersection.glsl"_fs
		<< "Shaders/Shade/ambient_occlusion.glsl"_fs
		<< "Shaders/Shade/brdf.glsl"_fs
		<< "Shaders/Shade/basic_shade.glsl"_fs
		<< "Shaders/main_cube.glsl"_fs
		<< "Shaders/sdf.glsl"_fs
		<< "Shaders/Math/primitives.glsl"_fs
//		<< "Shaders/SDF/weightBlend.glsl"_fs
		<< "Shaders/SDF/sample3d.glsl"_fs

		<< "Shaders/SDFScenes/torus_pipes.glsl"_fs
		<< "Shaders/SDF/sdfProceduralScene.glsl"_fs
//		<< "Shaders/SDF/sdf0thOrder.glsl"_fs
//		<< "Shaders/SDF/sdf1stOrder.glsl"_fs
		
		<< LinkProgram;
	std::cout << sdfProgram.GetErrors();

	
}
