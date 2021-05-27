#include "App.h"

#define TOOLTIP(msg) {if (ImGui::IsItemHovered()) {ImGui::SetTooltip(msg);}}

using namespace df;

const std::vector<glm::vec3> CUBE_COLORS = {
	glm::vec3(1),
	glm::vec3(0.5),
	glm::vec3(0.835, 0, 0),
	glm::vec3(0.188, 0.31, 0.996),
	glm::vec3(0, 0.784, 0.325),
	glm::vec3(1, 0.671, 0),
	glm::vec3(1, 0.573, 0.573),
	glm::vec3(0.86, 0.529, 0.655)
};

void testIntegrals()
{
	auto eval = [](std::pair<std::string, std::function<float(glm::vec3)>> func, IntegralEvaluator& eval, int maxPointCount, float realResult, glm::vec3 start, glm::vec3 end)
	{
		std::cout << std::setprecision(7);
		for (int i = maxPointCount; i >= 2; i /= 2)
		{
			std::cout << "Testing " << i << " points  " << func.first << " on x=" << start.x << ".." << end.x << " y=" << start.y << ".." << end.y << " z=" << start.z << ".." << end.z <<
				" \t\t Approx: " << eval.evaluateIntegral(i, func.second, start, end) << " \t Real: " << realResult << std::endl;
		}
	};

	AverageIntegralEvaluator avg;
	QuadratureEvaluator quad;
	auto fun1 = std::make_pair("x^2+ 2 * y^3 + z", [](glm::vec3 p) { return p.x * p.x + 2.0f * p.y * p.y * p.y + p.z; });
	auto fun2 = std::make_pair("sin(x + y)+ cos(2 * y^2) + z + x^2", [](glm::vec3 p) { return sin(p.x + p.y) + cos(2.0f * p.y * p.y) + p.z + p.x * p.x; });
	auto fun3 = std::make_pair("sin(x + y)+ y ^ 4 + 3.0 * z - 256 * x ^ 0.2", [](glm::vec3 p) { return sin(p.x + p.y) + p.y * p.y * p.y * p.y + 3.0f * p.z - 256.0f * pow(p.x, 0.2f); });

	std::cout << std::endl << std::endl << "------------------- I N T E G R A L   T E S T I N G -------------------" << std::endl;
	std::cout << "Simpson's max. 100 points" << std::endl;
	eval(fun1, avg, 100, 2.66667f, glm::vec3(-1), glm::vec3(1));
	eval(fun2, avg, 100, 8.00744f, glm::vec3(-1), glm::vec3(1));

	eval(fun1, avg, 100, 0.01826f, glm::vec3(0), glm::vec3(0.4f));
	eval(fun2, avg, 100, 0.017771f, glm::vec3(0.3f), glm::vec3(0.5f));

	std::cout << std::endl << "Gauss max. 100 points" << std::endl;
	eval(fun1, quad, 20, 2.66667f, glm::vec3(-1), glm::vec3(1));
	eval(fun2, quad, 20, 8.00744f, glm::vec3(-1), glm::vec3(1));

	eval(fun1, quad, 20, 0.01826f, glm::vec3(0), glm::vec3(0.4f));
	eval(fun2, quad, 20, 0.017771f, glm::vec3(0.3f), glm::vec3(0.5f));
	eval(fun3, quad, 20, -1.68663f, glm::vec3(0.3f), glm::vec3(0.5f));
	eval(fun3, quad, 20, -0.000200385f, glm::vec3(0.3f), glm::vec3(0.31f));
	eval(fun3, quad, 20, -0.00000192903f, glm::vec3(0.801f), glm::vec3(0.803f));
	eval(fun2, quad, 20, -0.0000000218018f, glm::vec3(0.801f), glm::vec3(0.803f));
}

App::App(df::Sample& s, int perfTestFrameCount, std::shared_ptr<SaveableOctree> octree) : sam(s), noVao(GL_TRIANGLES, 3), perfTest(perfTestFrameCount)
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (octree == nullptr)
	{
		CalculateOctreeSendToGPU();
	}
	else
	{
		currOctreeSet(octree);
	}

	sam.AddHandlerClass(state.cam);

	// testIntegrals();

	GL_CHECK;
}

void App::CalculateOctreeSendToGPU()
{
	currOctreeSet(std::make_shared<SaveableOctree>(state.activeSDFIndex, state.activeApproxTypeIndex, state.constructionParams));

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
	if (!state.inTestMode && state.drawOctreeGrid)
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
	auto prog = sdfProgram;

#ifdef USE_0th_ORDER
	if (show0thOrder) prog = sdf0thOrderProgram;
#endif

	auto& cam = state.cam;
	auto& settings = state.settings;

	glm::vec3 modelTrans = glm::vec3(0);
	glm::vec3 modelScale = glm::vec3(1);

	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;
	
#ifdef USE_0th_ORDER
	if (show0thOrder)
	{
		sdf0thOrderCurrOctree->bind();
	}
#endif
	currOctree->SetOctreeUniforms(*prog);
	
	Backbuffer << *prog
		<< "viewProj" << cam.GetViewProj()
		<< "planeDist" << planeDist
		<< "gInverseViewProj" << cam.GetInverseViewProj()
		<< "gCameraPos" << cam.GetEye()
		<< "gCameraDir" << cam.GetDir()
		<< "gLightPos" << settings.gLightPos
		<< "maxStep" << settings.maxStep
		<< "refineRoot" << state.settings.refineRoot
		<< "epsilon" << state.settings.epsilonToSurface
		<< "smallestStep" << state.settings.smallestStep
		<< "biggestStep" << state.settings.biggestStep
		<< "stepMultiplier" << state.settings.stepMultiplier
		<< "sphereTraceType" << state.settings.sphereTraceType;

	GL_CHECK;

	glm::vec3 dir = cam.GetDir();
	glm::vec3 frontVertex = glm::vec3((dir.x < 0 ? modelScale.x : 0), (dir.y < 0 ? modelScale.y : 0), (dir.z < 0 ? modelScale.z : 0));
	frontVertex += modelTrans;

	timer.Start();
	if (glm::dot(frontVertex, dir) < planeDist) // the bounding box' corner is clipped
		// clip
		*prog << df::NoVao(GL_TRIANGLE_FAN, 6, 3);
	// bounding box

	*prog << df::NoVao(GL_TRIANGLE_STRIP, 14, 9);
	perfTest.addFrameTime(timer.StopMillis());

	GL_CHECK;
}

void App::RenderGUI()
{
	if (!state.enableGUI || state.inTestMode) return;

	if (state.enableSDFShadersEditor)
	{
		sdfProgram->Render();
	}

#ifdef USE_0th_ORDER
	if (show0thOrderShaderEditor)
	{
		sdf0thOrderProgram->Render();
	}
#endif

	if (state.enableHeatmapShaderEditor)
	{
		SDFHeatmapVisualizer::renderShaderEditor();
	}
	ImGui::SetNextWindowSize({ 600,400 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Construction"))
	{
		if (ImGui::Button("Recalculate octree", ImVec2(450, 20)))
		{
			CalculateOctreeSendToGPU();
		}
		TOOLTIP("Calculates octree with current settings.");
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		// Construction settings
		if (ImGui::CollapsingHeader("Construction settings"))
		{
			ImGui::DragInt("Start degree", &state.constructionParams.startDegree, 0.3f, 0, 10);
			TOOLTIP("The degree of the initial cells in the octree.");
			ImGui::DragInt("Max degree", &state.constructionParams.maxDegree, 0.3f, 2, 10);
			TOOLTIP("The maximum degree that can be reached for each cell.");
			ImGui::DragInt("Max level", &state.constructionParams.maxLevel, 0.3f, 2, 10);
			TOOLTIP("The maximum level in the octree that can be reached for each cell.");
			ImGui::DragFloat("Error threshold", &state.constructionParams.errorThreshold, 0.000005f, 0.0000001f, 1.0f, "%.9f");
			TOOLTIP("The absolute sum error below which octree generation should be halted.");

			// make sure that at least one of the adapts is set to true
			if (ImGui::Checkbox("Use h-adapt", &state.constructionParams.useHAdapt))
			{
				if (!state.constructionParams.useHAdapt && !state.constructionParams.usePAdapt)
				{
					state.constructionParams.usePAdapt = true;
				}
			}
			TOOLTIP("Use subdivision step in octree generation?");
			if (ImGui::Checkbox("Use p-adapt", &state.constructionParams.usePAdapt))
			{
				if (!state.constructionParams.useHAdapt && !state.constructionParams.usePAdapt)
				{
					state.constructionParams.useHAdapt = true;
				}
			}
			TOOLTIP("Use polynomial degree raise step in octree generation?");

			if (state.activeApproxType().calculatedOnGPU)
			{
				ImGui::DragInt("Cell worker group size", &state.constructionParams.cellGroupSize, 0.3f, 1, 20);
				TOOLTIP("How many refinements to execute at once during octree generation");
			}
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

	ImGui::SetNextWindowSize({ 600,400 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Program settings"))
	{
		// Scene Settings
		TOOLTIP("Saves octree to selected file.");
		if (ImGui::Button("Recenter camera", ImVec2(450, 20)))
		{
			SceneCameraRecenter();
		}
		TOOLTIP("Relocates camera to default position.");
		if (ImGui::Button("Recenter light source", ImVec2(450, 20)))
		{
			SceneLightToCamera();
		}
		TOOLTIP("Relocates light source to camera."); if (ImGui::Button("Load from file", ImVec2(450, 20)))
		{
			std::wstring path = FileSelector::OpenLoadDialog();
			if (!path.empty())
			{
				currOctreeSet(SaveableOctree::loadFrom(path));
			}
		}
		TOOLTIP("Loads octree from selected file.");
		if (ImGui::Button("Save to file", ImVec2(450, 20)))
		{
			std::wstring path = FileSelector::OpenSaveDialog();
			if (!path.empty())
			{
				currOctree->saveTo(path);
			}
		}
		ImGui::Dummy(ImVec2(0.0f, 20.0f));


		ImGui::Text("Current octree %s", currOctree->getName());
		ImGui::Text("This list contains the inactive generated octrees in memory");
		if (ImGui::ListBoxHeader("SDF History"))
		{
			static int selectedOctreeHistory = 0;
			int i = 0;
			for (auto it = octreeHistory.begin(); it < octreeHistory.end(); it++, i++)
			{
				if (ImGui::Selectable((*it)->getName().c_str(), i == selectedOctreeHistory))
				{
					selectedOctreeHistory = i;
				}
			}
			ImGui::ListBoxFooter();
			if (ImGui::Button("Load selected", ImVec2(450, 20)))
			{
				currOctreeSetFromHistory(selectedOctreeHistory);
			}
		}
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		// ### Current description
		if (ImGui::CollapsingHeader("Current construction params"))
		{
			currOctree->renderConstructionParamsGUI();

			ImGui::Text("\n Data of last few frames:");
			auto results = perfTest.getResults();
			ImGui::Text("Render time min.: %f ms", results.min);
			ImGui::Text("Render time avg.: %f ms", results.avg);
			ImGui::Text("Render time max.: %f ms", results.max);

			ImGui::Text("");
			static ErrorStatistics::ErrorStatResult errorResults;

			if (ImGui::Button("Calculate statistics"))
			{
				static ErrorStatistics stats;
				static ErrorStatistics::StatSettings settings = {
					glm::vec3(0), // border
					true, // calcMedian
					true // calcHistogram
				};

				auto ref = currOctree->sdfFunction()->discreteSDFValuesTexture2D();
				auto data = currOctree->calculateSdf0thOrderTexture2D(ERROR_HEATMAP_SIZE);

				errorResults = stats.CalcStatistics(ref, data, settings);
			}

			std::stringstream ss;
			ss << "Mean:\t" << errorResults.mean << "\n"
				<< "Min:\t" << errorResults.min << "\n"
				<< "Max:\t" << errorResults.max << "\n"
				<< "Median:\t" << errorResults.median << "\n"
				<< "Sd:\t" << errorResults.sd << "\n"
				<< "L2:\t" << errorResults.l2;
			std::string outputString = ss.str();
			ImGui::Text(outputString.c_str());

			if (ImGui::Button("Copy text"))
			{
				const char* output = outputString.c_str();
				const size_t len = strlen(output) + 1;
				HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
				memcpy(GlobalLock(hMem), output, len);
				GlobalUnlock(hMem);
				OpenClipboard(0);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hMem);
				CloseClipboard();
			}
			TOOLTIP("Copies all the performance data to the clipboard.");
		}

		// ### Heatmaps
		if (ImGui::CollapsingHeader("Heatmaps"))
		{
			SDFHeatmapVisualizer::renderToGUI(errorHeatmapSlice.x, errorHeatmapSlice.y, errorHeatmapSlice.z, currOctree, state.cam);
			ImGui::DragInt3("Error heatmap slice", &errorHeatmapSlice.x, 0.5f, 0, ERROR_HEATMAP_SIZE - 1);
			TOOLTIP("Changes the coord of planes that slice the SDF.");
		}

		// ### Debug settings
		if (ImGui::CollapsingHeader("Debug settings"))
		{
#ifdef USE_0th_ORDER
			ImGui::Checkbox("Show 0th order", &show0thOrder);
			if (show0thOrder)
			{
				static int resolution = sdf0thOrderResolution;
				ImGui::InputInt("0th ortder resolution", &resolution);
				if (resolution != sdf0thOrderResolution)
				{
					sdf0thOrderResolution = resolution;
					sdf0thOrderCurrOctree = currOctree->calculateSdf0thOrderTexture(sdf0thOrderResolution);
				}
			}
#endif
			// Drawing octree attributes
			ImGui::Checkbox("Draw octree grid", &state.drawOctreeGrid);
			TOOLTIP("When checked, draws the wireframe of the octree grid.");
			if (state.drawOctreeGrid)
			{
				ImGui::DragInt("Show level", &state.drawOctreeLevel, 0.1f, -1, state.constructionParams.maxLevel);
				TOOLTIP("-1 = all levels. 0 = upper most level. Increasing for lower levels.");

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
			TOOLTIP("When checked, shows surface normals. Recompiles shaders!");

			if (ImGui::Checkbox("Print octree", &state.printOctree))
			{
				if (state.printOctree)
				{
					PrintCurrentOctree();
				}
			}
			TOOLTIP("When checked, prints the polys of the current octree (LONG OPERATION!)\nand prints all future octrees' polys when they are generated.");
		}

		if (ImGui::CollapsingHeader("Render settings"))
		{
			ImGui::Text("Sphere trace type:");
			ImGui::RadioButton("Basic", &state.settings.sphereTraceType, 0); ImGui::SameLine();
			TOOLTIP("Use basic sphere tracing to display the SDF.");
			ImGui::RadioButton("Relaxed", &state.settings.sphereTraceType, 1); ImGui::SameLine();
			TOOLTIP("Use relaxed sphere tracing to display the SDF.");
			ImGui::RadioButton("Enhanced", &state.settings.sphereTraceType, 2);
			TOOLTIP("Use enhanced sphere tracing to display the SDF.");

			ImGui::Text( "Root refinement type:" );
			ImGui::RadioButton( "None", &state.settings.refineRoot, 0 ); ImGui::SameLine();
			TOOLTIP("Use no root refinement to display the SDF.");
			ImGui::RadioButton( "Bisection", &state.settings.refineRoot, 1 ); ImGui::SameLine();
			TOOLTIP("Use bisection based root refinement to display the SDF.");
			ImGui::RadioButton( "Linear", &state.settings.refineRoot, 2 );
			TOOLTIP("Use linear root refinement to display the SDF.");

			ImGui::InputInt("Max step", &state.settings.maxStep);
			ImGui::InputFloat("Epsilon to surface", &state.settings.epsilonToSurface, 0, 0, "%.7f");
			TOOLTIP("At what distance the tracing algorithm should stop when close to the SDF surface.");
			ImGui::InputFloat("Smallest allowed step", &state.settings.smallestStep, 0, 0, "%.7f");
			TOOLTIP("The smallest allowed step for the tracing algorithm.");
			ImGui::InputFloat("Biggest allowed step", &state.settings.biggestStep, 0, 0, "%.5f");
			TOOLTIP("The highest allowed step for the tracing algorithm.");
			ImGui::InputFloat("Step multiplier", &state.settings.stepMultiplier, 0, 0, "%.5f");
			TOOLTIP("A multiplier for each step in the tracing algorithm.");

			if (ImGui::Checkbox("Use lookup table", &state.useLookupTable))
			{
				CompileShaders();
			}
			TOOLTIP("Use lookup table to display the SDF?");
		}

		// ### Shader editors
		if (ImGui::CollapsingHeader("Shader editors"))
		{
			ImGui::Checkbox("SDF octree shader", &state.enableSDFShadersEditor);
			TOOLTIP("The editor of the main octree renderer");
			ImGui::Checkbox("SDF heatmap shader", &state.enableHeatmapShaderEditor);
			TOOLTIP("The editor for displaying the heatmaps");
#ifdef USE_0th_ORDER
			ImGui::Checkbox("SDF 0th order shader", &show0thOrderShaderEditor);
#endif
		}
	}
	ImGui::End();
}

bool App::HandleKeyDown(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_F2:
		// toggle GUI
		state.enableGUI = !state.enableGUI;
		return true;
	case SDLK_F3:
		std::cout << octreeEvaluationToString();
		return true;
	}
	return false;
}

/* Called before the compile step
*/
void App::CompilePreprocess()
{
	std::map<std::string, std::string> defines;
	if (state.showNormals)
	{
		defines["SHOW_NORMALS"] = "";
	}
	if (state.useLookupTable) 
	{
		defines["USE_LOOKUP_TABLE"] = "";
	}
	
	currOctree->saveDefinesFile(defines);
}

void App::CompileShaders()
{
	CompilePreprocess();
	
	std::cout << "Compiling cube wire program...  " << std::endl;
	cubeWireProgram
		<< "Shaders/cube.vert"_vs
		<< "Shaders/cube.frag"_fs
		<< LinkProgram;
	std::cout << cubeWireProgram.GetErrors() << std::endl;

	std::cout << "Compiling cube mesh program...  " << std::endl;
	meshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << meshProgram.GetErrors() << std::endl;

	std::cout << "Compiling mesh program...  " << std::endl;
	flatMeshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/flatMesh.geom"_geom
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << flatMeshProgram.GetErrors() << std::endl;

	std::cout << "Compiling sdf trace program...  " << std::endl;
	
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
	std::cout << sdfProgram->GetErrors() << std::endl;

	if (!state.inTestMode)
		SDFHeatmapVisualizer::RecompileShaders();

#ifdef USE_0th_ORDER
	delete sdf0thOrderProgram;
	sdf0thOrderProgram = new df::ShaderProgramEditorVF("SDF-0thorder-prog");
	*sdf0thOrderProgram
		<< "Shaders/defines.glsl"_fs
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

		<< "Shaders/SDF/sdf0thOrder.glsl"_fs

		<< LinkProgram;
	std::cout << sdf0thOrderProgram->GetErrors();
#endif
}
