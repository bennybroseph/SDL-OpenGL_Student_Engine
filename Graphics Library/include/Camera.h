//////////////////////////////////////////////////////////////
// File: Window.h
// Author: Ben Odom
// Brief: This class contains all the necessary variables for
//		  a viewport in OpenGL. It also includes some 
//		  functionality for scrolling cameras, as well as
//		  zoom and rotation for single instances of a camera.
//////////////////////////////////////////////////////////////

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "System.h"

namespace Graphics
{
	template <typename T>
	class Camera
	{
	private:
		System::Point2D<T>		  m_ScreenPos;
		System::Point2D<T>		  m_WorldPos;
		const System::Point2D<T>* m_RelativePos;

		System::Size2D<T> m_Dimensions;
		System::Size2D<T> m_Resolution;

		System::Size2D<T> m_Zoom;
		T m_Rotation;

		bool m_bIsScrolling;

		System::AngularVel<T> m_Velocity;

		unsigned int m_uiWindowIndex;
		unsigned int m_uiWorldSpace;

	public:
		void Resize(const System::Size2D<T>&  ac_NewDimensions);
		void RePosition(const System::Point2D<T>& ac_ScreenPos);
		void ReBind(const System::Point2D<T>& ac_NewRelativePos);

		void Update();

		const System::Point2D<T>& GetScreenPos();
		const System::Point2D<T>& GetWorldPos();
		const System::Size2D<T>& GetDimensions();
		const System::Size2D<T>& GetResolution();
		const System::Size2D<T>& GetZoom();
		const T GetRotation();
		const unsigned int GetWindowIndex();
		const unsigned int GetWorldSpace();

		Camera(
			const System::Point2D<T>&	 ac_ScreenPos,
			const System::Point2D<T>&	 ac_WorldPos,
			const System::Point2D<T>&	 ac_RelativePos,
			const System::Size2D<T>&	 ac_Dimension,
			const System::Size2D<T>&	 ac_Resolution,
			const System::Size2D<T>&	 ac_Zoom,
			const T						 ac_Rotation,
			const bool					 ac_bIsScrolling,
			const System::AngularVel<T>& ac_Velocity,
			const unsigned int			 ac_uiWindowIndex,
			const unsigned int			 ac_uiWorldSpace);
		Camera() = delete;
		~Camera();
	};

	template <typename T>
	void Camera<T>::Resize(const System::Size2D<T>& ac_NewDimensions)
	{
		m_Dimensions = ac_NewDimensions;
	}
	template <typename T>
	void Camera<T>::RePosition(const System::Point2D<T>& ac_NewScreenPos)
	{
		m_ScreenPos = ac_NewScreenPos;
	}
	template <typename T>
	void Camera<T>::ReBind(const System::Point2D<T>& ac_NewRelativePos)
	{
		m_RelativePos = &ac_NewWorldPos;
	}

	template <typename T>
	void Camera<T>::Update()
	{
		if (m_bIsScrolling)
			m_WorldPos += m_Velocity;
	}

	template <typename T>
	const System::Point2D<T>& Camera<T>::GetScreenPos()
	{
		return m_ScreenPos;
	}
	template <typename T>
	const System::Point2D<T>& Camera<T>::GetWorldPos()
	{
		return m_WorldPos + *m_RelativePos;
	}
	template <typename T>
	const System::Size2D<T>& Camera<T>::GetDimensions()
	{
		return m_Dimensions;
	}
	template <typename T>
	const System::Size2D<T>& Camera<T>::GetResolution()
	{
		return m_Resolution;
	}
	template <typename T>
	const System::Size2D<T>& Camera<T>::GetZoom()
	{
		return m_Zoom;
	}
	template <typename T>
	const T Camera<T>::GetRotation()
	{
		return m_Rotation;
	}
	template <typename T>
	const unsigned int Camera<T>::GetWindowIndex()
	{
		return m_uiWindowIndex;
	}
	template <typename T>
	const unsigned int Camera<T>::GetWorldSpace()
	{
		return m_uiWorldSpace;
	}

	template <typename T>
	Camera<T>::Camera(
		const System::Point2D<T>&	 ac_ScreenPos,
		const System::Point2D<T>&	 ac_WorldPos,
		const System::Point2D<T>&	 ac_RelativePos,
		const System::Size2D<T>&	 ac_Dimension,
		const System::Size2D<T>&	 ac_Resolution,
		const System::Size2D<T>&	 ac_Zoom,
		const T						 ac_Rotation,
		const bool					 ac_bIsScrolling,
		const System::AngularVel<T>& ac_Velocity,
		const unsigned int			 ac_uiWindowIndex,
		const unsigned int			 ac_uiWorldSpace)
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
	template <typename T>
	Camera<T>::~Camera()
	{

	}
}

#endif // _CAMERA_H_