#include "include/Window.h"



namespace Graphics
{
	bool Window::Resize(const UVector2 &newDimensions, unsigned int monitorIndex)
	{
		m_monitorIndex = monitorIndex;

		m_dimensions->x = m_isFullscreen ? m_sdlDisplayMode[m_monitorIndex].w : newDimensions.x;
		m_dimensions->y = m_isFullscreen ? m_sdlDisplayMode[m_monitorIndex].h : newDimensions.y;

		*m_windowedDimensions = newDimensions;

		if (m_isFullscreen)
			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP * m_isFullscreen);
		else
		{
			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP * m_isFullscreen);
			SDL_SetWindowSize(m_sdlWindow, m_windowedDimensions->x, m_windowedDimensions->y);
			SDL_SetWindowPosition(
				m_sdlWindow,
				SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitorIndex),
				SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitorIndex));
		}

		return true;
	}
	bool Window::Rename(const char*newTitle)
	{
		m_title = newTitle;
		SDL_SetWindowTitle(m_sdlWindow, m_title.c_str());

		return true;
	}

	bool Window::ToggleFullscreen()
	{
		m_isFullscreen = !m_isFullscreen;
		return Resize(*m_windowedDimensions, m_monitorIndex);
	}

	const UVector2 & Window::GetDimensions() const
	{
		return *m_dimensions;
	}
	const UVector2 & Window::GetWindowedDimensions() const
	{
		return *m_windowedDimensions;
	}
	const UVector2 & Window::GetResolution() const
	{
		return *m_resolution;
	}

	SDL_Window * Window::GetWindow() const
	{
		return m_sdlWindow;
	}
	const SDL_GLContext & Window::GetContext() const
	{
		return m_sdlGLContext;
	}

	bool Window::GetIsFullscreen() const
	{
		return m_isFullscreen;
	}

	void Window::Flip() const
	{
		SDL_GL_SwapWindow(m_sdlWindow);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::Window(
		const UVector2 &				resolution,
		const bool						isFullscreen,
		const UVector2 &				dimensions,
		const char *					title,
		const unsigned int				monitorIndex,
		const List<SDL_DisplayMode> &	displayMode) : m_sdlDisplayMode(displayMode)
	{
		m_monitorIndex = monitorIndex;

		m_dimensions->x = (isFullscreen) ? m_sdlDisplayMode[m_monitorIndex].w : dimensions.x;
		m_dimensions->y = (isFullscreen) ? m_sdlDisplayMode[m_monitorIndex].h : dimensions.y;

		*m_windowedDimensions = dimensions;

		*m_resolution = resolution;

		m_title = title;

		m_isFullscreen = isFullscreen;

		m_sdlWindow = SDL_CreateWindow(
			m_title.c_str(),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitorIndex),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_monitorIndex),
			m_windowedDimensions->x,
			m_windowedDimensions->y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP * m_isFullscreen | SDL_WINDOW_OPENGL);

		if (m_sdlWindow == nullptr)
		{
			printf("SDL_Error: %s\n", SDL_GetError());
			return;
		}

		m_sdlGLContext = SDL_GL_CreateContext(m_sdlWindow);
	}
	Window::~Window()
	{

	}
}
