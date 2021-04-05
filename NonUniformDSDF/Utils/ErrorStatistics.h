#ifndef ERRORSTATISTICS_H
#define ERRORSTATISTICS_H

#include <glm/glm.hpp>
#include <vector>
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/Texture/Texture2DArray.h>
#include <Dragonfly/detail/Texture/Texture3D.h>

class ErrorStatistics
{
public:
	struct ErrorStatResult {
		float mean   = -1;
		float sd     = -1;
		float sign   = -1;
		float median = -1;
		float min    = -1;
		float max    = -1;
		float l2 = -1.0f;
		float rms = -1.0f;
		int64_t sampNum = -1;
		std::vector<int64_t> histogram;
		void Print() const;
	};

	struct StatSettings {
		glm::ivec3 border = glm::ivec3(0); // exclude samples form the calculations near the border
		bool calcMedian = false; // median calculation is expensive, it needs to sort all values
		bool calcHistogram = false; // histogram needs sorting as well (calcMedian is implied)
	};

	ErrorStatistics();
	~ErrorStatistics() = default;
	
	ErrorStatResult CalcStatistics(std::shared_ptr<df::Texture2DArray<float>> ref, std::shared_ptr<df::Texture2DArray<float>> data, const StatSettings& settings);
	static glm::ivec3 GetBorder(glm::ivec3 refDim, glm::ivec3 origDim);

protected:
	df::ComputeProgramEditor initDiff1Prog = "Init-Diff1-Prog";
	df::ComputeProgramEditor reduce1Prog = "Reduce1-Prog";
	df::ComputeProgramEditor initDiff2Prog = "Init-Diff2-Prog";
	df::ComputeProgramEditor reduce2Prog = "Reduce2-Prog";
};

#endif