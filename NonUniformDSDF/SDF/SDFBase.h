#pragma once
#include <glm/vec3.hpp>

class SDFBase
{
public:
	SDFBase(std::string sdfName, glm::vec3 worldMinPos = glm::vec3(0), glm::vec3 worldSize = glm::vec3(1)) :
		_sdfName(sdfName), _worldMinPos(worldMinPos), _worldSize(worldSize) {}
	virtual float operator()(glm::vec3 p) const { return 0.0f; }
	virtual void renderGUI() { }
	virtual ~SDFBase() = default;

	glm::vec3 worldMinPos() const { return _worldMinPos; }
	glm::vec3 worldSize() const { return _worldSize; }
	std::string name() const { return _sdfName; }

protected:
	glm::vec3 _worldMinPos;
	glm::vec3 _worldSize;

	std::string _sdfName = "Default SDF name";
};
