#include "include\EventHandler.h"



EventHandler::EventHandler()
{

}

EventHandler::~EventHandler()
{
	// Pure virtual, do nothing 
}

void EventHandler::OnEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	// Windows kinds of events
	case SDL_WINDOWEVENT:
	{
		switch (event.window.event)
		{
		case SDL_WINDOWEVENT_ENTER: OnMouseFocus(); break;
		case SDL_WINDOWEVENT_LEAVE: OnMouseBlur(); break;

		case SDL_WINDOWEVENT_FOCUS_GAINED: OnInputFocus(); break;
		case SDL_WINDOWEVENT_FOCUS_LOST: OnInputBlur(); break;

		case SDL_WINDOWEVENT_MINIMIZED: OnRestore(); break;
		case SDL_WINDOWEVENT_RESTORED: OnMinimize(); break;

		case SDL_WINDOWEVENT_RESIZED: OnResize(event.window.data1, event.window.data2); break;

		case SDL_WINDOWEVENT_EXPOSED: OnExpose(); break;
		}
		break;
	}

	case SDL_KEYDOWN: OnKeyDown(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.scancode); break;
	case SDL_KEYUP: OnKeyUp(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.scancode); break;

	case SDL_MOUSEMOTION:
	{
		OnMouseMove(
			event.motion.x,
			event.motion.y,
			event.motion.xrel,
			event.motion.yrel,
			(event.motion.state&SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
			(event.motion.state&SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
			(event.motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT: OnLButtonDown(event.button.x, event.button.y); break;
		case SDL_BUTTON_RIGHT: OnRButtonDown(event.button.x, event.button.y); break;
		case SDL_BUTTON_MIDDLE:  OnMButtonDown(event.button.x, event.button.y); break;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT: OnLButtonUp(event.button.x, event.button.y); break;
		case SDL_BUTTON_RIGHT: OnRButtonUp(event.button.x, event.button.y); break;
		case SDL_BUTTON_MIDDLE: OnMButtonUp(event.button.x, event.button.y); break;
		}
		break;
	}

	case SDL_JOYAXISMOTION: OnJoyAxis(event.jaxis.which, event.jaxis.axis, event.jaxis.value); break;

	case SDL_JOYBALLMOTION: OnJoyBall(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel); break;

	case SDL_JOYHATMOTION: OnJoyHat(event.jhat.which, event.jhat.hat, event.jhat.value); break;

	case SDL_JOYBUTTONDOWN: OnJoyButtonDown(event.jbutton.which, event.jbutton.button); break;
	case SDL_JOYBUTTONUP: OnJoyButtonUp(event.jbutton.which, event.jbutton.button); break;

	case SDL_QUIT: OnExit(); break;

	case SDL_SYSWMEVENT: break;

	// No clue
	default: OnUser(event.user.type, event.user.code, event.user.data1, event.user.data2);
		break;
	}
}

// All these remain empty because child classes will redefine them themselves
void EventHandler::OnInputFocus()
{
	// Pure virtual, do nothing
}

void EventHandler::OnInputBlur()
{
	// Pure virtual, do nothing
}

void EventHandler::OnKeyDown(const SDL_Keycode ac_sdlSym, const Uint16 ac_uiMod, const SDL_Scancode ac_sdlScancode)
{
	// Pure virtual, do nothing
}

void EventHandler::OnKeyUp(const SDL_Keycode ac_sdlSym, const Uint16 ac_uiMod, const SDL_Scancode ac_sdlScancode)
{
	// Pure virtual, do nothing
}

void EventHandler::OnMouseFocus()
{
	// Pure virtual, do nothing
}

void EventHandler::OnMouseBlur()
{
	// Pure virtual, do nothing
}

void EventHandler::OnMouseMove(const int ac_iMouseX, const int ac_iMouseY, const int ac_iVelX, const int ac_VelY, const bool ac_bLeft, const bool ac_bRight, const bool ac_bMiddle)
{
	// Pure virtual, do nothing
}

void EventHandler::OnMouseWheel(const bool ac_bUp, const bool ac_bDown)
{
	// Pure virtual, do nothing
}

void EventHandler::OnLButtonDown(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnLButtonUp(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnRButtonDown(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnRButtonUp(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnMButtonDown(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnMButtonUp(const int ac_MouseX, const int ac_MouseY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnJoyAxis(const Uint8 ac_uiWhich, const Uint8 ac_uiAxis, const Sint16 ac_uiValue)
{
	// Pure virtual, do nothing
}

void EventHandler::OnJoyButtonDown(const Uint8 ac_uiWhich, const Uint8 ac_uiButton)
{
	// Pure virtual, do nothing
}

void EventHandler::OnJoyButtonUp(const Uint8 ac_uiWhich, const Uint8 ac_uiButton)
{
	// Pure virtual, do nothing
}

void EventHandler::OnJoyHat(const Uint8 ac_uiWhich, const Uint8 ac_uiHat, const Uint8 ac_uiValue)
{
	// Pure virtual, do nothing
}

void EventHandler::OnJoyBall(const Uint8 ac_uiWhich, const Uint8 ac_uiBall, const Sint16 ac_uiRelX, const Sint16 ac_uiRelY)
{
	// Pure virtual, do nothing
}

void EventHandler::OnMinimize()
{
	// Pure virtual, do nothing
}

void EventHandler::OnRestore()
{
	// Pure virtual, do nothing
}

void EventHandler::OnResize(const int ac_iWidth, const int ac_iHeight)
{
	// Pure virtual, do nothing
}

void EventHandler::OnExpose()
{
	// Pure virtual, do nothing
}

void EventHandler::OnExit()
{
	// Pure virtual, do nothing
}

void EventHandler::OnUser(const Uint8 ac_uiType, const int ac_iCode, const void* ac_pData1, const void* ac_pData2)
{
	// Pure virtual, do nothing
}
