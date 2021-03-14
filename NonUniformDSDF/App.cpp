#include "App.h"

using namespace df;

const std::vector<glm::vec3> CUBE_COLORS = {
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	CalculateOctreeSendToGPU();
	CompileShaders();

	sam.AddHandlerClass(state.cam);


	GL_CHECK;
}

void App::CalculateOctreeSendToGPU()
{
	std::cout << state.activeApproxTypeIndex << std::endl;
	currOctree = std::make_shared<SaveableOctree>(state.activeSDFIndex, state.activeApproxTypeIndex, state.constructionParams);

	if (state.printOctree) PrintCurrentOctree();
}


App::~App(){}

void App::Update()
{
	state.cam.Update();
}

void App::DrawOctree(df::ShaderProgramEditorVF& program, const std::shared_ptr<Octree<Cell>::Node>& currentNode, int level)
{
	if (currentNode->type() == Octree<Cell>::BranchNode)
	{
		auto branch = std::dynamic_pointer_cast<const Octree<Cell>::Branch>(currentNode);
		for (int i = 0; i < 8; i++)
		{
			const std::shared_ptr<Octree<Cell>::Node> ch = branch->child(i);
			DrawOctree(program, ch, level);
		}
	}
	else
	{
		assert(currentNode->type() == Octree<Cell>::LeafNode);
		
		auto leaf = std::dynamic_pointer_cast<const Octree<Cell>::Leaf>(currentNode);
		if (level != -1 && leaf->layer() != level) return;

		df::Backbuffer << program
		<< "MVP" << state.cam.GetViewProj() *
			        glm::translate(glm::vec3(leaf->value().bbox.min) / currOctree->getConstructionParams().sizeInWorld) * 
			        glm::scale(glm::vec3(leaf->value().bbox.size().x) / currOctree->getConstructionParams().sizeInWorld)
		<< "color" << CUBE_COLORS[leaf->value().degree()];
		program << df::NoVao(GL_LINES, 24, 0);
	}
}

void App::Render()
{
	// Clear backbuffer
	Backbuffer << Clear(0.1f, 0.1f, 0.1f, 1.0f);

	if (currOctree == nullptr) return;

	glm::mat4 mvp = state.cam.GetViewProj();

	// Draw grid of octree
	if (state.drawOctreeGrid)
	{
		DrawOctree(cubeWireProgram, currOctree->getOctree()->root(), state.drawOctreeLevel);

		if (state.drawOctreeLevel != -1)
		{
			// Draw bounding box
			Backbuffer
				<< cubeWireProgram
				<< "MVP" << mvp
				<< "color" << glm::vec3(1, 1, .2);
			cubeWireProgram << df::NoVao(GL_LINES, 24, 0);
		}
	}

	//// Draw SDF
	auto& prog = sdfProgram;
	auto& cam = state.cam;
	auto& settings = state.settings;

	glm::vec3 modelTrans = glm::vec3(0);
	glm::vec3 modelScale = glm::vec3(1);

	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;

	currOctree->SetOctreeUniforms(*prog);
	Backbuffer << *prog
		<< "viewProj" << cam.GetViewProj()
		<< "modelTrans" << modelTrans
		<< "modelScale" << modelScale
		<< "planeDist" << planeDist
		<< "gInverseViewProj" << cam.GetInverseViewProj()
		// << "gTanPixelAngle" << cam.GetTanPixelFow()
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
		<< "gNormEps" << glm::vec3(0.01f)
		<< "maxStep" << settings.maxStep
		<< "refineRoot" << (state.settings.refineRoot ? 1 : 0)
		<< "epsilon" << state.settings.stepEpsilon;


	GL_CHECK;

	glm::vec3 dir = cam.GetDir();
	glm::vec3 frontVertex = glm::vec3((dir.x < 0 ? modelScale.x : 0), (dir.y < 0 ? modelScale.y : 0), (dir.z < 0 ? modelScale.z : 0));
	frontVertex += modelTrans;
	if (glm::dot(frontVertex, dir) < planeDist) // the bounding box' corner is clipped
		// clip
		*prog << df::NoVao(GL_TRIANGLE_FAN, 6, 3);
	// bounding box

	*prog << df::NoVao(GL_TRIANGLE_STRIP, 14, 9);

	GL_CHECK;

}

void App::RenderGUI()
{
	if (!state.enableGUI) return;

	if (state.enableSDFShadersEditor)
	{
		sdfProgram->Render();
	}

	if (state.enableHeatmapShaderEditor)
	{
		SDFHeatmapVisualizer::renderShaderEditor();
	}

	// MAIN MENU BAR
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Button("Load"))
			{
				std::wstring path = FileSelector::OpenLoadDialog();
				if (!path.empty())
				{
					currOctreeSet(SaveableOctree::loadFrom(path));
				}
			}

			if (ImGui::Button("Save"))
			{
				std::wstring path = FileSelector::OpenSaveDialog();
				if (!path.empty())
				{
					currOctree->saveTo(path);
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowSize({ 600,400 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Program settings"))
	{
		// Scene Settings
		if (ImGui::Button("Recalculate octree", ImVec2(450, 20)))
		{
			CalculateOctreeSendToGPU();
			CompileShaders();
		}
		if (ImGui::Button("Recenter camera", ImVec2(450, 20)))
		{
			SceneCameraRecenter();
		}
		if (ImGui::Button("Recenter light source", ImVec2(450, 20)))
		{
			SceneLightToCamera();
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		// ### Heatmaps
		if (ImGui::CollapsingHeader("Heatmaps"))
		{
			SDFHeatmapVisualizer::renderToGUI(errorHeatmapSlice.x, errorHeatmapSlice.y, errorHeatmapSlice.z, currOctree, state.cam);
			ImGui::DragInt3("Error heatmap slice", &errorHeatmapSlice.x, 0.5f, 0, ERROR_HEATMAP_SIZE - 1);
		}

		// ### Debug settings
		if (ImGui::CollapsingHeader("Debug settings"))
		{
			// Drawing octree attributes
			ImGui::Checkbox("Draw octree grid", &state.drawOctreeGrid);
			if (state.drawOctreeGrid)
			{
				ImGui::DragInt("Show level", &state.drawOctreeLevel, 0.1f, -1, state.constructionParams.maxLevel);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("-1 = all levels. 0 = upper most level. Increasing for lower levels.");

				ImGui::Text("Level colors:");
				ImVec2 rectSize = ImVec2(15, 15);
				const ImVec2 p = ImGui::GetCursorScreenPos();
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				int rowCount = 0;

				ImVec2 cursor = p;

				for (int i = 0; i < CUBE_COLORS.size(); i++)
				{
					auto c = ImGui::GetColorU32(ImVec4(CUBE_COLORS[i].x, CUBE_COLORS[i].y, CUBE_COLORS[i].z, 1.0f));
					draw_list->AddRectFilled(cursor, ImVec2(cursor.x + rectSize.x, cursor.y + rectSize.y), c);

					cursor.x += rectSize.x;
				}

				ImGui::Dummy(ImVec2(rectSize.x * CUBE_COLORS.size(), rectSize.y));
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
			}

			if (ImGui::Checkbox("Show surface normals", &state.showNormals))
			{
				CompileShaders();
			}

			if (ImGui::Checkbox("Print octree", &state.printOctree))
			{
				if (state.printOctree)
				{
					PrintCurrentOctree();
				}
			}
		}

		if (ImGui::CollapsingHeader("Render settings"))
		{
			ImGui::Checkbox("Refine root", &state.settings.refineRoot);
			ImGui::InputInt("Max step", &state.settings.maxStep);
			ImGui::InputFloat("Epsilon to surface", &state.settings.stepEpsilon, 0, 0, "%.7f");
		}

		// Construction settings
		if (ImGui::CollapsingHeader("Construction settings"))
		{
			ImGui::DragInt("Max degree", &state.constructionParams.maxDegree, 0.3f, 2, 10);
			ImGui::DragInt("Max level", &state.constructionParams.maxLevel, 0.3f, 2, 10);
			ImGui::DragFloat("Error threshold", &state.constructionParams.errorThreshold, 0.000005f, 0.0000001f, 1.0f, "%.7f");
		}

		// ### Shader editors
		if (ImGui::CollapsingHeader("Shader editors"))
		{
			ImGui::Checkbox("SDF octree shader", &state.enableSDFShadersEditor);
			ImGui::Checkbox("SDF heatmap shader", &state.enableHeatmapShaderEditor);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));


		if (ImGui::BeginCombo("ApproxType", state.activeApproxType().name.c_str()))
		{
			for (int n = 0; n < sizeof(approxTypes) / sizeof(PolynomialBase); n++)
			{
				bool is_selected = n == state.activeApproxTypeIndex;

				if (ImGui::Selectable(approxTypes[n].name.c_str(), is_selected))
					state.activeApproxTypeIndex = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		
		// SDF settings
		if (ImGui::BeginCombo("##combo", state.activeSDF()->name().c_str()))
		{
			for (int n = 0; n < sdfs.size(); n++)
			{
				bool is_selected = n == state.activeSDFIndex;

				if (ImGui::Selectable(sdfs[n]->name().c_str(), is_selected))
					state.activeSDFIndex = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		state.activeSDF()->renderGUI();
	}
	ImGui::End();
	
	// ImGui::ShowDemoWindow();
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
		{ "evalPolynom", state.activeApproxType().shaderEvalFunctionName },

		{ "MAX_COEFF_SIZE", std::to_string(Polynomial::calculateCoeffCount(currOctree->getConstructionParams().maxLevel) + 1) }
	};

	if (state.showNormals)
	{
		defines["SHOW_NORMALS"] = "";
	}
	
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
	
	delete sdfProgram;
	sdfProgram = new df::ShaderProgramEditorVF("SDF-prog");
	*sdfProgram
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
	std::cout << sdfProgram->GetErrors();
}
