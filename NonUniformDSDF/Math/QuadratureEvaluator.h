#ifndef QUADRATUREEVALUATOR_H
#define QUADRATUREEVALUATOR_H
#include <utility>
#include <vector>
#include <glm/vec3.hpp>
#include <iostream>
#include "IntegralEvaluator.h"

class QuadratureEvaluator : public IntegralEvaluator
{
private:
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

	struct CoeffPoint
	{
		float coefficient;
		glm::vec3 point;
	};

	struct QuadraturePoints
	{
		std::vector<CoeffPoint> coeffPoints;
	};

public:
	QuadratureEvaluator(int _maxPointCount = 20);

	/*
	 * Evaluates the given function over the given integral (defaults to [-1;1]^3)
	 */
	virtual float evaluateIntegral(int pointCount, std::function<float(glm::vec3)> function,
		glm::vec3 intervalStarting = glm::vec3(-1), glm::vec3 intervalEnding = glm::vec3(1)) override
	{
		if (pointCount > maxPointCount || pointCount <= 0)
		{
#if DEBUG
			std::cerr << "Point count for gaussian quadratic is maximum " << maxPointCount << " and minimum 1! Point count may be capped!" << std::endl;
#endif
			if (pointCount <= 0) return 0.0f;
		}

		pointCount = std::min(pointCount, maxPointCount);

		glm::vec3 intervalSize = intervalEnding - intervalStarting;
		float value = 0.0f;
		for (int i = 0; i < quadraturePoints[pointCount].coeffPoints.size(); i++)
		{
			// shift the quadratic point into the input integral
			glm::vec3 shiftedPoint = (quadraturePoints[pointCount].coeffPoints[i].point * 0.5f + 0.5f) * intervalSize + intervalStarting;

			value += quadraturePoints[pointCount].coeffPoints[i].coefficient * function(shiftedPoint);
		}

		return value * intervalSize.x * intervalSize.y * intervalSize.z / 8.0f;
	}
private:
	int maxPointCount;
	// ith contains coefficients and points for the quadrature with i*i*i points
	std::vector<QuadraturePoints> quadraturePoints;
};

#endif