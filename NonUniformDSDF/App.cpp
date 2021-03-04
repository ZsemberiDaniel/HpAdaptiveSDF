#include "App.h"

#include <glm/gtx/transform.hpp>

using namespace df;

const glm::vec3 CUBE_COLORS[8] = {
	glm::vec3(0.92, 0.50, 0.33),
	glm::vec3(0.02, 0.14, 0.20),
	glm::vec3(0.80, 0.42, 0.44),
	glm::vec3(0.07, 0.19, 0.43),
	glm::vec3(0.25, 0.22, 0.57),
	glm::vec3(0.42, 0.27, 0.59),
	glm::vec3(0.49, 0.31, 0.56),
	glm::vec3(0.65, 0.36, 0.52)
};

App::App(df::Sample& s) : sam(s), noVao(GL_TRIANGLES, 3)
{
	// TODO: resize not working
	
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitShaders();

	sam.AddHandlerClass(state.cam);

	auto SDF = [](glm::vec3 p) -> float
	{
		auto sdfBox = [](glm::vec3 p, glm::vec3 size)
		{
			glm::vec3 d = abs(p) - size;
			return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(max(d, 0.0f));
		};

		// TODO: plane, stop at 1st

		// sphere in -2, -2, -2 with radius 1
		return glm::length(p - glm::vec3(0.5f)) - 0.4f;

		// plane
		// return dot(p - glm::vec3(0, 0, 0.5f), glm::normalize(glm::vec3(0.4f, 0, 1)));

		// return sdfBox(p - glm::vec3(0.5f), glm::vec3(0.2f));

		// 2odfok

		// torus
		// p -= glm::vec3(1);
		/*const float R = 0.8f;
		const float r = 0.4f;
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x, p.z)) - R, p.y);
		return length(q) - r;*/
	};

	OctreeGenerator::constructField<GaussPolynomialGenerator>(octree, gaussPolyGenerator, octreeConstructionParams, SDF);
	// OctreeGenerator::constructField<LSQPolyGenerator>(octree, lsqGenerator, octreeConstructionParams, SDF);
	octree.print(octreeConstructionParams.minPos, octreeConstructionParams.minPos + octreeConstructionParams.sizeInWorld);

	std::vector<unsigned int> branchGPU, leavesGPU;
	octree.packForGPU(branchGPU, leavesGPU, octreeBranchCount);
	/*for (const auto& i : branchGPU)
		std::cout << i << '\t';
	std::cout << std::endl;
	for (const auto& i : leavesGPU)
		std::cout << i << '\t';
	std::cout << std::endl;*/

	branchSSBO.constructImmutable(branchGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);
	leavesSSBO.constructImmutable(leavesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);

	GL_CHECK;
}

App::~App()
{
}

void App::Update()
{
	state.cam.Update();
}

void App::DrawOctree(df::ShaderProgramEditorVF& program, const Octree<Cell>::Node* currentNode, int level)
{
	if (currentNode->type() == Octree<Cell>::BranchNode)
	{
		auto* branch = reinterpret_cast<const Octree<Cell>::Branch*>(currentNode);
		for (int i = 0; i < 8; i++)
		{
			DrawOctree(program, branch->child(i), level);
		}
	}
	else
	{
		assert(currentNode->type() == Octree<Cell>::LeafNode);
		
		auto* leaf = reinterpret_cast<const Octree<Cell>::Leaf*>(currentNode);
		if (level != -1 && leaf->layer() != level) return;

		df::Backbuffer << program
		<< "MVP" << state.cam.GetViewProj() *
			        glm::translate(glm::vec3(leaf->value().bbox.min)) * 
			        glm::scale(glm::vec3(leaf->value().bbox.size().x))
		<< "color" << CUBE_COLORS[leaf->value().degree()];
		program << df::NoVao(GL_LINES, 24, 0);
	}
}

void App::Render()
{
	// Clear backbuffer
	Backbuffer << (const df::detail::ClearF<0>&)state.clear; // Dragonfly pls... if it is not const, it thinks it is a program
	Backbuffer << Clear(0.1f, 0.1f, 0.1f, 1.0f);

	int currentLevel = static_cast<int>(SDL_GetTicks() / 1000.0f) % 5;
	DrawOctree(cubeWireProgram, octree.root(), -1);

	glm::mat4 mvp = state.cam.GetViewProj()
		* glm::translate(desc.SDFCorner + state.SDFTrans)
		* glm::scale(state.SDFScale * desc.SDFSize);

	// Draw bounding box
	Backbuffer
		<< cubeWireProgram
		<< "MVP" << mvp
		<< "color" << glm::vec3(1, 1, .2);
	cubeWireProgram << df::NoVao(GL_LINES, 24, 0);

	//// Draw SDF
	auto& prog = sdfProgram;
	auto& cam = state.cam;
	auto& settings = state.settings;

	glm::vec3 modelTrans = desc.SDFCorner + state.SDFScale * desc.SDFBorder + state.SDFTrans;
	glm::vec3 modelScale = state.SDFScale * (desc.SDFSize - 2.0f * desc.SDFBorder);
	
	modelTrans = glm::vec3(0);
	modelScale = glm::vec3(1);

	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;

	branchSSBO.bindBufferRange(0);
	leavesSSBO.bindBufferRange(1);
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
		<< "branchCount"<< octreeBranchCount
		<< "octreeMinCoord" << octreeConstructionParams.minPos
		<< "octreeSize" << octreeConstructionParams.sizeInWorld
		<< "param1" << glm::vec3(0.7, 0, 0)
		<< "param2" << glm::vec3(0, 0, 0)
		<< "time" << SDL_GetTicks() / 1000.0f;

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
		<< "Shaders/Math/common.glsl"_fs
		<< "Shaders/common.glsl"_fs
		<< "Shaders/Octree/octree.glsl"_fs
		<< "Shaders/Math/box_ray_intersection.glsl"_fs
		<< "Shaders/Shade/ambient_occlusion.glsl"_fs
		<< "Shaders/Shade/brdf.glsl"_fs
		<< "Shaders/Shade/basic_shade.glsl"_fs
		<< "Shaders/main_cube.glsl"_fs
		<< "Shaders/sdf.glsl"_fs
		<< "Shaders/Math/primitives.glsl"_fs
//		<< "Shaders/SDF/weightBlend.glsl"_fs
		<< "Shaders/SDF/sample3d.glsl"_fs

		<< "Shaders/SDF/sdfOctree.glsl"_fs
		// << "Shaders/SDFScenes/torus_pipes.glsl"_fs
		// << "Shaders/SDF/sdfProceduralScene.glsl"_fs
//
// 
//		<< "Shaders/SDF/sdf0thOrder.glsl"_fs
//		<< "Shaders/SDF/sdf1stOrder.glsl"_fs
		
		<< LinkProgram;
	std::cout << sdfProgram.GetErrors();
}
