#include "include/Window.h"



namespace Graphics
{
	bool Window::Resize(const UVector2 &newDimensions, unsigned monitorIndex)
	{
		m_MonitorIndex = monitorIndex;

		m_Dimensions->x = m_IsFullscreen ? m_SDLDisplayModes[m_MonitorIndex].w : newDimensions.x;
		m_Dimensions->y = m_IsFullscreen ? m_SDLDisplayModes[m_MonitorIndex].h : newDimensions.y;

		*m_WindowedDimensions = newDimensions;

		if (m_IsFullscreen)
			SDL_SetWindowFullscreen(m_SDLWindow, SDL_WINDOW_FULLSCREEN_DESKTOP * m_IsFullscreen);
		else
		{
			SDL_SetWindowFullscreen(m_SDLWindow, SDL_WINDOW_FULLSCREEN_DESKTOP * m_IsFullscreen);
			SDL_SetWindowSize(m_SDLWindow, m_WindowedDimensions->x, m_WindowedDimensions->y);
			SDL_SetWindowPosition(
				m_SDLWindow,
				SDL_WINDOWPOS_CENTERED_DISPLAY(m_MonitorIndex),
				SDL_WINDOWPOS_CENTERED_DISPLAY(m_MonitorIndex));
		}

		return true;
	}
	bool Window::Rename(const char*newTitle)
	{
		m_Title = newTitle;
		SDL_SetWindowTitle(m_SDLWindow, m_Title.c_str());

		return true;
	}

	bool Window::ToggleFullscreen()
	{
		m_IsFullscreen = !m_IsFullscreen;
		return Resize(*m_WindowedDimensions, m_MonitorIndex);
	}

	const UVector2 & Window::GetDimensions() const
	{
		return *m_Dimensions;
	}
	const UVector2 & Window::GetWindowedDimensions() const
	{
		return *m_WindowedDimensions;
	}
	const UVector2 & Window::GetResolution() const
	{
		return *m_Resolution;
	}

	SDL_Window * Window::GetWindow() const
	{
		return m_SDLWindow;
	}
	const SDL_GLContext & Window::GetContext() const
	{
		return m_SDLGLContext;
	}

	bool Window::GetIsFullscreen() const
	{
		return m_IsFullscreen;
	}

	void Window::Flip() const
	{
		SDL_GL_SwapWindow(m_SDLWindow);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::Window(
		const UVector2 &				resolution,
		bool							isFullscreen,
		const UVector2 &				dimensions,
		const char *					title,
		unsigned						monitorIndex,
		const List<SDL_DisplayMode> &	displayModes) : m_SDLDisplayModes(displayModes)
	{
		m_MonitorIndex = monitorIndex;

		m_Dimensions->x = (isFullscreen) ? m_SDLDisplayModes[m_MonitorIndex].w : dimensions.x;
		m_Dimensions->y = (isFullscreen) ? m_SDLDisplayModes[m_MonitorIndex].h : dimensions.y;

		*m_WindowedDimensions = dimensions;

		*m_Resolution = resolution;

		m_Title = title;

		m_IsFullscreen = isFullscreen;

		m_SDLWindow = SDL_CreateWindow(
			m_Title.c_str(),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_MonitorIndex),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_MonitorIndex),
			m_WindowedDimensions->x,
			m_WindowedDimensions->y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP * m_IsFullscreen | SDL_WINDOW_OPENGL);

		if (m_SDLWindow == nullptr)
		{
			printf("SDL_Error: %s\n", SDL_GetError());
			return;
		}

		//m_SDLGLContext = SDL_GL_CreateContext(m_SDLWindow);
	}
	Window::~Window()
	{

	}
}
