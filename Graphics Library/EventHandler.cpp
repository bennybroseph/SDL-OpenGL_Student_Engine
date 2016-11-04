#include "include/EventHandler.h"



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