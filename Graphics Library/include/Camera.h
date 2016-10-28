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

#include "MasterHeader.h"

namespace Graphics
{
	class Camera
	{

	public:
		void Resize(const Vector2&  ac_NewDimensions);
		void RePosition(const Vector2& ac_ScreenPos);
		void ReBind(const Vector2& ac_NewRelativePos);

		void Update();

		const Vector2 & GetScreenPos();
		const Vector2 & GetWorldPos();
		const Vector2 & GetDimensions();
		const Vector2 & GetResolution();
		const Vector2 & GetZoom();
		float GetRotation();
		unsigned int GetWindowIndex();
		unsigned int GetWorldSpace();

		Camera(
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
			unsigned int	ac_uiWorldSpace);
		Camera() = delete;
		~Camera();

	private:

		Vector2 m_ScreenPos;
		Vector2 m_WorldPos;
		const Vector2 *	m_RelativePos;

		Vector2 m_Dimensions;
		Vector2 m_Resolution;

		Vector2 m_Zoom;
		float m_Rotation;

		bool m_bIsScrolling;

		Vector2 m_Velocity;

		unsigned int m_uiWindowIndex;
		unsigned int m_uiWorldSpace;
	};
}

#endif // _CAMERA_H_