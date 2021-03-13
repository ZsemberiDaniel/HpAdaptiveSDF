#ifndef SDFHEATMAPVISUALIZER_H
#define SDFHEATMAPVISUALIZER_H

#include <sstream>
#include <memory>

#include "SDFBase.h"
#include "../constants.h"
#include "../Utils/Lazy.hpp"

using namespace df;

class SDFHeatmapVisualizer
{
public:
	inline static void renderShaderEditor()
	{
		differenceCalculateProgram()->Render();
	}

	inline static void renderToGUI(int xSlice, int ySlice, int zSlice, std::shared_ptr<SaveableOctree> saveableOctree, df::Camera& cam)
	{
		// update SSBO
		if (saveableOctree != previouslyDrawnHeatmap)
		{
			previouslyDrawnHeatmap = saveableOctree;
			sdfSSBO.force().assignMutable(saveableOctree->sdfFunction()->discreteSDFValues()->innerVector(), 0);
		}

		int heatMapSize = ERROR_HEATMAP_SIZE;

		glBindImageTexture(0, (GLuint)sdfValuesXSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, (GLuint)sdfValuesYSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, (GLuint)sdfValuesZSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glBindImageTexture(3, (GLuint)sdfErrorXSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(4, (GLuint)sdfErrorYSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(5, (GLuint)sdfErrorZSlice.force(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		sdfSSBO.force().bindBufferRange(3);

		saveableOctree->SetOctreeUniforms(*(differenceCalculateProgram()));
		*(differenceCalculateProgram())
			<< "heatMapSize" << heatMapSize
			<< "xSliceId" << xSlice
			<< "ySliceId" << ySlice
			<< "zSliceId" << zSlice
			<< "maxSDFValue" << saveableOctree->sdfFunction()->discreteMaxSDFValue()
			<< "renderDiff" << (int)renderDiff
			<< "diffMultiplier" << diffMultiplier;
		

		glDispatchCompute(heatMapSize, heatMapSize, 1);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		GL_CHECK;

		ImGui::Checkbox("Render diff", &renderDiff);
		if (renderDiff) ImGui::InputFloat("Diff multiplier", &diffMultiplier);

		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorXSlice.force(), ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorYSlice.force(), ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(2 * heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorZSlice.force(), ImVec2(heatMapSize, heatMapSize));


		// Displays the SDF value textures
		ImGui::Text("SDF VALUES");
		ImGui::Text(std::to_string(saveableOctree->sdfFunction()->discreteMaxSDFValue()).c_str());
		ImGui::SameLine();
		{
			ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth() * 0.4f, 20);
			ImVec2 p0 = ImGui::GetCursorScreenPos();
			ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
			ImU32 col_a = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImU32 col_b = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
			ImGui::InvisibleButton("##gradient1", gradient_size);
		}
		ImGui::SameLine();
		ImGui::Text("0.0");
		ImGui::SameLine();
		{
			ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth() * 0.4f, 20);
			ImVec2 p0 = ImGui::GetCursorScreenPos();
			ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
			ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImU32 col_b = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
			ImGui::InvisibleButton("##gradient2", gradient_size);
		}
		ImGui::SameLine();
		ImGui::Text(std::to_string(-saveableOctree->sdfFunction()->discreteMaxSDFValue()).c_str());

		ImGui::Text("X slice");
		ImGui::SameLine(heatMapSize);
		ImGui::Text("Y slice");
		ImGui::SameLine(2 * heatMapSize);
		ImGui::Text("Z slice");

		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesXSlice.force(), ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesYSlice.force(), ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(2 * heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesZSlice.force(), ImVec2(heatMapSize, heatMapSize));


		// RENDER PLANES
		glDisable(GL_CULL_FACE);

		glm::vec3 middle = glm::vec3(xSlice, ySlice, zSlice) / (float)(heatMapSize - 1.0f);

		struct Corners
		{
			glm::vec3 p1, p2, p3, p4;

			glm::vec3 center() { return (p1 + p2 + p3 + p4) / 4.0f; }
		};

		Corners planes[12] = {
			// z plane
			{ 
				middle,
				middle + glm::vec3(1.0f - middle.x, 0.0f, 0.0f),
				middle + glm::vec3(1.0f - middle.x, 1.0f - middle.y, 0.0f),
				middle + glm::vec3(0.0f, 1.0f - middle.y, 0.0f)
			},
			{
				middle + glm::vec3(-middle.x, 0.0f, 0.0f),
				middle,
				middle + glm::vec3(0.0f, 1.0f - middle.y, 0.0f),
				middle + glm::vec3(-middle.x, 1.0f - middle.y, 0.0f)
			},
			{
				middle + glm::vec3(-middle.x, -middle.y, 0.0f),
				middle + glm::vec3(0.0f, -middle.y, 0.0f),
				middle,
				middle + glm::vec3(-middle.x, 0.0f, 0.0f)
			},
			{
				middle + glm::vec3(0.0f, -middle.y, 0.0f),
				middle + glm::vec3(1.0f - middle.x, -middle.y, 0.0f),
				middle + glm::vec3(1.0f - middle.x, 0.0f, 0.0f),
				middle
			},
			// x plane
			{
				middle,
				middle + glm::vec3(0.0f, 1.0f - middle.y, 0.0f),
				middle + glm::vec3(0.0f, 1.0f - middle.y, 1.0f - middle.z),
				middle + glm::vec3(0.0f, 0.0f, 1.0f - middle.z)
			},
			{
				middle + glm::vec3(0.0f, -middle.y, 0.0f),
				middle,
				middle + glm::vec3(0.0f, 0.0f, 1.0f - middle.z),
				middle + glm::vec3(0.0f, -middle.y, 1.0f - middle.z)
			},
			{
				middle + glm::vec3(0.0f, -middle.y, -middle.z),
				middle + glm::vec3(0.0f, 0.0f, -middle.z),
				middle,
				middle + glm::vec3(0.0f, -middle.y, 0.0f)
			},
			{
				middle + glm::vec3(0.0f, 0.0f, -middle.z),
				middle + glm::vec3(0.0f, 1.0f - middle.y, -middle.z),
				middle + glm::vec3(0.0f, 1.0f - middle.y, 0.0f),
				middle
			},
			// y plane
			{
				middle,
				middle + glm::vec3(1.0f - middle.x, 0.0f, 0.0f),
				middle + glm::vec3(1.0f - middle.x, 0.0f, 1.0f - middle.z),
				middle + glm::vec3(0.0f, 0.0f, 1.0f - middle.z)
			},
			{
				middle + glm::vec3(-middle.x, 0.0f, 0.0f),
				middle,
				middle + glm::vec3(0.0f, 0.0f, 1.0f - middle.z),
				middle + glm::vec3(-middle.x, 0.0f, 1.0f - middle.z)
			},
			{
				middle + glm::vec3(-middle.x, 0.0f, -middle.z),
				middle + glm::vec3(0.0f, 0.0f, -middle.z),
				middle,
				middle + glm::vec3(-middle.x, 0.0f, 0.0f)
			},
			{
				middle + glm::vec3(0.0f, 0.0f, -middle.z),
				middle + glm::vec3(1.0f - middle.x, 0.0f, -middle.z),
				middle + glm::vec3(1.0f - middle.x, 0.0f, 0.0f),
				middle
			},
		};

		std::map<float, int> sorted;
		for (int i = 0; i < 12; i++)
		{
			float distance = glm::length(cam.GetEye() - planes[i].center());
			sorted[distance] = i;
		}

		for (std::map<float, int>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			Corners& c = planes[it->second];

			df::Backbuffer << *planeShaderProgram() 
				<< "MVP" << cam.GetViewProj()
				<< "p1" << c.p1
				<< "p2" << c.p2
				<< "p3" << c.p3
				<< "p4" << c.p4
				<< "color" << glm::vec4(0.3f);
			*planeShaderProgram() << df::NoVao(GL_TRIANGLES, 6, 0);
		}

		glEnable(GL_CULL_FACE);
	}

private:
	static inline std::shared_ptr<SaveableOctree> previouslyDrawnHeatmap;

	static inline Lazy<eltecg::ogl::ShaderStorageBuffer> sdfSSBO = Lazy< eltecg::ogl::ShaderStorageBuffer>([]() 
	{
		auto sdfSSBO = eltecg::ogl::ShaderStorageBuffer();
		std::vector<float> sdfValues(ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE, 0.0f);

		sdfSSBO.constructMutable(sdfValues, GL_DYNAMIC_READ);

		return sdfSSBO;
	});


	inline static std::unique_ptr<df::ComputeProgramEditor> _differenceCalculateProgram;
	static std::unique_ptr<df::ComputeProgramEditor>& differenceCalculateProgram()
	{
		if (_differenceCalculateProgram == nullptr)
		{
			_differenceCalculateProgram = std::make_unique<df::ComputeProgramEditor>("HeatmapDiffProgram");

			*_differenceCalculateProgram
				<< "Shaders/defines.glsl"_comp
				<< "Shaders/uniforms.glsl"_comp
				<< "Shaders/Math/common.glsl"_comp
				<< "Shaders/common.glsl"_comp
				<< "Shaders/Octree/octree.glsl"_comp
				<< "Shaders/SDF/sdfOctree.glsl"_comp
				<< "Shaders/Evaluate/sdfDifference.comp"_comp
				<< df::LinkProgram;
		}

		return _differenceCalculateProgram;
	}

	inline static std::unique_ptr<df::ShaderProgramEditorVF> _planeShaderProgram;
	static std::unique_ptr<df::ShaderProgramEditorVF>& planeShaderProgram()
	{
		if (_planeShaderProgram == nullptr)
		{
			_planeShaderProgram = std::make_unique<df::ShaderProgramEditorVF>("HeatmapPlane");

			*_planeShaderProgram
				<< "Shaders/Primitives/plane.vert"_vs
				<< "Shaders/Primitives/plane.frag"_fs
				<< df::LinkProgram;
		}

		return _planeShaderProgram;
	}
	

	inline static Lazy<df::Texture2D<glm::vec4>>
		sdfValuesXSlice = Lazy<df::Texture2D<glm::vec4>>([]() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); }),
		sdfValuesYSlice = Lazy<df::Texture2D<glm::vec4>>([]() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); }),
		sdfValuesZSlice = Lazy<df::Texture2D<glm::vec4>>([]() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); });

	inline static Lazy<df::Texture2D<glm::vec4>> 
		sdfErrorXSlice = Lazy<df::Texture2D<glm::vec4>>( []() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); } ),
		sdfErrorYSlice = Lazy<df::Texture2D<glm::vec4>>( []() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); } ),
		sdfErrorZSlice = Lazy<df::Texture2D<glm::vec4>>( []() { return df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE); } );

	inline static bool renderDiff = false;
	inline static float diffMultiplier = 1.0f;
};

#endif