#pragma once

#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Core/Core.hpp"

#include "Lumen/Maths/Structs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>
#include <type_traits>

namespace Lumen::Maths
{

	////////////////////////////////////////////////////////////////////////////////////
	// Vectors
	////////////////////////////////////////////////////////////////////////////////////
	Vec3<float> Normalize(const Vec3<float>& vector);
	Vec3<float> Cross(const Vec3<float>& a, const Vec3<float>& b);

	////////////////////////////////////////////////////////////////////////////////////
	// Matrices
	////////////////////////////////////////////////////////////////////////////////////
	Mat4 Perspective(float fov, float aspectRatio, float nearClip = 0.1f, float farClip = 100.0f);

	Mat4 Orthographic(float aspectRatio, float zoom = 1.0f);
	Mat4 Orthographic(float left, float right, float bottom, float top, float nearClip = -1.0f, float farClip = 1.0f);

	Mat4 LookAt(const Vec3<float>& position, const Vec3<float>& target, const Vec3<float>& up = { 0.0f, 1.0f, 0.0f });

	Mat4 Translate(const Mat4& matrix, const Vec3<float>& translation);

	////////////////////////////////////////////////////////////////////////////////////
	// Trigonometry 
	////////////////////////////////////////////////////////////////////////////////////
	float Sin(float angle);
	float Cos(float angle);
	float Tan(float angle);

	////////////////////////////////////////////////////////////////////////////////////
	// Utils
	////////////////////////////////////////////////////////////////////////////////////
	float Radians(float degrees);
	float AspectRatio(uint32_t width, uint32_t height);

}

#include "Lumen/Maths/Functions.inl"