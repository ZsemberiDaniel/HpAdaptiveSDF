#include "App.h"

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
	Backbuffer << Clear(1, 1, 1, 1);
	auto& prog = sdfProgram;
	

	GL_CHECK;
}

void App::RenderGUI()
{
	if (!state.enableGUI) return;
	sdfProgram.Render();
	state.cam.RenderUI();
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
		<< "Shaders/SDF/weightBlend.glsl"_fs
		<< "Shaders/SDF/sample3d.glsl"_fs

		<< "Shaders/SDFScenes/torus_pipes.glsl"_fs
		<< "Shaders/SDF/sdfProceduralScene.glsl"_fs
//		<< "Shaders/SDF/sdf0thOrder.glsl"_fs
//		<< "Shaders/SDF/sdf1stOrder.glsl"_fs
		
		<< LinkProgram;
	std::cout << sdfProgram.GetErrors();

	
}
