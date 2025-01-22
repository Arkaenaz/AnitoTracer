#pragma once
#include "../Math/Math.h"
using namespace Math;

// ---- ENGINE CONFIG ---- //
const float WINDOW_WIDTH = 1440.0f;
const float WINDOW_HEIGHT = 980.0f;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;

// ---- STRUCTS ---- //

// Constant buffer.
__declspec(align(16))
struct Constant {
	Matrix4x4 world;
	Matrix4x4 view;
	Matrix4x4 proj;
	Vector4 lightDir;
	Vector4 cameraPos;
};

struct Vertex
{
	Vector3 position;
	Vector4 color;
};

struct Vertex3D {
	Vector3 position;
	Vector2 texcoord;
	Vector4 color;
};

//// ---- COLORS ---- //
//const Vector4 CREAM = Vector4(1.0f, 0.99f, 0.81f, 1.0f);
//const Vector4 MATCHA = Vector4(139.0f / 255.0f, 168.0f / 255.0f, 136.0f / 255.0f, 1.0f);
//const Vector4 SPACE = Vector4(59.0f / 255.0f, 59.0f / 255.0f, 88.0f / 255.0f, 1.0f);
//const Vector4 LAVENDER = Vector4(215 / 255.0f, 180 / 255.0f, 243 / 255.0f, 1.0f);
//const Vector4 GOAWAYGREEN = Vector4(181.0f / 255.0f, 175.0f / 255.0f, 20.0f / 255.0f, 1.0f);
//const Vector4 LIGHTMAUVE = Vector4(0.78f, 0.45f, 0.59f, 1.0f);
//
//// Red & Neutrals
//const Vector4 REDCARPET = Vector4(0.75f, 0.12f, 0.12f, 1.0f);
//const Vector4 REDRAMPAGE = Vector4(0.95f, 0.22f, 0.15f, 1.0f);
//const Vector4 CORALROSE = Vector4(0.95f, 0.47f, 0.29f, 1.0f);
//const Vector4 SCOTCHMIST = Vector4(0.94f, 0.89f, 0.80f, 1.0f);
//const Vector4 COUNTRYCORK = Vector4(0.72f, 0.65f, 0.52f, 1.0f);
//
//// Pink & Neutrals
//const Vector4 PINK = Vector4(0.957f, 0.761f, 0.761f, 1.0f);
//const Vector4 ASHGRAY = Vector4(174.0f / 255.0f, 180.0f / 255.0f, 169.0f / 255.0f, 1.0f);
//const Vector4 PALEDOGWOOD = Vector4(224.0f / 255.0f, 193.0f / 255.0f, 179.0f / 255.0f, 1.0f);
//const Vector4 ROSYBROWN = Vector4(216.0f / 255.0f, 154.0f / 255.0f, 158.0f / 255.0f, 1.0f);
//const Vector4 PUCE = Vector4(195.0f / 255.0f, 125.0f / 255.0f, 146.0f / 255.0f, 1.0f);
//const Vector4 ROSETAUPE = Vector4(132.0f / 255.0f, 98.0f / 255.0f, 103.0f / 255.0f, 1.0f);
//
//// Basics
//const Vector4 GRAY = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
//const Vector4 WHITE = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
//const Vector4 BLACK = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
//const Vector4 CLEAR = Vector4(1, 1, 1, 0.0f);
//
//// Rainbow
//const Vector4 RED = Vector4(1, 0, 0, 1);
//const Vector4 ORANGE = Vector4(1, 0.6f, 0, 1);
//const Vector4 YELLOW = Vector4(1, 1, 0, 1);
//const Vector4 GREEN = Vector4(0, 1, 0, 1);
//const Vector4 BLUE = Vector4(0, 0, 1, 1);
//const Vector4 INDIGO = Vector4(0.3f, 0, 0.5f, 1);
//const Vector4 VIOLET = Vector4(0.5f, 0, 1, 1);

// ---- ENUMS ---- //
enum CameraView
{
	orthographic = 0,
	perspective,
	topdown,
	sidescroller
};

enum Screens {
	hierarchy = 0,
	inspector,
	profiler,
	color_picker,
	credits
};