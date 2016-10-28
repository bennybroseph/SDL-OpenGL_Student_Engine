#include "include/Camera.h"



namespace Graphics
{
	void Camera::Resize(const Vector2 &newDimensions)
	{
		m_Dimensions = newDimensions;
	}
	void Camera::RePosition(const Vector2 &ac_NewScreenPos)
	{
		m_ScreenPos = ac_NewScreenPos;
	}
	void Camera::ReBind(const Vector2 &ac_NewRelativePos)
	{
		m_RelativePos = &ac_NewRelativePos;
	}

	void Camera::Update()
	{
		if (m_IsScrolling)
			m_WorldPos += m_Velocity;
	}

	const Vector2 & Camera::GetScreenPos() const
	{
		return m_ScreenPos;
	}
	const Vector2 & Camera::GetWorldPos() const
	{
		return m_WorldPos + *m_RelativePos;
	}
	const Vector2 & Camera::GetDimensions() const
	{
		return m_Dimensions;
	}
	const Vector2 & Camera::GetResolution() const
	{
		return m_Resolution;
	}
	const Vector2 & Camera::GetZoom() const
	{
		return m_Zoom;
	}
	float Camera::GetRotation() const
	{
		return m_Rotation;
	}
	unsigned int Camera::GetWindowIndex() const
	{
		return m_WindowIndex;
	}
	unsigned int Camera::GetWorldSpace() const
	{
		return m_WorldSpace;
	}

	Camera::Camera(
		const Vector2 &	screenPosition,
		const Vector2 &	worldPosition,
		const Vector2 &	relativePosition,
		const Vector2 &	size,
		const Vector2 &	resolution,
		const Vector2 &	zoom,
		float			rotation,
		bool			isScrolling,
		const Vector2 &	velocity,
		unsigned 		windowIndex,
		unsigned 		worldSpace)
	{
		m_ScreenPos =	 screenPosition;
		m_WorldPos =	 worldPosition;
		m_RelativePos = &relativePosition;

		m_Dimensions = size;
		m_Resolution = resolution;

		m_Zoom =	 zoom;
		m_Rotation = rotation;

		m_IsScrolling = isScrolling;

		m_Velocity = velocity;

		m_WindowIndex = windowIndex;
		m_WorldSpace =  worldSpace;
	}
	Camera::~Camera()
	{

	}
}
