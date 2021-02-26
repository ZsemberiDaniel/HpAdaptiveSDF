#include "App.h"

#include <glm/gtx/transform.hpp>

#include "OctreeConverter.h"

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

const double gaussPoints1D[20][20] = {
	{0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{-0.577350269189626,0.577350269189626,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,-0.774596669241483,0.774596669241483,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{-0.339981043584856,-0.861136311594053,0.339981043584856,0.861136311594053,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,-0.538469310105683,-0.906179845938664,0.538469310105683,0.906179845938664,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.238619186083197,0.661209386466264,0.932469514203152,-0.238619186083197,-0.661209386466264,-0.932469514203152,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,0.405845151377397,0.741531185599394,0.949107912342758,-0.405845151377397,-0.741531185599394,-0.949107912342758,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{-0.183434642495650,-0.525532409916329,-0.796666477413627,-0.960289856497536,0.183434642495650,0.525532409916329,0.796666477413627,0.960289856497536,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,-0.324253423403809,-0.613371432700590,-0.836031107326636,-0.968160239507626,0.324253423403809,0.613371432700590,0.836031107326636,0.968160239507626,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{-0.148874338981631,-0.433395394129247,-0.679409568299024,-0.865063366688985,-0.973906528517172,0.148874338981631,0.433395394129247,0.679409568299024,0.865063366688985,0.973906528517172,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,-0.269543155952345,-0.519096129206812,-0.730152005574049,-0.887062599768095,-0.978228658146057,0.269543155952345,0.519096129206812,0.730152005574049,0.887062599768095,0.978228658146057,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.125233408511469,0.367831498998180,0.587317954286617,0.769902674194305,0.904117256370475,0.981560634246719,-0.125233408511469,-0.367831498998180,-0.587317954286617,-0.769902674194305,-0.904117256370475,-0.981560634246719,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,0.230458315955135,0.448492751036447,0.642349339440340,0.801578090733310,0.917598399222978,0.984183054718588,-0.230458315955135,-0.448492751036447,-0.642349339440340,-0.801578090733310,-0.917598399222978,-0.984183054718588,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.108054948707344,0.319112368927890,0.515248636358154,0.687292904811685,0.827201315069765,0.928434883663574,0.986283808696812,-0.108054948707344,-0.319112368927890,-0.515248636358154,-0.687292904811685,-0.827201315069765,-0.928434883663574,-0.986283808696812,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,0.201194093997435,0.394151347077563,0.570972172608539,0.724417731360170,0.848206583410427,0.937273392400706,0.987992518020485,-0.201194093997435,-0.394151347077563,-0.570972172608539,-0.724417731360170,-0.848206583410427,-0.937273392400706,-0.987992518020485,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.095012509837637,0.281603550779259,0.458016777657227,0.617876244402644,0.755404408355003,0.865631202387832,0.944575023073233,0.989400934991650,-0.095012509837637,-0.281603550779259,-0.458016777657227,-0.617876244402644,-0.755404408355003,-0.865631202387832,-0.944575023073233,-0.989400934991650,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.000000000000000,0.178484181495848,0.351231763453876,0.512690537086477,0.657671159216691,0.781514003896801,0.880239153726986,0.950675521768768,0.990575475314417,-0.178484181495848,-0.351231763453876,-0.512690537086477,-0.657671159216691,-0.781514003896801,-0.880239153726986,-0.950675521768768,-0.990575475314417,0.000000000000000,0.000000000000000,0.000000000000000},
	{-0.084775013041735,-0.251886225691505,-0.411751161462843,-0.559770831073948,-0.691687043060353,-0.803704958972523,-0.892602466497556,-0.955823949571398,-0.991565168420931,0.084775013041735,0.251886225691505,0.411751161462843,0.559770831073948,0.691687043060353,0.803704958972523,0.892602466497556,0.955823949571398,0.991565168420931,0.000000000000000,0.000000000000000},
	{0.000000000000000,-0.160358645640225,-0.316564099963630,-0.464570741375961,-0.600545304661681,-0.720966177335229,-0.822714656537143,-0.903155903614818,-0.960208152134830,-0.992406843843584,0.160358645640225,0.316564099963630,0.464570741375961,0.600545304661681,0.720966177335229,0.822714656537143,0.903155903614818,0.960208152134830,0.992406843843584,0.000000000000000},
	{0.076526521133497,0.227785851141645,0.373706088715420,0.510867001950827,0.636053680726515,0.746331906460151,0.839116971822219,0.912234428251326,0.963971927277914,0.993128599185095,-0.076526521133497,-0.227785851141645,-0.373706088715420,-0.510867001950827,-0.636053680726515,-0.746331906460151,-0.839116971822219,-0.912234428251326,-0.963971927277914,-0.993128599185095}
};

const double gaussCoeffs1D[20][20] = {
	{2.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{1.000000000000000,1.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.888888888888889,0.555555555555556,0.555555555555556,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.652145154862546,0.347854845137454,0.652145154862546,0.347854845137454,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.568888888888889,0.478628670499366,0.236926885056189,0.478628670499366,0.236926885056189,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.467913934572691,0.360761573048139,0.171324492379170,0.467913934572691,0.360761573048139,0.171324492379170,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.417959183673469,0.381830050505119,0.279705391489277,0.129484966168870,0.381830050505119,0.279705391489277,0.129484966168870,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.362683783378362,0.313706645877887,0.222381034453375,0.101228536290376,0.362683783378362,0.313706645877887,0.222381034453375,0.101228536290376,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.330239355001260,0.312347077040003,0.260610696402935,0.180648160694857,0.081274388361574,0.312347077040003,0.260610696402935,0.180648160694857,0.081274388361574,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.295524224714753,0.269266719309996,0.219086362515982,0.149451349150581,0.066671344308688,0.295524224714753,0.269266719309996,0.219086362515982,0.149451349150581,0.066671344308688,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.272925086777901,0.262804544510247,0.233193764591990,0.186290210927734,0.125580369464905,0.055668567116174,0.262804544510247,0.233193764591990,0.186290210927734,0.125580369464905,0.055668567116174,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.249147045813403,0.233492536538355,0.203167426723066,0.160078328543346,0.106939325995318,0.047175336386512,0.249147045813403,0.233492536538355,0.203167426723066,0.160078328543346,0.106939325995318,0.047175336386512,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.232551553230874,0.226283180262897,0.207816047536889,0.178145980761946,0.138873510219787,0.092121499837728,0.040484004765316,0.226283180262897,0.207816047536889,0.178145980761946,0.138873510219787,0.092121499837728,0.040484004765316,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.215263853463158,0.205198463721296,0.185538397477938,0.157203167158194,0.121518570687903,0.080158087159760,0.035119460331752,0.215263853463158,0.205198463721296,0.185538397477938,0.157203167158194,0.121518570687903,0.080158087159760,0.035119460331752,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.202578241925561,0.198431485327112,0.186161000015562,0.166269205816994,0.139570677926154,0.107159220467172,0.070366047488108,0.030753241996117,0.198431485327112,0.186161000015562,0.166269205816994,0.139570677926154,0.107159220467172,0.070366047488108,0.030753241996117,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.189450610455068,0.182603415044922,0.169156519395003,0.149595988816577,0.124628971255534,0.095158511682492,0.062253523938648,0.027152459411754,0.189450610455068,0.182603415044922,0.169156519395003,0.149595988816577,0.124628971255534,0.095158511682492,0.062253523938648,0.027152459411754,0.000000000000000,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.179446470356207,0.176562705366993,0.168004102156450,0.154045761076810,0.135136368468525,0.111883847193404,0.085036148317179,0.055459529373987,0.024148302868548,0.176562705366993,0.168004102156450,0.154045761076810,0.135136368468525,0.111883847193404,0.085036148317179,0.055459529373987,0.024148302868548,0.000000000000000,0.000000000000000,0.000000000000000},
	{0.169142382963144,0.164276483745833,0.154684675126265,0.140642914670651,0.122555206711479,0.100942044106287,0.076425730254889,0.049714548894970,0.021616013526483,0.169142382963144,0.164276483745833,0.154684675126265,0.140642914670651,0.122555206711479,0.100942044106287,0.076425730254889,0.049714548894970,0.021616013526483,0.000000000000000,0.000000000000000},
	{0.161054449848784,0.158968843393954,0.152766042065860,0.142606702173607,0.128753962539336,0.111566645547334,0.091490021622450,0.069044542737641,0.044814226765700,0.019461788229727,0.158968843393954,0.152766042065860,0.142606702173607,0.128753962539336,0.111566645547334,0.091490021622450,0.069044542737641,0.044814226765700,0.019461788229727,0.000000000000000},
	{0.152753387130726,0.149172986472604,0.142096109318382,0.131688638449177,0.118194531961518,0.101930119817240,0.083276741576705,0.062672048334109,0.040601429800387,0.017614007139152,0.152753387130726,0.149172986472604,0.142096109318382,0.131688638449177,0.118194531961518,0.101930119817240,0.083276741576705,0.062672048334109,0.040601429800387,0.017614007139152}

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
	// return glm::abs(dot(p - glm::vec3(0, 0.5f, 0), glm::normalize(glm::vec3(0, 1, 0))));

	return sdfBox(p, glm::vec3(1.0f, 0.3f, 1.0f));

	// 2odfok

	// torus
	// p -= glm::vec3(1);
	/*const float R = 0.8f;
	const float r = 0.4f;
	glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x, p.z)) - R, p.y);
	return length(q) - r;*/
}

/* Returns points and coefficients for Gaussian-Legendre quadratics
*/
GaussCoeffPoints gaussPoints(int pointCount)
{
	GaussCoeffPoints output;
	if (pointCount > 20 || pointCount <= 0)
	{
		std::cerr << "Point count for gaussian quadratic is maximum 20 and minimum 1!" << std::endl;
		return output;
	}

	output.points.resize(pointCount * pointCount * pointCount);
	output.coeffs.resize(pointCount * pointCount * pointCount);

	for (int i = 0, m = 0; i < pointCount; i++)
	{
		for (int k = 0; k < pointCount; k++)
		{
			for (int j = 0; j < pointCount; j++, m++)
			{
				output.coeffs[m] = gaussCoeffs1D[pointCount - 1][i] * 
								   gaussCoeffs1D[pointCount - 1][k] *
								   gaussCoeffs1D[pointCount - 1][j];
				output.points[m] = glm::vec3(gaussPoints1D[pointCount - 1][i],
											 gaussPoints1D[pointCount - 1][k],
											 gaussPoints1D[pointCount - 1][j]);
			}
		}
	}

	return output;
}

/* Evaluates the Gaussian-Legendre quadratics for the given R^3 -> R function
*/
template<typename F>
float evaluateGaussQuadratic(const GaussCoeffPoints& gcp, F function)
{
	float value = 0.0f;
	for (int i = 0; i < gcp.coeffs.size(); i++)
	{
		value += gcp.coeffs[i] * function(gcp.points[i]);
	}

	return value;
}

void testGaussPoints() 
{
	GaussCoeffPoints gcp = gaussPoints(15);

	auto eval = [&gcp](std::function<float(glm::vec3)> fun, float realVal) 
	{
		float value = 0.0f;
		for (int i = 0; i < gcp.coeffs.size(); i++)
		{
			value += gcp.coeffs[i] * fun(gcp.points[i]);
		}
		std::cout << "Calculated by gaussian: " << value << "\tReal: " << realVal << std::endl;
	};
	
	// x^2 + y^2 + z^2
	eval([](glm::vec3 x) { return glm::dot(x, x); }, 8);

	// sin(x^2) + y + z^3
	eval([](glm::vec3 p) { return sinf(p.x * p.x) + p.y + p.z * p.z * p.z; }, 2.48215);

	// sphere in (1, 1, 1) with radius 1
	eval([](glm::vec3 p) { return glm::length(p - glm::vec3(1)) - 1; }, 7.3694);
}

float legendref(int deg, float p)
{
	if (deg == 0) return 1;
	else if (deg == 1) return p;

	float a = 1;
	float b = p;

	for (int at = 2; at <= deg; at++)
	{
		float c = (1.0f / at) * ((2.0f * at - 1.0f) * p * b - (at - 1.0f) * a);
		a = b; b = c;
	}

	return b;
}

/* Equation 4
*/
float shiftedNormalizedLegendre(const BoundingBox& bbox, glm::ivec3 degrees, glm::vec3 p)
{
	glm::vec3 shiftedNormalizedP = 2.0f / glm::vec3(bbox.max - bbox.min) * p 
							       - glm::vec3((bbox.max + bbox.min) / (bbox.max - bbox.min));
	glm::vec3 shiftedNormalizedCoeff = (2.0f * glm::vec3(degrees) + 1.0f) / glm::vec3(bbox.size());

	float value = (shiftedNormalizedCoeff.x) * legendref(degrees.x, shiftedNormalizedP.x);
	value *= (shiftedNormalizedCoeff.y) * legendref(degrees.y, shiftedNormalizedP.y);
	value *= (shiftedNormalizedCoeff.z) * legendref(degrees.z, shiftedNormalizedP.z);

	return value;
}

int getCoeffCount(int degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}

/* Equation 5
*/
Polynomial fitPolynomial(const BoundingBox& bbox, int degree)
{
	Polynomial polynomial;
	// taken this from the paper: n_c, section 3.3
	polynomial.coeffs.resize(getCoeffCount(degree));
	polynomial.degree = degree;

	GaussCoeffPoints gcp = gaussPoints(4 * degree);

	int m = 0;
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				polynomial.coeffs[m] = 0.0;
				
				for (int n = 0; n < gcp.coeffs.size(); n++)
				{
					// point inside the bounding box
					glm::vec3 p = bbox.min + (gcp.points[n] * 0.5f + 0.5f) * bbox.size();

					// doing quadrature from bbox.min to bbox.max
					polynomial.coeffs[m] += bbox.size().x * bbox.size().y * bbox.size().z / 8.0f * 
						gcp.coeffs[n] *
						(SDF(p) * shiftedNormalizedLegendre(bbox, glm::ivec3(i, k, j), p));
				}
			}
		}
	}

	return polynomial;
}

void printPolynomial(const Polynomial& poly)
{
	int m = 0;
	std::cout << std::setprecision(3);
	for (int i = 0; i <= poly.degree; i++)
	{
		for (int k = 0; i + k <= poly.degree; k++)
		{
			for (int j = 0; i + k + j <= poly.degree; j++, m++)
			{
				if (glm::abs(poly.coeffs[m]) < 0.0001f) continue;
				
				std::cout << (i + k + j == 0 ? "" : " + ") << poly.coeffs[m];
				if (i != 0 && i != 1) std::cout << " * Lx^" << i;
				if (i == 1) std::cout << " * Lx";
				if (k != 0 && k != 1) std::cout << " * Ly^" << k;
				if (k == 1) std::cout << " * Ly";
				if (j != 0 && j != 1) std::cout << " * Lz^" << j;
				if (j == 1) std::cout << " * Lz";
			}
		}
	}

	std::cout << std::endl;
}

void printOctree(Octree<Cell>::Node* node, BoundingBox currBox)
{
	if (node->type() == Octree<Cell>::LeafNode)
	{
		std::cout << "Bbox " << currBox.min.x << " " << currBox.min.y << " " << currBox.min.z << " ---> "
			      << currBox.max.x << " " << currBox.max.y << " " << currBox.max.z << std::endl;
		printPolynomial(static_cast<Octree<Cell>::Leaf*>(node)->value().poly);
		std::cout << std::endl;
	}
	else if (node->type() == Octree<Cell>::BranchNode)
	{
		auto* branch = static_cast<Octree<Cell>::Branch*>(node);

		for (int z = 0; z <= 1; z++)
		{
			for (int y = 0; y <= 1; y++)
			{
				for (int x = 0; x <= 1; x++)
				{
					int index = z * 4 + y * 2 + x;

					assert(branch->child(x, y, z) == branch->child(index));
					glm::vec3 bboxSizeHalf = currBox.size() / 2.0f;
					
					BoundingBox newBox;
					newBox.min = glm::vec3(
						currBox.min.x + (x == 0 ? 0 : bboxSizeHalf.x),
						currBox.min.y + (y == 0 ? 0 : bboxSizeHalf.y),
						currBox.min.z + (z == 0 ? 0 : bboxSizeHalf.z)
					);
					newBox.max = glm::vec3(
						currBox.max.x - (x == 1 ? 0 : bboxSizeHalf.x),
						currBox.max.y - (y == 1 ? 0 : bboxSizeHalf.y),
						currBox.max.z - (z == 1 ? 0 : bboxSizeHalf.z)
					);

					printOctree(branch->child(x, y, z), newBox);
				}
			}
		}
	}
}

float estimateError(const Polynomial& polynomial)
{
	float error = 0.0;

	// only the coefficients where i + j + k = p
	for (int i = 0, m = 0; i <= polynomial.degree; i++)
	{
		for (int k = 0; i + k <= polynomial.degree; k++)
		{
			for (int j = 0; i + k + j <= polynomial.degree; j++, m++)
			{
				if (i + j + k == polynomial.degree)
				{
					error += polynomial.coeffs[m] * polynomial.coeffs[m];
				}
			}
		}
	}

	return error;
}

void App::constructField(Grid& grid, int maxDegree, int maxLevel, float errorThreshold)
{
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
				float currentError = estimateError(poly);

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
			pImprovementError = estimateError(pImprovementPoly);

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
						float currentError = estimateError(poly);
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
	testGaussPoints();

	sam.AddHandlerClass(state.cam);

	// testGaussPoints();
	constructField(octreeGrid);
	BoundingBox bbox;
	bbox.min = octreeGrid.minPos;
	bbox.max = octreeGrid.minPos + octreeGrid.cellSize * (float)octreeGrid.initialCellCount;
	printOctree(octree.root(), bbox);

	std::vector<unsigned int> branchGPU, leavesGPU;
	packOctree(octree, branchGPU, leavesGPU, octreeBranchCount);
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

	// THIS IS THE PROBLEM
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
