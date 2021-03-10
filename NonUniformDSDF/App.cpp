#include "App.h"

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
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	CompileShaders();

	sam.AddHandlerClass(state.cam);

	CalculateOctreeSendToGPU();

	// Not working either:
	// CalculateOctreeSendToGPU();

	GL_CHECK;
}

void App::CalculateOctreeSendToGPU()
{
	state.activeApproxType().cpuConstruction(this);
	octree.print(state.octreeConstructionParams.minPos, state.octreeConstructionParams.minPos + state.octreeConstructionParams.sizeInWorld);

	std::vector<unsigned int> branchGPU, leavesGPU;
	octree.packForGPU(branchGPU, leavesGPU, octreeBranchCount);

	if (branchSSBOPoi != 0)
	{
		glDeleteBuffers(1, &branchSSBOPoi);
	}
	glGenBuffers(1, &branchSSBOPoi);

	//bool newlyCreated = false;
	//if (branchSSBOPoi == 0)
	//{
	//	glGenBuffers(1, &branchSSBOPoi);
	//	newlyCreated = true;
	//}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, branchSSBOPoi);
	int bufferSize = branchGPU.size() * sizeof(unsigned int);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)branchGPU.data(), GL_MAP_READ_BIT);
	// glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)branchGPU.data(), GL_STATIC_READ);

	//if (newlyCreated)
	//{
	//	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)branchGPU.data(), GL_STATIC_READ);
	//}
	//else
	//{
	//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, (GLvoid*)branchGPU.data());
	//}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glFlush(); glFinish();


	if (leavesSSBOPoi != 0)
	{
		glDeleteBuffers(1, &leavesSSBOPoi);
	}
	glGenBuffers(1, &leavesSSBOPoi);

	//newlyCreated = false;
	//if (leavesSSBOPoi == 0)
	//{
	//	glGenBuffers(1, &leavesSSBOPoi);
	//	newlyCreated = true;
	//}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, leavesSSBOPoi);
	bufferSize = leavesGPU.size() * sizeof(unsigned int);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)leavesGPU.data(), GL_MAP_READ_BIT);
	// glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)leavesGPU.data(), GL_STATIC_READ);

	//if (newlyCreated)
	//{
	//	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)leavesGPU.data(), GL_STATIC_READ);
	//}
	//else
	//{
	//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, (GLvoid*)leavesGPU.data());
	//}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glFlush(); glFinish();
	// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	// glMemoryBarrier(GL_ALL_BARRIER_BITS);
	// glMemoryBarrierByRegion(GL_SHADER_STORAGE_BARRIER_BIT);
	
	//branchSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
	// branchSSBO->constructImmutable(branchGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);
	//branchSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
	//branchSSBO->constructImmutable(branchGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);
	//branchSSBO->constructMutable(branchGPU, GL_STATIC_READ);
	//leavesSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
	//leavesSSBO->constructImmutable(leavesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);
	//leavesSSBO->constructMutable(leavesGPU, GL_STATIC_READ);
}


App::~App()
{
	for (auto sdf: state.sdfs)
	{
		delete sdf;
	}
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
	Backbuffer << Clear(0.1f, 0.1f, 0.1f, 1.0f);

	glm::mat4 mvp = state.cam.GetViewProj()
		* glm::translate(desc.SDFCorner + state.SDFTrans)
		* glm::scale(state.SDFScale * desc.SDFSize);

	// Draw grid of octree
	if (state.drawOctreeGrid)
	{
		DrawOctree(cubeWireProgram, octree.root(), state.drawOctreeLevel);

		// Draw bounding box
		Backbuffer
			<< cubeWireProgram
			<< "MVP" << mvp
			<< "color" << glm::vec3(1, 1, .2);
		cubeWireProgram << df::NoVao(GL_LINES, 24, 0);
	}

	//// Draw SDF
	auto& prog = sdfProgram;
	auto& cam = state.cam;
	auto& settings = state.settings;

	glm::vec3 modelTrans = glm::vec3(0);
	glm::vec3 modelScale = glm::vec3(1);

	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;

	/*branchSSBO->bindBufferRange(0);
	leavesSSBO->bindBufferRange(1);*/
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, branchSSBOPoi);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, leavesSSBOPoi);
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
		<< "octreeMinCoord" << state.octreeConstructionParams.minPos
		<< "octreeSize" << state.octreeConstructionParams.sizeInWorld
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

	ImGui::SetNextWindowSize({ 600,400 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Program settings"))
	{
		// Scene Settings
		if (ImGui::Button("Recalculate octree"))
		{
			CalculateOctreeSendToGPU();
			// CompileShaders();
		}
		if (ImGui::Button("Recenter camera"))
		{
			SceneCameraRecenter();
		}
		if (ImGui::Button("Recenter light source"))
		{
			SceneLightToCamera();
		}

		if (ImGui::BeginCombo("ApproxType", state.activeApproxType().name.c_str()))
		{
			for (int n = 0; n < state.approxTypes.size(); n++)
			{
				bool is_selected = n == state.activeApproxTypeIndex;

				if (ImGui::Selectable(state.approxTypes[n].name.c_str(), is_selected))
					state.activeApproxTypeIndex = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		
		// Drawing octree attributes
		ImGui::Checkbox("Draw octree grid", &state.drawOctreeGrid);
		if (state.drawOctreeGrid)
		{
			ImGui::DragInt("Show level", &state.drawOctreeLevel, 1, -1, state.octreeConstructionParams.maxLevel);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("-1 = all levels. 0 = upper most level. Increasing for lower levels.");
		}
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		
		// SDF settings
		if (ImGui::BeginCombo("##combo", currentSdf()->name().c_str()))
		{
			for (int n = 0; n < state.sdfs.size(); n++)
			{
				bool is_selected = n == state.activeSDFIndex;

				if (ImGui::Selectable(state.sdfs[n]->name().c_str(), is_selected))
					state.activeSDFIndex = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		currentSdf()->renderGUI();
	}
	ImGui::End();
	
	ImGui::ShowDemoWindow();
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


/* Writes the given values in the map to a glsl file as define directives, the key of the map being
the identifier and the value being the token-string.
*/
void writeDefines(std::map<std::string, std::string> defines, std::string path = "Shaders/defines.glsl")
{
	std::ofstream out(path);
	if (!out.is_open())
	{
		std::cout << "Could not open defines file!" << std::endl;
	}
	for (auto const& define : defines)
	{
		out << "#define " << define.first << " " << define.second << "\n";
	}

	out.close();
}

/* Called before the compile step
*/
void App::CompilePreprocess()
{
	std::map<std::string, std::string> defines{
		// which type of evalutaion to use for the polynomials
		{ "evalPolynom", state.activeApproxType().shaderEvalFunctionName }
	};
	
	writeDefines(defines);
}

void App::CompileShaders()
{
	CompilePreprocess();
	
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
		<< "Shaders/defines.glsl"_fs
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
		<< "Shaders/SDF/sample3d.glsl"_fs

		<< "Shaders/SDF/sdfOctree.glsl"_fs
		
		<< LinkProgram;
	std::cout << sdfProgram.GetErrors();
}
