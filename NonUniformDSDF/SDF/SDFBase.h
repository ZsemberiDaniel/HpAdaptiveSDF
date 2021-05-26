#ifndef SDFBASE_H
#define SDFBASE_H
#include <glm/vec3.hpp>
#include "../vector3d.h"
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/buffer.h> //will be replaced
#include <Dragonfly/detail/Texture/Texture3D.h>
#include <Dragonfly/detail/Texture/Texture2DArray.h>
#include "../constants.h"

/// <summary>
/// A base class for defining SDFs that have define directives and helper textures for the GUI.
/// </summary>
class SDFBase
{
private:
	/// <summary>
	/// Max SDF value stored in discreteSDFTexture and discreteSDFTexture2D.
	/// </summary>
	float maxSDFValue = -std::numeric_limits<float>::max();

	bool discreteSDFInitialized = false;
	/// <summary>
	/// A 3D texture that stores raw SDF values. Used for showing heatmaps in the GUI.
	/// Size is ERROR_HEATMAP_SIZE.
	/// </summary>
	std::shared_ptr<df::Texture3D<float>> discreteSDFTexture = nullptr;
	/// <summary>
	/// A 2D texture array that stores raw SDF values. Used for calculating errors
	/// Size is ERROR_HEATMAP_SIZE.
	/// </summary>
	std::shared_ptr<df::Texture2DArray<float>> discreteSDFTexture2D = nullptr;

protected:
	/// <summary>
	/// The min pos of the SDF in the world. Does not work currently.
	/// </summary>
	glm::vec3 _worldMinPos;
	/// <summary>
	/// The size of the SDF in the world. Does not work currently.
	/// </summary>
	float _worldSize;
	/// <summary>
	/// The path to this file.
	/// </summary>
	std::string _path;

	std::string _sdfName = "Default SDF name";

	virtual void writeAttributes(std::ofstream& out) const
	{
		/*for (auto const& define : defines)
		{
			out << "#define " << define.first << " " << define.second << "\n";
		}*/
	}
public:
	/// <summary>
	/// Initializes an SDF.
	/// </summary>
	/// <param name="sdfName">Name of the SDF. Should be unique for the UI to work properly.</param>
	/// <param name="path">Path to the file/header that implements the SDF.</param>
	/// <param name="worldMinPos">Does not work currently. The min pos of the SDF in the world.</param>
	/// <param name="worldSize">Does not work currently. The size of the SDF in the world.</param>
	SDFBase(std::string sdfName, std::string path, glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		_sdfName(sdfName), _worldMinPos(worldMinPos), _worldSize(worldSize) , _path(path)
	{
		
	}
	/// <summary>
	/// Evaluates the SDF at the given point
	/// </summary>
	virtual float operator()(glm::vec3 p) const { return 0.0f; }
	/// <summary>
	/// Renders settings of this SDF to the GUI via ImGUI.
	/// </summary>
	virtual void renderGUI()
	{
		// ImGui::InputFloat3("World min pos", &_worldMinPos.x, 1);
		// ImGui::InputFloat("World size", &_worldSize, 1);
	}
	virtual ~SDFBase() 
	{	
		
	}

	/// <summary>
	/// Writes attributes of this sdf to the given file as shader macros. Overwrites the given file.
	/// </summary>
	/// <param name="path">A relative path to the desired directive file.</param>
	void writeAttributesToDefinesFile(const std::string& path) const
	{
		std::ofstream out(path);
		if (!out.is_open())
		{
			std::cout << "Could not open attributes defines file!" << std::endl;
		}
		writeAttributes(out);

		out.close();
	}

	/// <summary>
	/// Initializes the discreteSDFTexture and discreteSDFTexture2D.
	/// </summary>
	void initializeSDFDiscreteValues()
	{
		std::vector<float> temp(ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE);
		float heatMapSize = (float)ERROR_HEATMAP_SIZE;
		for (int x = 0; x < ERROR_HEATMAP_SIZE; x++)
		{
			for (int y = 0; y < ERROR_HEATMAP_SIZE; y++)
			{
				for (int z = 0; z < ERROR_HEATMAP_SIZE; z++)
				{
					float val = this->operator()(glm::vec3(_worldSize) * glm::vec3(x, y, z) / heatMapSize);
					temp[ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE * x + ERROR_HEATMAP_SIZE * y + z] = val;

					if (glm::abs(val) > maxSDFValue) maxSDFValue = glm::abs(val);
				}
			}
		}

		discreteSDFInitialized = true;

		discreteSDFTexture = std::make_shared<df::Texture3D<float>>(static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE));
		discreteSDFTexture->LoadData(temp);

		discreteSDFTexture2D = std::make_shared<df::Texture2DArray<float>>(static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE));
		discreteSDFTexture2D->LoadData(temp);
		GL_CHECK;
	}

	glm::vec3 worldMinPos() const { return _worldMinPos; }
	float worldSize() const { return _worldSize; }
	std::string name() const { return _sdfName; }
	const std::string& path() const { return _path; }

	/// <summary>
	/// Initializes if not initialized and returns a 3D texture of size ERROR_HEATMAP_SIZE that store SDF values
	/// in a uniform grid from this SDF.
	/// </summary>
	std::shared_ptr<df::Texture3D<float>> discreteSDFValuesTexture()
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return discreteSDFTexture;
	}

	/// <summary>
	/// Initializes if not initialized and returns a 2D texture array of size ERROR_HEATMAP_SIZE that store SDF values
	/// in a uniform grid from this SDF.
	/// </summary>
	std::shared_ptr<df::Texture2DArray<float>> discreteSDFValuesTexture2D()
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return discreteSDFTexture2D;
	}

	/// <summary>
	/// Initializes if not initialized and returns the maximum of a texture of size ERROR_HEATMAP_SIZE that store SDF values
	/// in a uniform grid from this SDF.
	/// </summary>
	float discreteMaxSDFValue() 
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return maxSDFValue;
	}
};

namespace writeDefinesOperators
{
	inline std::ostream& operator<<(std::ostream& os, const glm::vec3 p)
	{
		os << "vec3(" << p.x << ", " << p.y << ", " << p.z << ")";
		return os;
	}
}
#endif
