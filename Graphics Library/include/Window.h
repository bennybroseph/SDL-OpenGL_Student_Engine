//////////////////////////////////////////////////////////////
// File: Window.h
// Author: Ben Odom
// Brief: This class contains all the necessary variables for
//		  the OpenGL context and SDL Window. Multiple
//		  instances of this class will exist at any given
//		  time, and each has functions that the 'Graphics'
//		  namespace can use to handle it internally
//////////////////////////////////////////////////////////////

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>
#include <SDL/SDL.h>

#include "MasterHeader.h"

namespace Graphics
{
	class Window
	{
	public:

		bool Resize(const UVector2 &newDimensions, unsigned int monitorIndex);
		bool Rename(const char *newTitle);

		bool ToggleFullscreen();

		const UVector2& GetDimensions() const;
		const UVector2& GetWindowedDimensions() const;
		const UVector2 & GetResolution() const;

		SDL_Window * GetWindow() const;
		const SDL_GLContext & GetContext() const;

		bool GetIsFullscreen() const;

		void Flip() const;

		// This is the only usable constructor
		Window(
			const UVector2 &				resolution,		// The window's internal resolution
			const bool						isFullscreen,	// Whether or not the window should be full screen on creation
			const UVector2 &				dimensions,		// The window's width and height
			const char *					title,			// The window's title
			const unsigned int				monitorIndex,	// Which monitor the window should be created on
			const List<SDL_DisplayMode> &	displayMode);	// A reference to all the current displays dimensions and specifications
		// The default constructor does not exist on purpose
		Window() = delete; // Make sure the default constructor cannot be called
		~Window();

	private:

		SDL_Window *m_sdlWindow;
		SDL_GLContext m_sdlGLContext;

		const List<SDL_DisplayMode> &m_sdlDisplayMode;

		UVector2PtrU m_dimensions			= make_unique<UVector2>();
		UVector2PtrU m_windowedDimensions	= make_unique<UVector2>();
		UVector2PtrU m_resolution			= make_unique<UVector2>();

		UVector2PtrU m_viewport		= make_unique<UVector2>();
		UVector2PtrU m_viewOffset	= make_unique<UVector2>();

		string m_title;

		bool m_isFullscreen;

		unsigned int m_monitorIndex;

	};
}

#endif // _WINDOW_H_