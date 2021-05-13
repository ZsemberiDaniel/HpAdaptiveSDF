#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model7 : public SDFBase
{
public:
	SDF2Model7(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model7", "SDFGPUandCPU/SDF2Model7.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.5f, 0.5f); 
		float r0 = sdfBox(p - vec3(0, 0, 0.13f), vec3(0.1f, 0.02f, 0.1f));
		float r1 = sdfBox(p - vec3(0.095f, 0.1f, 0.13f), vec3(0.005f, 0.1f, 0.1f));
		float r2 = sdfBox(p - vec3(-0.095f, 0.1f, 0.13f), vec3(0.005f, 0.1f, 0.1f));
		r1 = min(r1, r2); //union 1
		float r3 = sdfBox(p - vec3(0, 0.1f, 0.225f), vec3(0.1f, 0.1f, 0.005f));
		r1 = min(r1, r3); //union 2
		float r4 = sdfBox(p - vec3(0, 0.1f, 0.035f), vec3(0.1f, 0.1f, 0.005f));
		r1 = min(r1, r4); //union 3
		r0 = min(r0, r1); //union 1
		float r5 = sdfBox(p - vec3(-0.032f, 0.1f, 0.215f), vec3(0.008f, 0.1f, 0.015f));
		float r6 = sdfBox(p - vec3(-0.032f, 0.1f, 0.045f), vec3(0.008f, 0.1f, 0.015f));
		r5 = min(r5, r6); //union 1
		float r7 = sdfBox(p - vec3(0.032f, 0.1f, 0.215f), vec3(0.008f, 0.1f, 0.015f));
		r5 = min(r5, r7); //union 1
		float r8 = sdfBox(p - vec3(0.032f, 0.1f, 0.045f), vec3(0.008f, 0.1f, 0.015f));
		r5 = min(r5, r8); //union 2
		r0 = min(r0, r5); //union 2
		float r9 = sdfBox(p - vec3(0.085f, 0.1f, 0.098f), vec3(0.015f, 0.1f, 0.008f));
		float r10 = sdfBox(p - vec3(-0.085f, 0.1f, 0.098f), vec3(0.015f, 0.1f, 0.008f));
		r9 = min(r9, r10); //union 1
		float r11 = sdfBox(p - vec3(0.085f, 0.1f, 0.13f), vec3(0.015f, 0.1f, 0.008f));
		r9 = min(r9, r11); //union 1
		float r12 = sdfBox(p - vec3(-0.085f, 0.1f, 0.13f), vec3(0.015f, 0.1f, 0.008f));
		r9 = min(r9, r12); //union 2
		float r13 = sdfBox(p - vec3(0.085f, 0.1f, 0.162f), vec3(0.015f, 0.1f, 0.008f));
		r9 = min(r9, r13); //union 1
		float r14 = sdfBox(p - vec3(-0.085f, 0.1f, 0.162f), vec3(0.015f, 0.1f, 0.008f));
		r9 = min(r9, r14); //union 2
		r0 = min(r0, r9); //union 3
		float r15 = sdfBox(p - vec3(0, 0, -0.13f), vec3(0.1f, 0.02f, 0.1f));
		float r16 = sdfBox(p - vec3(0.095f, 0.1f, -0.13f), vec3(0.005f, 0.1f, 0.1f));
		float r17 = sdfBox(p - vec3(-0.095f, 0.1f, -0.13f), vec3(0.005f, 0.1f, 0.1f));
		r16 = min(r16, r17); //union 1
		float r18 = sdfBox(p - vec3(0, 0.1f, -0.035f), vec3(0.1f, 0.1f, 0.005f));
		r16 = min(r16, r18); //union 2
		float r19 = sdfBox(p - vec3(0, 0.1f, -0.225f), vec3(0.1f, 0.1f, 0.005f));
		r16 = min(r16, r19); //union 3
		r15 = min(r15, r16); //union 1
		float r20 = sdfBox(p - vec3(-0.032f, 0.1f, -0.045f), vec3(0.008f, 0.1f, 0.015f));
		float r21 = sdfBox(p - vec3(-0.032f, 0.1f, -0.215f), vec3(0.008f, 0.1f, 0.015f));
		r20 = min(r20, r21); //union 1
		float r22 = sdfBox(p - vec3(0.032f, 0.1f, -0.045f), vec3(0.008f, 0.1f, 0.015f));
		r20 = min(r20, r22); //union 1
		float r23 = sdfBox(p - vec3(0.032f, 0.1f, -0.215f), vec3(0.008f, 0.1f, 0.015f));
		r20 = min(r20, r23); //union 2
		r15 = min(r15, r20); //union 2
		float r24 = sdfBox(p - vec3(0.085f, 0.1f, -0.162f), vec3(0.015f, 0.1f, 0.008f));
		float r25 = sdfBox(p - vec3(-0.085f, 0.1f, -0.162f), vec3(0.015f, 0.1f, 0.008f));
		r24 = min(r24, r25); //union 1
		float r26 = sdfBox(p - vec3(0.085f, 0.1f, -0.13f), vec3(0.015f, 0.1f, 0.008f));
		r24 = min(r24, r26); //union 1
		float r27 = sdfBox(p - vec3(-0.085f, 0.1f, -0.13f), vec3(0.015f, 0.1f, 0.008f));
		r24 = min(r24, r27); //union 2
		float r28 = sdfBox(p - vec3(0.085f, 0.1f, -0.098f), vec3(0.015f, 0.1f, 0.008f));
		r24 = min(r24, r28); //union 1
		float r29 = sdfBox(p - vec3(-0.085f, 0.1f, -0.098f), vec3(0.015f, 0.1f, 0.008f));
		r24 = min(r24, r29); //union 2
		r15 = min(r15, r24); //union 3
		r0 = min(r0, r15); //union 1
		float r30 = sdfBox(p - vec3(-0.08f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r30); //union 1
		float r31 = sdfBox(p - vec3(0.08f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r31); //union 2
		float r32 = sdfBox(p - vec3(-0.0666667f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r32); //union 1
		float r33 = sdfBox(p - vec3(0.0666667f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r33); //union 2
		float r34 = sdfBox(p - vec3(-0.0533333f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r34); //union 1
		float r35 = sdfBox(p - vec3(0.0533333f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r0 = min(r0, r35); //union 2
		float r36 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.13f), vec2(0.06f, 0.025f));
		float r37 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.13f), vec2(0.054f, 0.05f));
		r37 *= -1.f; //invert
		r36 = max(r36, r37); //intersect 1
		r0 = min(r0, r36); //union 1
		float r38 = sdfCylinderX(p - vec3(0.125f, 0.06f, -0.13f), vec2(0.06f, 0.025f));
		float r39 = sdfCylinderX(p - vec3(0.125f, 0.06f, -0.13f), vec2(0.054f, 0.05f));
		r39 *= -1.f; //invert
		r38 = max(r38, r39); //intersect 1
		r0 = min(r0, r38); //union 2
		float r40 = sdfCylinderZ(p - vec3(0, 0.06f, 0.255f), vec2(0.06f, 0.025f));
		float r41 = sdfCylinderZ(p - vec3(0, 0.06f, 0.255f), vec2(0.054f, 0.05f));
		r41 *= -1.f; //invert
		r40 = max(r40, r41); //intersect 1
		r0 = min(r0, r40); //union 1
		float r42 = sdfBox(p - vec3(0, 0, 0.64f), vec3(0.1f, 0.02f, 0.1f));
		float r43 = sdfBox(p - vec3(0.095f, 0.1f, 0.64f), vec3(0.005f, 0.1f, 0.1f));
		float r44 = sdfBox(p - vec3(-0.095f, 0.1f, 0.64f), vec3(0.005f, 0.1f, 0.1f));
		r43 = min(r43, r44); //union 1
		float r45 = sdfBox(p - vec3(0, 0.1f, 0.735f), vec3(0.1f, 0.1f, 0.005f));
		r43 = min(r43, r45); //union 2
		float r46 = sdfBox(p - vec3(0, 0.1f, 0.545f), vec3(0.1f, 0.1f, 0.005f));
		r43 = min(r43, r46); //union 3
		r42 = min(r42, r43); //union 1
		float r47 = sdfBox(p - vec3(-0.032f, 0.1f, 0.725f), vec3(0.008f, 0.1f, 0.015f));
		float r48 = sdfBox(p - vec3(-0.032f, 0.1f, 0.555f), vec3(0.008f, 0.1f, 0.015f));
		r47 = min(r47, r48); //union 1
		float r49 = sdfBox(p - vec3(0.032f, 0.1f, 0.725f), vec3(0.008f, 0.1f, 0.015f));
		r47 = min(r47, r49); //union 1
		float r50 = sdfBox(p - vec3(0.032f, 0.1f, 0.555f), vec3(0.008f, 0.1f, 0.015f));
		r47 = min(r47, r50); //union 2
		r42 = min(r42, r47); //union 2
		float r51 = sdfBox(p - vec3(0.085f, 0.1f, 0.608f), vec3(0.015f, 0.1f, 0.008f));
		float r52 = sdfBox(p - vec3(-0.085f, 0.1f, 0.608f), vec3(0.015f, 0.1f, 0.008f));
		r51 = min(r51, r52); //union 1
		float r53 = sdfBox(p - vec3(0.085f, 0.1f, 0.64f), vec3(0.015f, 0.1f, 0.008f));
		r51 = min(r51, r53); //union 1
		float r54 = sdfBox(p - vec3(-0.085f, 0.1f, 0.64f), vec3(0.015f, 0.1f, 0.008f));
		r51 = min(r51, r54); //union 2
		float r55 = sdfBox(p - vec3(0.085f, 0.1f, 0.672f), vec3(0.015f, 0.1f, 0.008f));
		r51 = min(r51, r55); //union 1
		float r56 = sdfBox(p - vec3(-0.085f, 0.1f, 0.672f), vec3(0.015f, 0.1f, 0.008f));
		r51 = min(r51, r56); //union 2
		r42 = min(r42, r51); //union 3
		float r57 = sdfBox(p - vec3(0, 0, 0.38f), vec3(0.1f, 0.02f, 0.1f));
		float r58 = sdfBox(p - vec3(0.095f, 0.1f, 0.38f), vec3(0.005f, 0.1f, 0.1f));
		float r59 = sdfBox(p - vec3(-0.095f, 0.1f, 0.38f), vec3(0.005f, 0.1f, 0.1f));
		r58 = min(r58, r59); //union 1
		float r60 = sdfBox(p - vec3(0, 0.1f, 0.475f), vec3(0.1f, 0.1f, 0.005f));
		r58 = min(r58, r60); //union 2
		float r61 = sdfBox(p - vec3(0, 0.1f, 0.285f), vec3(0.1f, 0.1f, 0.005f));
		r58 = min(r58, r61); //union 3
		r57 = min(r57, r58); //union 1
		float r62 = sdfBox(p - vec3(-0.032f, 0.1f, 0.465f), vec3(0.008f, 0.1f, 0.015f));
		float r63 = sdfBox(p - vec3(-0.032f, 0.1f, 0.295f), vec3(0.008f, 0.1f, 0.015f));
		r62 = min(r62, r63); //union 1
		float r64 = sdfBox(p - vec3(0.032f, 0.1f, 0.465f), vec3(0.008f, 0.1f, 0.015f));
		r62 = min(r62, r64); //union 1
		float r65 = sdfBox(p - vec3(0.032f, 0.1f, 0.295f), vec3(0.008f, 0.1f, 0.015f));
		r62 = min(r62, r65); //union 2
		r57 = min(r57, r62); //union 2
		float r66 = sdfBox(p - vec3(0.085f, 0.1f, 0.348f), vec3(0.015f, 0.1f, 0.008f));
		float r67 = sdfBox(p - vec3(-0.085f, 0.1f, 0.348f), vec3(0.015f, 0.1f, 0.008f));
		r66 = min(r66, r67); //union 1
		float r68 = sdfBox(p - vec3(0.085f, 0.1f, 0.38f), vec3(0.015f, 0.1f, 0.008f));
		r66 = min(r66, r68); //union 1
		float r69 = sdfBox(p - vec3(-0.085f, 0.1f, 0.38f), vec3(0.015f, 0.1f, 0.008f));
		r66 = min(r66, r69); //union 2
		float r70 = sdfBox(p - vec3(0.085f, 0.1f, 0.412f), vec3(0.015f, 0.1f, 0.008f));
		r66 = min(r66, r70); //union 1
		float r71 = sdfBox(p - vec3(-0.085f, 0.1f, 0.412f), vec3(0.015f, 0.1f, 0.008f));
		r66 = min(r66, r71); //union 2
		r57 = min(r57, r66); //union 3
		r42 = min(r42, r57); //union 1
		float r72 = sdfBox(p - vec3(-0.08f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r72); //union 1
		float r73 = sdfBox(p - vec3(0.08f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r73); //union 2
		float r74 = sdfBox(p - vec3(-0.0666667f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r74); //union 1
		float r75 = sdfBox(p - vec3(0.0666667f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r75); //union 2
		float r76 = sdfBox(p - vec3(-0.0533333f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r76); //union 1
		float r77 = sdfBox(p - vec3(0.0533333f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r42 = min(r42, r77); //union 2
		r0 = min(r0, r42); //union 1
		float r78 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.64f), vec2(0.06f, 0.025f));
		float r79 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.64f), vec2(0.054f, 0.05f));
		r79 *= -1.f; //invert
		r78 = max(r78, r79); //intersect 1
		r0 = min(r0, r78); //union 1
		float r80 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.38f), vec2(0.06f, 0.025f));
		float r81 = sdfCylinderX(p - vec3(0.125f, 0.06f, 0.38f), vec2(0.054f, 0.05f));
		r81 *= -1.f; //invert
		r80 = max(r80, r81); //intersect 1
		r0 = min(r0, r80); //union 2
		float r82 = sdfBox(p - vec3(0.25f, 0, 0.13f), vec3(0.1f, 0.02f, 0.1f));
		float r83 = sdfBox(p - vec3(0.345f, 0.1f, 0.13f), vec3(0.005f, 0.1f, 0.1f));
		float r84 = sdfBox(p - vec3(0.155f, 0.1f, 0.13f), vec3(0.005f, 0.1f, 0.1f));
		r83 = min(r83, r84); //union 1
		float r85 = sdfBox(p - vec3(0.25f, 0.1f, 0.225f), vec3(0.1f, 0.1f, 0.005f));
		r83 = min(r83, r85); //union 2
		float r86 = sdfBox(p - vec3(0.25f, 0.1f, 0.035f), vec3(0.1f, 0.1f, 0.005f));
		r83 = min(r83, r86); //union 3
		r82 = min(r82, r83); //union 1
		float r87 = sdfBox(p - vec3(0.218f, 0.1f, 0.215f), vec3(0.008f, 0.1f, 0.015f));
		float r88 = sdfBox(p - vec3(0.218f, 0.1f, 0.045f), vec3(0.008f, 0.1f, 0.015f));
		r87 = min(r87, r88); //union 1
		float r89 = sdfBox(p - vec3(0.282f, 0.1f, 0.215f), vec3(0.008f, 0.1f, 0.015f));
		r87 = min(r87, r89); //union 1
		float r90 = sdfBox(p - vec3(0.282f, 0.1f, 0.045f), vec3(0.008f, 0.1f, 0.015f));
		r87 = min(r87, r90); //union 2
		r82 = min(r82, r87); //union 2
		float r91 = sdfBox(p - vec3(0.335f, 0.1f, 0.098f), vec3(0.015f, 0.1f, 0.008f));
		float r92 = sdfBox(p - vec3(0.165f, 0.1f, 0.098f), vec3(0.015f, 0.1f, 0.008f));
		r91 = min(r91, r92); //union 1
		float r93 = sdfBox(p - vec3(0.335f, 0.1f, 0.13f), vec3(0.015f, 0.1f, 0.008f));
		r91 = min(r91, r93); //union 1
		float r94 = sdfBox(p - vec3(0.165f, 0.1f, 0.13f), vec3(0.015f, 0.1f, 0.008f));
		r91 = min(r91, r94); //union 2
		float r95 = sdfBox(p - vec3(0.335f, 0.1f, 0.162f), vec3(0.015f, 0.1f, 0.008f));
		r91 = min(r91, r95); //union 1
		float r96 = sdfBox(p - vec3(0.165f, 0.1f, 0.162f), vec3(0.015f, 0.1f, 0.008f));
		r91 = min(r91, r96); //union 2
		r82 = min(r82, r91); //union 3
		float r97 = sdfBox(p - vec3(0.25f, 0, -0.13f), vec3(0.1f, 0.02f, 0.1f));
		float r98 = sdfBox(p - vec3(0.345f, 0.1f, -0.13f), vec3(0.005f, 0.1f, 0.1f));
		float r99 = sdfBox(p - vec3(0.155f, 0.1f, -0.13f), vec3(0.005f, 0.1f, 0.1f));
		r98 = min(r98, r99); //union 1
		float r100 = sdfBox(p - vec3(0.25f, 0.1f, -0.035f), vec3(0.1f, 0.1f, 0.005f));
		r98 = min(r98, r100); //union 2
		float r101 = sdfBox(p - vec3(0.25f, 0.1f, -0.225f), vec3(0.1f, 0.1f, 0.005f));
		r98 = min(r98, r101); //union 3
		r97 = min(r97, r98); //union 1
		float r102 = sdfBox(p - vec3(0.218f, 0.1f, -0.045f), vec3(0.008f, 0.1f, 0.015f));
		float r103 = sdfBox(p - vec3(0.218f, 0.1f, -0.215f), vec3(0.008f, 0.1f, 0.015f));
		r102 = min(r102, r103); //union 1
		float r104 = sdfBox(p - vec3(0.282f, 0.1f, -0.045f), vec3(0.008f, 0.1f, 0.015f));
		r102 = min(r102, r104); //union 1
		float r105 = sdfBox(p - vec3(0.282f, 0.1f, -0.215f), vec3(0.008f, 0.1f, 0.015f));
		r102 = min(r102, r105); //union 2
		r97 = min(r97, r102); //union 2
		float r106 = sdfBox(p - vec3(0.335f, 0.1f, -0.162f), vec3(0.015f, 0.1f, 0.008f));
		float r107 = sdfBox(p - vec3(0.165f, 0.1f, -0.162f), vec3(0.015f, 0.1f, 0.008f));
		r106 = min(r106, r107); //union 1
		float r108 = sdfBox(p - vec3(0.335f, 0.1f, -0.13f), vec3(0.015f, 0.1f, 0.008f));
		r106 = min(r106, r108); //union 1
		float r109 = sdfBox(p - vec3(0.165f, 0.1f, -0.13f), vec3(0.015f, 0.1f, 0.008f));
		r106 = min(r106, r109); //union 2
		float r110 = sdfBox(p - vec3(0.335f, 0.1f, -0.098f), vec3(0.015f, 0.1f, 0.008f));
		r106 = min(r106, r110); //union 1
		float r111 = sdfBox(p - vec3(0.165f, 0.1f, -0.098f), vec3(0.015f, 0.1f, 0.008f));
		r106 = min(r106, r111); //union 2
		r97 = min(r97, r106); //union 3
		r82 = min(r82, r97); //union 1
		float r112 = sdfBox(p - vec3(0.17f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r112); //union 1
		float r113 = sdfBox(p - vec3(0.33f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r113); //union 2
		float r114 = sdfBox(p - vec3(0.183333f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r114); //union 1
		float r115 = sdfBox(p - vec3(0.316667f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r115); //union 2
		float r116 = sdfBox(p - vec3(0.196667f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r116); //union 1
		float r117 = sdfBox(p - vec3(0.303333f, 0.02f, 0), vec3(0.00333333f, 0.08f, 0.03f));
		r82 = min(r82, r117); //union 2
		r0 = min(r0, r82); //union 1
		float r118 = sdfCylinderZ(p - vec3(0.25f, 0.06f, 0.255f), vec2(0.06f, 0.025f));
		float r119 = sdfCylinderZ(p - vec3(0.25f, 0.06f, 0.255f), vec2(0.054f, 0.05f));
		r119 *= -1.f; //invert
		r118 = max(r118, r119); //intersect 1
		r0 = min(r0, r118); //union 1
		float r120 = sdfBox(p - vec3(0.25f, 0, 0.64f), vec3(0.1f, 0.02f, 0.1f));
		float r121 = sdfBox(p - vec3(0.345f, 0.1f, 0.64f), vec3(0.005f, 0.1f, 0.1f));
		float r122 = sdfBox(p - vec3(0.155f, 0.1f, 0.64f), vec3(0.005f, 0.1f, 0.1f));
		r121 = min(r121, r122); //union 1
		float r123 = sdfBox(p - vec3(0.25f, 0.1f, 0.735f), vec3(0.1f, 0.1f, 0.005f));
		r121 = min(r121, r123); //union 2
		float r124 = sdfBox(p - vec3(0.25f, 0.1f, 0.545f), vec3(0.1f, 0.1f, 0.005f));
		r121 = min(r121, r124); //union 3
		r120 = min(r120, r121); //union 1
		float r125 = sdfBox(p - vec3(0.218f, 0.1f, 0.725f), vec3(0.008f, 0.1f, 0.015f));
		float r126 = sdfBox(p - vec3(0.218f, 0.1f, 0.555f), vec3(0.008f, 0.1f, 0.015f));
		r125 = min(r125, r126); //union 1
		float r127 = sdfBox(p - vec3(0.282f, 0.1f, 0.725f), vec3(0.008f, 0.1f, 0.015f));
		r125 = min(r125, r127); //union 1
		float r128 = sdfBox(p - vec3(0.282f, 0.1f, 0.555f), vec3(0.008f, 0.1f, 0.015f));
		r125 = min(r125, r128); //union 2
		r120 = min(r120, r125); //union 2
		float r129 = sdfBox(p - vec3(0.335f, 0.1f, 0.608f), vec3(0.015f, 0.1f, 0.008f));
		float r130 = sdfBox(p - vec3(0.165f, 0.1f, 0.608f), vec3(0.015f, 0.1f, 0.008f));
		r129 = min(r129, r130); //union 1
		float r131 = sdfBox(p - vec3(0.335f, 0.1f, 0.64f), vec3(0.015f, 0.1f, 0.008f));
		r129 = min(r129, r131); //union 1
		float r132 = sdfBox(p - vec3(0.165f, 0.1f, 0.64f), vec3(0.015f, 0.1f, 0.008f));
		r129 = min(r129, r132); //union 2
		float r133 = sdfBox(p - vec3(0.335f, 0.1f, 0.672f), vec3(0.015f, 0.1f, 0.008f));
		r129 = min(r129, r133); //union 1
		float r134 = sdfBox(p - vec3(0.165f, 0.1f, 0.672f), vec3(0.015f, 0.1f, 0.008f));
		r129 = min(r129, r134); //union 2
		r120 = min(r120, r129); //union 3
		float r135 = sdfBox(p - vec3(0.25f, 0, 0.38f), vec3(0.1f, 0.02f, 0.1f));
		float r136 = sdfBox(p - vec3(0.345f, 0.1f, 0.38f), vec3(0.005f, 0.1f, 0.1f));
		float r137 = sdfBox(p - vec3(0.155f, 0.1f, 0.38f), vec3(0.005f, 0.1f, 0.1f));
		r136 = min(r136, r137); //union 1
		float r138 = sdfBox(p - vec3(0.25f, 0.1f, 0.475f), vec3(0.1f, 0.1f, 0.005f));
		r136 = min(r136, r138); //union 2
		float r139 = sdfBox(p - vec3(0.25f, 0.1f, 0.285f), vec3(0.1f, 0.1f, 0.005f));
		r136 = min(r136, r139); //union 3
		r135 = min(r135, r136); //union 1
		float r140 = sdfBox(p - vec3(0.218f, 0.1f, 0.465f), vec3(0.008f, 0.1f, 0.015f));
		float r141 = sdfBox(p - vec3(0.218f, 0.1f, 0.295f), vec3(0.008f, 0.1f, 0.015f));
		r140 = min(r140, r141); //union 1
		float r142 = sdfBox(p - vec3(0.282f, 0.1f, 0.465f), vec3(0.008f, 0.1f, 0.015f));
		r140 = min(r140, r142); //union 1
		float r143 = sdfBox(p - vec3(0.282f, 0.1f, 0.295f), vec3(0.008f, 0.1f, 0.015f));
		r140 = min(r140, r143); //union 2
		r135 = min(r135, r140); //union 2
		float r144 = sdfBox(p - vec3(0.335f, 0.1f, 0.348f), vec3(0.015f, 0.1f, 0.008f));
		float r145 = sdfBox(p - vec3(0.165f, 0.1f, 0.348f), vec3(0.015f, 0.1f, 0.008f));
		r144 = min(r144, r145); //union 1
		float r146 = sdfBox(p - vec3(0.335f, 0.1f, 0.38f), vec3(0.015f, 0.1f, 0.008f));
		r144 = min(r144, r146); //union 1
		float r147 = sdfBox(p - vec3(0.165f, 0.1f, 0.38f), vec3(0.015f, 0.1f, 0.008f));
		r144 = min(r144, r147); //union 2
		float r148 = sdfBox(p - vec3(0.335f, 0.1f, 0.412f), vec3(0.015f, 0.1f, 0.008f));
		r144 = min(r144, r148); //union 1
		float r149 = sdfBox(p - vec3(0.165f, 0.1f, 0.412f), vec3(0.015f, 0.1f, 0.008f));
		r144 = min(r144, r149); //union 2
		r135 = min(r135, r144); //union 3
		r120 = min(r120, r135); //union 1
		float r150 = sdfBox(p - vec3(0.17f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r150); //union 1
		float r151 = sdfBox(p - vec3(0.33f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r151); //union 2
		float r152 = sdfBox(p - vec3(0.183333f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r152); //union 1
		float r153 = sdfBox(p - vec3(0.316667f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r153); //union 2
		float r154 = sdfBox(p - vec3(0.196667f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r154); //union 1
		float r155 = sdfBox(p - vec3(0.303333f, 0.02f, 0.51f), vec3(0.00333333f, 0.08f, 0.03f));
		r120 = min(r120, r155); //union 2
		r0 = min(r0, r120); //union 1
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model7() final = default;

private:
};
#endif
