#include "ErrorStatistics.h"

uint32_t nextPowerOf2(uint32_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

// the input should be sorted
// H = [#{x >= base^0}, #{x >= base^-1}, #{x >= base^-2}, ..., #{x >= base^-max_i}, #{x < base^-max_i}]
std::vector<int64_t> CalcHistogram(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end, float base, int max_i)
{
	std::vector<int64_t> H(max_i + 2);
	const float oneOverBase = 1 / base;
	float pivot = 1.0f;
	for (int i = 0; i <= max_i; ++i, pivot *= oneOverBase) {
		auto it = std::lower_bound(begin, end, pivot);
		H[i] = int64_t(end - it);
		end = it;
	}
	H.back() = end - begin;
	return H;
}

ErrorStatistics::ErrorStatistics()
{
	initDiff1Prog << "Shaders/Evaluate/initDiff1.comp"_comp << df::LinkProgram;
	std::cout << initDiff1Prog.GetErrors();
	reduce1Prog << "Shaders/Evaluate/reduce1.comp"_comp << df::LinkProgram;
	std::cout << reduce1Prog.GetErrors();
	initDiff2Prog << "Shaders/Evaluate/initDiff2.comp"_comp << df::LinkProgram;
	std::cout << initDiff2Prog.GetErrors();
	reduce2Prog << "Shaders/Evaluate/reduce2.comp"_comp << df::LinkProgram;
	std::cout << reduce2Prog.GetErrors();
}

ErrorStatistics::ErrorStatResult ErrorStatistics::CalcStatistics(std::shared_ptr<df::Texture2DArray<float>> ref, std::shared_ptr<df::Texture2DArray<float>> data, const StatSettings& settings)
{
	auto& border = settings.border;
	const glm::ivec3 groupSize = glm::ivec3(32, 1, 1);
	constexpr GLenum storeFormatFloat = df::detail::getInternalFormat<float>();
	constexpr GLenum storeFormatVec2 = df::detail::getInternalFormat<glm::vec2>();
	constexpr GLenum storeFormatVec4 = df::detail::getInternalFormat<glm::vec4>();

	glm::ivec3 refDim = glm::ivec3(ref->getWidth(), ref->getHeight(), ref->getLayers());
	glm::ivec3 dataDim = glm::ivec3(data->getWidth(), data->getHeight(), data->getLayers());
	if (refDim != dataDim) {
		std::cerr << "[ErrorStatistics] reference and data size do not match:\n";
		std::cerr << "ref: [" << refDim.x << ", " << refDim.y << ", " << refDim.z
			<< "], data: [" << dataDim.x << ", " << dataDim.y << ", " << dataDim.z << "]\n";
		return {};
	}
	const glm::uvec3 dd = glm::uvec3(nextPowerOf2(refDim.x), nextPowerOf2(refDim.y), nextPowerOf2(refDim.z));
	const uint32_t sDim = glm::max(dd.x, glm::max(dd.y, dd.z));
	const glm::ivec3 dim = glm::ivec3(sDim);

	// Init pass 1: ref + data ->  stats  ( abs err, sign, --, coverage )
	df::Texture3D<glm::vec4> stats = df::Texture3D<glm::vec4>(sDim, sDim, sDim, df::ALL);
	// set uniforms
	initDiff1Prog
		<< "gIdOffset" << glm::ivec3(0)
		<< "maxDim" << dim
		<< "insideMin" << border
		<< "insideMax" << refDim - border;
	// bind img textures
	glBindImageTexture(0, (GLuint)ref->operator GLuint(), 0, GL_TRUE, 0, GL_READ_ONLY, storeFormatFloat);
	glBindImageTexture(1, (GLuint)data->operator GLuint(), 0, GL_TRUE, 0, GL_READ_ONLY, storeFormatFloat);
	glBindImageTexture(2, (GLuint)stats, 0, GL_TRUE, 0, GL_WRITE_ONLY, storeFormatVec4);
	// dispatch
	glm::uvec3 inv = (dim + groupSize - 1) / groupSize;
	glDispatchCompute(inv.x, inv.y, inv.z);
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

	// Reduce passes 1: stats -> stats  ( mean, sign, --, coverage )
	uint32_t baseLevel = 0;
	for (glm::ivec3 baseDim = dim / 2; baseDim.x > 0; baseDim /= 2, baseLevel++) {
		// set uniforms
		reduce1Prog
			<< "gIdOffset" << glm::ivec3(0)
			<< "maxDim" << baseDim;
		// bind img textures
		glBindImageTexture(0, (GLuint)stats, baseLevel, GL_TRUE, 0, GL_READ_ONLY, storeFormatVec4);
		glBindImageTexture(1, (GLuint)stats, baseLevel + 1, GL_TRUE, 0, GL_WRITE_ONLY, storeFormatVec4);
		// dispatch
		glm::uvec3 inv = (baseDim + groupSize - 1) / groupSize;
		glDispatchCompute(inv.x, inv.y, inv.z);
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
	}

	// Read results
	glFinish();
	stats.bind();
	float buf[4];
	glGetnTexImage(GL_TEXTURE_3D, baseLevel, GL_RGBA, GL_FLOAT, 4 * sizeof(float), buf);

	ErrorStatResult ret;
	ret.mean = buf[0];
	ret.sign = buf[1];
	ret.sampNum = int64_t(double(buf[3]) * ((int64_t)sDim * sDim * sDim)); // this is only approx. if sDim is too big

	// Init pass 2: stats ->  stats2  ( (err - mean)^2, coverage )
	df::Texture3D<glm::vec2> stats2 = df::Texture3D<glm::vec2>(sDim, sDim, sDim, df::ALL);
	// set uniforms
	initDiff2Prog
		<< "gIdOffset" << glm::ivec3(0)
		<< "maxDim" << dim
		<< "mean" << ret.mean;
	// bind img textures
	glBindImageTexture(0, (GLuint)stats, 0, GL_TRUE, 0, GL_READ_ONLY, storeFormatVec4);
	glBindImageTexture(1, (GLuint)stats2, 0, GL_TRUE, 0, GL_WRITE_ONLY, storeFormatVec2);
	// dispatch
	inv = (dim + groupSize - 1) / groupSize;
	glDispatchCompute(inv.x, inv.y, inv.z);
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

	// Reduce passes 2: stats -> stats  ( SD^2, coverage )
	baseLevel = 0;
	for (glm::ivec3 baseDim = dim / 2; baseDim.x > 0; baseDim /= 2u, baseLevel++) {
		// set uniforms
		reduce2Prog
			<< "gIdOffset" << glm::ivec3(0)
			<< "maxDim" << baseDim;
		// bind img textures
		glBindImageTexture(0, (GLuint)stats2, baseLevel, GL_TRUE, 0, GL_READ_ONLY, storeFormatVec2);
		glBindImageTexture(1, (GLuint)stats2, baseLevel + 1, GL_TRUE, 0, GL_WRITE_ONLY, storeFormatVec2);
		// dispatch
		glm::uvec3 inv = (baseDim + groupSize - 1) / groupSize;
		glDispatchCompute(inv.x, inv.y, inv.z);
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
	}

	// Read results
	glFinish();
	stats2.bind();
	glGetnTexImage(GL_TEXTURE_3D, baseLevel, GL_RG, GL_FLOAT, 2 * sizeof(float), &buf);
	ret.sd = sqrt(buf[0]);

	// calc median and histogram
	if (settings.calcMedian || settings.calcHistogram) {
		stats.bind();
		std::vector<float> buffer((uint64_t)sDim * sDim * sDim);
		glGetnTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, sDim * sDim * sDim * sizeof(float), buffer.data());
		std::sort(buffer.begin(), buffer.end());
		auto first = std::lower_bound(buffer.begin(), buffer.end(), 0);
		int64_t num_ignored = first - buffer.begin();
		int64_t num_seq = (int64_t)sDim * sDim * sDim - num_ignored;
		ret.median = num_seq % 2 == 1 ? buffer[num_ignored + num_seq / 2] : (buffer[num_ignored + num_seq / 2 - 1] + buffer[num_ignored + num_seq / 2]) / 2;
		ret.min = buffer[num_ignored];
		ret.max = buffer.back();
		ret.sampNum = num_seq;

		ret.l2 = 0.0f;
		ret.rms = 0.0f;
		for ( size_t i = num_ignored; i < buffer.size(); ++i )
		{
			float& x = buffer[ i ];
			ret.l2 += x * x;
		}
		ret.l2 = std::sqrt( ret.l2 );
		ret.rms = ret.l2 / std::sqrt( num_seq );

		if (settings.calcHistogram)
			ret.histogram = CalcHistogram(first, buffer.end(), 10, 10);
	}
	return ret;
}

glm::ivec3 ErrorStatistics::GetBorder(glm::ivec3 refDim, glm::ivec3 origDim)
{
	return glm::ivec3(glm::ceil(glm::vec3(refDim) / 2.f / glm::vec3(origDim)));
}

void ErrorStatistics::ErrorStatResult::Print() const
{
	bool writeHist = !histogram.empty();
	std::cout << "\n========\n mean: " << mean
		<< "\n sd:   " << sd
		<< "\n sign: " << sign
		<< "\n medi: " << median
		<< "\n min:  " << min
		<< "\n max:  " << max
		<< "\n samp: " << sampNum
		<< "\n L2:   " << l2
		<< "\n RMS:  " << rms;
	if (writeHist) {
		std::cout << "\n Histogram: >=1, >=1/10, >=1/100...\n";
		for (auto n : histogram) std::cout << n << ",";
	}
	std::cout << "\n========\n";
}
