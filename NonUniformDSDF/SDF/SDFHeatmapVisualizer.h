#ifndef SDFHEATMAPVISUALIZER_H
#define SDFHEATMAPVISUALIZER_H

#include <sstream>
#include <memory>
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/vao.h>	 //will be replaced

#include "SDF/SDFBase.h"
#include "../constants.h"

class SDFHeatmapVisualizer
{
public:

	SDFHeatmapVisualizer(std::shared_ptr<SaveableOctree> saveableOctree_) :
		saveableOctree(saveableOctree_)
	{
		std::stringstream ss;
		ss << saveableOctree->sdfFunction()->name() << "-diff";
		differenceCalculateProgram = new df::ComputeProgramEditor(ss.str());

		*differenceCalculateProgram
			<< "Shaders/defines.glsl"_comp
			<< "Shaders/uniforms.glsl"_comp
			<< "Shaders/Math/common.glsl"_comp
			<< "Shaders/common.glsl"_comp
			<< "Shaders/Octree/octree.glsl"_comp
			<< "Shaders/SDF/sdfOctree.glsl"_comp
			<< "Shaders/Evaluate/sdfDifference.comp"_comp
			<< df::LinkProgram;


		sdfSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
		sdfSSBO->constructImmutable(saveableOctree->sdfFunction()->discreteSDFValues()->innerVector(), eltecg::ogl::BufferFlags::MAP_READ_BIT);

		sdfValuesXSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);
		sdfValuesYSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);
		sdfValuesZSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);

		sdfErrorXSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);
		sdfErrorYSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);
		sdfErrorZSlice = df::Texture2D<glm::vec4>(ERROR_HEATMAP_SIZE, ERROR_HEATMAP_SIZE);

		GL_CHECK;
	}

	~SDFHeatmapVisualizer()
	{
		delete differenceCalculateProgram;
	}

	void renderToGUI(int xSlice, int ySlice, int zSlice)
	{
		int heatMapSize = ERROR_HEATMAP_SIZE;

		glBindImageTexture(0, (GLuint)sdfValuesXSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, (GLuint)sdfValuesYSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, (GLuint)sdfValuesZSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glBindImageTexture(3, (GLuint)sdfErrorXSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(4, (GLuint)sdfErrorYSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(5, (GLuint)sdfErrorZSlice, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		sdfSSBO->bindBufferRange(3);

		saveableOctree->SetOctreeUniforms(*differenceCalculateProgram);
		*differenceCalculateProgram
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

		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorXSlice, ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorYSlice, ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(2 * heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfErrorZSlice, ImVec2(heatMapSize, heatMapSize));


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

		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesXSlice, ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesYSlice, ImVec2(heatMapSize, heatMapSize));
		ImGui::SameLine(2 * heatMapSize);
		ImGui::Image((void*)(intptr_t)(GLuint)sdfValuesZSlice, ImVec2(heatMapSize, heatMapSize));
	}

private:
	std::shared_ptr<SaveableOctree> saveableOctree;

	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> sdfSSBO;
	df::ComputeProgramEditor* differenceCalculateProgram;

	df::Texture2D<glm::vec4> sdfValuesXSlice, sdfValuesYSlice, sdfValuesZSlice;
	df::Texture2D<glm::vec4> sdfErrorXSlice, sdfErrorYSlice, sdfErrorZSlice;

	bool renderDiff = false;
	float diffMultiplier = 1.0f;
};

#endif
