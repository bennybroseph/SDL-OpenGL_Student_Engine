#include "include/Camera.h"



namespace Graphics
{
	void Camera::Resize(const Vector2 &ac_NewDimensions)
	{
		m_Dimensions = ac_NewDimensions;
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
		if (m_bIsScrolling)
			m_WorldPos += m_Velocity;
	}

	const Vector2 & Camera::GetScreenPos()
	{
		return m_ScreenPos;
	}
	const Vector2 & Camera::GetWorldPos()
	{
		return m_WorldPos + *m_RelativePos;
	}
	const Vector2 & Camera::GetDimensions()
	{
		return m_Dimensions;
	}
	const Vector2 & Camera::GetResolution()
	{
		return m_Resolution;
	}
	const Vector2 & Camera::GetZoom()
	{
		return m_Zoom;
	}
	float Camera::GetRotation()
	{
		return m_Rotation;
	}
	unsigned int Camera::GetWindowIndex()
	{
		return m_uiWindowIndex;
	}
	unsigned int Camera::GetWorldSpace()
	{
		return m_uiWorldSpace;
	}

	Camera::Camera(
		const Vector2 &	ac_ScreenPos,
		const Vector2 &	ac_WorldPos,
		const Vector2 &	ac_RelativePos,
		const Vector2 &	ac_Dimension,
		const Vector2 &	ac_Resolution,
		const Vector2 &	ac_Zoom,
		float			ac_Rotation,
		bool			ac_bIsScrolling,
		const Vector2 &	ac_Velocity,
		unsigned int	ac_uiWindowIndex,
		unsigned int	ac_uiWorldSpace)
	{
		m_ScreenPos =	 ac_ScreenPos;
		m_WorldPos =	 ac_WorldPos;
		m_RelativePos = &ac_RelativePos;

		m_Dimensions = ac_Dimension;
		m_Resolution = ac_Resolution;

		m_Zoom =	 ac_Zoom;
		m_Rotation = ac_Rotation;

		m_bIsScrolling = ac_bIsScrolling;

		m_Velocity = ac_Velocity;

		m_uiWindowIndex = ac_uiWindowIndex;
		m_uiWorldSpace =  ac_uiWorldSpace;
	}
	Camera::~Camera()
	{

	}
}