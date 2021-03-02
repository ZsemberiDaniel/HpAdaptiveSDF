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

float SDF(glm::vec3 p)
{
	auto sdfBox = [](glm::vec3 p, glm::vec3 size)
	{
		glm::vec3 d = abs(p) - size;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(max(d, 0.0f));
	};
	
	// TODO: plane, stop at 1st
	
	// sphere in -2, -2, -2 with radius 1
	// return glm::length(p - glm::vec3(1)) - 0.2f;

	// plane
	return glm::abs(dot(p - glm::vec3(0, 0.5f, 0), glm::normalize(glm::vec3(0, 1, 0))));

	// return sdfBox(p, glm::vec3(1.0f, 0.3f, 1.0f));

	// 2odfok

	// torus
	// p -= glm::vec3(1);
	/*const float R = 0.8f;
	const float r = 0.4f;
	glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x, p.z)) - R, p.y);
	return length(q) - r;*/
}

/* Equation 4
*/
float shiftedNormalizedLegendre(const BoundingBox& bbox, glm::ivec3 degrees, glm::vec3 p)
{
	glm::vec3 shiftedNormalizedP = 2.0f / glm::vec3(bbox.max - bbox.min) * p 
							       - glm::vec3((bbox.max + bbox.min) / (bbox.max - bbox.min));
	glm::vec3 shiftedNormalizedCoeff = (2.0f * glm::vec3(degrees) + 1.0f) / glm::vec3(bbox.size());

	float value = (shiftedNormalizedCoeff.x) * std::legendref(degrees.x, shiftedNormalizedP.x);
	value *= (shiftedNormalizedCoeff.y) * std::legendref(degrees.y, shiftedNormalizedP.y);
	value *= (shiftedNormalizedCoeff.z) * std::legendref(degrees.z, shiftedNormalizedP.z);

	return value;
}

/* Equation 5
*/
Polynomial App::fitPolynomial(const BoundingBox& bbox, int degree)
{
	Polynomial polynomial(degree);

	int m = 0;
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				polynomial(m) = quadratureEvaluator.evaluateIntegral(4 * degree,
					[&bbox, i, k, j](glm::vec3 p)
					{
						return SDF(p) * shiftedNormalizedLegendre(bbox, glm::ivec3(i, k, j), p);
					}, bbox.min, bbox.max);
			}
		}
	}

	return polynomial;
}

void App::constructField(Grid& grid, int maxDegree, int maxLevel, float errorThreshold)
{
	auto getCoeffCount = [](int degree) -> int
	{
		return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
	};
	
	vector3d<Cell> initialCells(grid.initialCellCount);
	vector3d<Octree<Cell>::Leaf*> generatedCells(grid.initialCellCount);

	float error = 0.0f;
	// TODO: make pending a non-pair, only with the Cell and custom comparator
	std::priority_queue< std::pair<float, Cell> > pending;

	for (int x = 0; x < grid.initialCellCount; x++)
	{
		for (int y = 0; y < grid.initialCellCount; y++)
		{
			for (int z = 0; z < grid.initialCellCount; z++)
			{
				glm::vec3 cellCoord = grid.minPos + grid.cellSize * glm::vec3(x, y, z);
				BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + grid.cellSize };

				const int startingDegree = 2;
				Polynomial poly = fitPolynomial(bbox, startingDegree);
				/*std::cout << "Bbox  " << bbox.min.x << " " << bbox.min.y << " " << bbox.min.z << " to "
				          << bbox.max.x << " " << bbox.max.y << " " << bbox.max.z << std::endl;
				printPolynomial(poly);*/
				float currentError = poly.coefficientBasedError();

				Cell cell = Cell{bbox, poly, currentError, startingDegree, 1};

				error += currentError;

				initialCells(x, y, z) = cell;
			}
		}
	}
	octree = Octree<Cell>(grid.initialCellCount, initialCells, generatedCells);
	for (int x = 0; x < grid.initialCellCount; x++)
	{
		for (int y = 0; y < grid.initialCellCount; y++)
		{
			for (int z = 0; z < grid.initialCellCount; z++)
			{
				initialCells(x, y, z).octreeLeaf = generatedCells(x, y, z);
				pending.push(std::make_pair(initialCells(x, y, z).error, initialCells(x, y, z)));
			}
		}
	}

	while (!pending.empty() && error > errorThreshold)
	{
		auto popped = pending.top();
		pending.pop();

		Cell& currentCell = popped.second;
		if (currentCell.degree >= maxDegree && currentCell.level >= maxLevel) continue;

		float pImprovement = -std::numeric_limits<float>::max();
		float hImprovement = -std::numeric_limits<float>::max();

		// store the changed variables for p improvement so we don't have to calculate the again to
		// commit these changes to the octree
		Polynomial pImprovementPoly;
		float pImprovementError;

		// same storage as above but for h improvement
		vector3d<Cell> hImpSubdividedCell(2);
		
		// p-improvement
		// estimate error when using a polynom that is a degree higher
		if (true/*currentCell.degree < maxDegree*/)
		{
			pImprovementPoly = fitPolynomial(currentCell.bbox, currentCell.degree + 1);
			pImprovementError = pImprovementPoly.coefficientBasedError();

			// Equation (8), section 3.6
			pImprovement = 1.0f / (getCoeffCount(currentCell.degree + 1) - getCoeffCount(currentCell.degree)) *
				(currentCell.error - 8 * pImprovementError);
		}

		// h-improvement
		// subdivide cell into 2x2x2
		if (true/*currentCell.level < maxLevel*/)
		{
			// max error for subdivided cells, needed for eq. (9)
			float maxError = -std::numeric_limits<float>::max();
			// how big a subdivided cube's side is
			float subdividedCubeSize = currentCell.bbox.size().x / 2.0f;

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						// the subdivided cubes' coord is shifted from the bigger cube's coord
						glm::vec3 cellCoord = currentCell.bbox.min + glm::vec3(x, y, z) * glm::vec3(subdividedCubeSize);
						BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + glm::vec3(subdividedCubeSize) };

						// polynom for new cell + making new cell
						Polynomial poly = fitPolynomial(bbox, currentCell.degree);
						float currentError = poly.coefficientBasedError();
						if (maxError < currentError)
						{
							maxError = currentError;
						}

						Cell cell = Cell{ bbox, poly, currentError, currentCell.degree, currentCell.level + 1 };

						hImpSubdividedCell(x, y, z) = cell;
					}
				}
			}

			// equation (9), section 3.6
			hImprovement = 1.0f / 7.0f / getCoeffCount(currentCell.degree) * (currentCell.error - 8 * maxError);
		}

		bool refineP = currentCell.degree < maxDegree && (currentCell.level == maxLevel || pImprovement > hImprovement);
		bool refineH = currentCell.level < maxLevel   && !refineP;

		// deciding in favor of p-improvement
		if (refineP/*pImprovement >= hImprovement*/)
		{
			error += pImprovementError - currentCell.error;
			
			currentCell.degree++;
			currentCell.poly  = pImprovementPoly;
			currentCell.error = pImprovementError;
			// printPolynomial(currentCell.poly);

			currentCell.octreeLeaf->setValue(currentCell);

			pending.push(std::make_pair(currentCell.error, currentCell));
		}
		// doing h-improvement otherwise
		if (refineH/*pImprovement < hImprovement*/)
		{
			error -= currentCell.error;
			
			vector3d<Octree<Cell>::Leaf*> leaves(2);
			currentCell.octreeLeaf->subdivide(hImpSubdividedCell, leaves);

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						hImpSubdividedCell(x, y, z).octreeLeaf = leaves(x, y, z);
						pending.push(std::make_pair(hImpSubdividedCell(x, y, z).error, hImpSubdividedCell(x, y, z)));

						error += hImpSubdividedCell(x, y, z).error;
					}
				}
			}
		}
	}
}


App::App(df::Sample& s) : sam(s), noVao(GL_TRIANGLES, 3)
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitShaders();

	sam.AddHandlerClass(state.cam);

	constructField(octreeGrid);
	octree.print(octreeGrid.minPos, octreeGrid.minPos + octreeGrid.cellSize * (float)octreeGrid.initialCellCount);

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
		<< "color" << CUBE_COLORS[leaf->value().degree];
		program << df::NoVao(GL_LINES, 24, 0);
	}
}

void App::Render()
{
	// Clear backbuffer
	Backbuffer << (const df::detail::ClearF<0>&)state.clear; // Dragonfly pls... if it is not const, it thinks it is a program
	Backbuffer << Clear(0.1f, 0.1f, 0.1f, 1.0f);

	/*int currentLevel = static_cast<int>(SDL_GetTicks() / 1000.0f) % 5;
	DrawOctree(cubeWireProgram, octree.root(), currentLevel);*/

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
	float planeDist = glm::dot(cam.GetEye(), cam.GetDir()) + cam.GetNearFarClips().x;

	branchSSBO.bindBuffer(0);
	leavesSSBO.bindBuffer(1);
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
		<< "octreeMinCoord" << octreeGrid.minPos
		<< "octreeSize" << (octreeGrid.cellSize * static_cast<float>(octreeGrid.initialCellCount)).x
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
