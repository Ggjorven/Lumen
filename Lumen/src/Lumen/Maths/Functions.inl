namespace Lumen::Maths
{

	////////////////////////////////////////////////////////////////////////////////////
	// Vectors
	////////////////////////////////////////////////////////////////////////////////////
	forceinline Vec3<float> Normalize(const Vec3<float>& vector)
	{
		return glm::normalize(vector);
	}

	forceinline Vec3<float> Cross(const Vec3<float>& a, const Vec3<float>& b)
	{
		return glm::cross(a, b);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Matrices
	////////////////////////////////////////////////////////////////////////////////////
	forceinline Mat4 Perspective(float fov, float aspectRatio, float nearClip, float farClip)
	{
		return glm::perspective(fov, aspectRatio, nearClip, farClip);
	}

	forceinline Mat4 Orthographic(float aspectRatio, float zoom)
	{
		return Orthographic(-aspectRatio * zoom, aspectRatio * zoom, -1.0f * zoom, 1.0f * zoom, -1.0f, 1.0f);
	}

	forceinline Mat4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
	{
		return glm::ortho(left, right, bottom, top, nearClip, farClip);
	}

	forceinline Mat4 LookAt(const Vec3<float>& position, const Vec3<float>& target, const Vec3<float>& up)
	{
		return glm::lookAt(position, target, up);
	}

	forceinline Mat4 Translate(const Mat4& matrix, const Vec3<float>& translation)
	{
		return glm::translate(matrix, translation);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Trigonometry
	////////////////////////////////////////////////////////////////////////////////////
	forceinline float Sin(float angle)
	{
		return glm::sin(angle);
	}

	forceinline float Cos(float angle)
	{
		return glm::cos(angle);
	}

	forceinline float Tan(float angle)
	{
		return glm::tan(angle);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Utils
	///////////////////////////////////////////////////////////////////////////////////
	forceinline float Radians(float degrees)
	{
		return glm::radians(degrees);
	}

	forceinline float AspectRatio(uint32_t width, uint32_t height)
	{
		return static_cast<float>(width) / static_cast<float>(height);
	}

}