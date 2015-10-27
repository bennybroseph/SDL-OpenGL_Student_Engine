#include "include\Window.h"



namespace Graphics
{
	void Window::Resize(const System::Size2D<unsigned int>& ac_uiNewDimensions, const unsigned int ac_uiNewMonitorIndex)
	{
		m_uiMonitorIndex = ac_uiNewMonitorIndex;

		m_uiDimensions.W = (m_bIsFullscreen) ? m_sdlDisplayMode[m_uiMonitorIndex].w : ac_uiNewDimensions.W;
		m_uiDimensions.H = (m_bIsFullscreen) ? m_sdlDisplayMode[m_uiMonitorIndex].h : ac_uiNewDimensions.H;

		m_uiNonFullscreen = ac_uiNewDimensions;

		if(m_bIsFullscreen)
			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP*m_bIsFullscreen);
		else
		{
			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP*m_bIsFullscreen);
			SDL_SetWindowSize(m_sdlWindow, m_uiNonFullscreen.W, m_uiNonFullscreen.H);
			SDL_SetWindowPosition(m_sdlWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(m_uiMonitorIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(m_uiMonitorIndex));
		}
	}
	void Window::Rename(const char* ac_szNewTitle)
	{
		m_sTitle = ac_szNewTitle;
		SDL_SetWindowTitle(m_sdlWindow, m_sTitle.c_str());
	}

	void Window::ToggleFullscreen()
	{
		m_bIsFullscreen = !m_bIsFullscreen;
		Resize(m_uiNonFullscreen, m_uiMonitorIndex);
	}

	const System::Size2D<unsigned int>& Window::GetDimensions()
	{
		return m_uiDimensions;
	}
	const System::Size2D<unsigned int>& Window::GetNonFullscreen()
	{
		return m_uiNonFullscreen;
	}

	SDL_Window* Window::GetWindow()
	{
		return m_sdlWindow;
	}

	const System::Size2D<unsigned int>& Window::GetResolution()
	{
		return m_uiResolution;
	}

	const bool Window::GetIsFullscreen()
	{
		return m_bIsFullscreen;
	}

	void Window::Flip()
	{
		SDL_GL_SwapWindow(m_sdlWindow);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::Window(
		const System::Size2D<unsigned int>& ac_uiResolution,
		const bool							ac_bFullscreen,
		const System::Size2D<unsigned int>& ac_uiDimensions,
		const char*							ac_szTitle,
		const unsigned int					ac_uiMonitorIndex,
		const std::vector<SDL_DisplayMode>&	ac_sdlDisplayMode) : m_sdlDisplayMode(ac_sdlDisplayMode)
	{
		m_uiMonitorIndex = ac_uiMonitorIndex;

		m_uiDimensions.W = (ac_bFullscreen) ? m_sdlDisplayMode[m_uiMonitorIndex].w : ac_uiDimensions.W;
		m_uiDimensions.H = (ac_bFullscreen) ? m_sdlDisplayMode[m_uiMonitorIndex].h : ac_uiDimensions.H;

		m_uiNonFullscreen = ac_uiDimensions;

		m_uiResolution = ac_uiResolution;

		m_sTitle = ac_szTitle;

		m_bIsFullscreen = ac_bFullscreen;

		m_sdlWindow = SDL_CreateWindow(
			m_sTitle.c_str(),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_uiMonitorIndex),
			SDL_WINDOWPOS_CENTERED_DISPLAY(m_uiMonitorIndex),
			m_uiNonFullscreen.W,
			m_uiNonFullscreen.H,
			SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP*m_bIsFullscreen | SDL_WINDOW_OPENGL);

		if (m_sdlWindow == nullptr)
			printf("SDL_Error: %s\n", SDL_GetError());
	}
	Window::~Window()
	{

	}
}