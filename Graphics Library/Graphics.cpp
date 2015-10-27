#include "include\Graphics.h"



namespace Graphics
{
	std::vector<SDL_DisplayMode> sdlDisplayMode;
	SDL_GLContext glContext = nullptr;
	unsigned int uiNumDisplays;

	std::vector<Window*>		voWindows;
	std::vector<CameraUnion*>	voCameras;

	std::vector<SurfaceUnion*> vglSurfaces;

	bool Init()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL_Error: %s\n", SDL_GetError());
			return 1;
		}

		if ((uiNumDisplays = SDL_GetNumVideoDisplays()) < 1)
		{
			SDL_Log("SDL_GetNumVideoDisplays returned: %i", uiNumDisplays);
			return 1;
		}

		for (int i = 0; i < uiNumDisplays; ++i)
		{
			SDL_DisplayMode sdlTemp;
			if (SDL_GetDisplayMode(i, i, &sdlTemp) != 0)
				printf("SDL_GetDisplayMode failed: %s", SDL_GetError());
			else
				sdlDisplayMode.push_back(sdlTemp);
		}

		return 0;
	}

	void NewWindow(
		const System::Size2D<unsigned int>& ac_iResolution,
		const bool							ac_bFullscreen,
		const System::Size2D<unsigned int>& ac_iDimensions,
		const char*							ac_szTitle,
		const unsigned int					ac_uiMonitorIndex)
	{
		voWindows.push_back(new Window(
			ac_iResolution,
			ac_bFullscreen,
			ac_iDimensions,
			ac_szTitle,
			ac_uiMonitorIndex,
			sdlDisplayMode));

		if (glContext == nullptr)
		{
			glContext = SDL_GL_CreateContext(voWindows[0]->GetWindow());

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			SDL_GL_SetSwapInterval(-1);

			glViewport(0, 0, ac_iDimensions.W, ac_iDimensions.H);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho( 0.0f, ac_iResolution.W, ac_iResolution.H, 0.0f, -1.0f, 1.0f);
		}
	}

	void Resize(
		const System::Size2D<unsigned int>& ac_uiNewDimensions,
		const unsigned int					ac_uiNewMonitorIndex,
		const unsigned int					ac_uiIndex)
	{
		for (int i = 0; i < voCameras.size(); ++i)
		{
			switch (voCameras[i]->Tag)
			{
			case CameraUnion::INT: ResizeCameras(*voCameras[i]->iCamera, ac_uiNewDimensions, ac_uiIndex); break;
			case CameraUnion::FLOAT: ResizeCameras(*voCameras[i]->fCamera, ac_uiNewDimensions, ac_uiIndex); break;
			}
		}
		voWindows[ac_uiIndex]->Resize(ac_uiNewDimensions, ac_uiNewMonitorIndex);
	}
	void ToggleFullscreen(const unsigned int ac_uiIndex)
	{
		System::Size2D<unsigned int> NewDimensions;
		NewDimensions.W = (!voWindows[ac_uiIndex]->GetIsFullscreen()) ? (unsigned int)sdlDisplayMode[ac_uiIndex].w : voWindows[ac_uiIndex]->GetNonFullscreen().W;
		NewDimensions.H = (!voWindows[ac_uiIndex]->GetIsFullscreen()) ? (unsigned int)sdlDisplayMode[ac_uiIndex].h : voWindows[ac_uiIndex]->GetNonFullscreen().H;

		for (int i = 0; i < voCameras.size(); ++i)
		{
			switch (voCameras[i]->Tag)
			{
			case CameraUnion::INT: ResizeCameras(*voCameras[i]->iCamera, NewDimensions, ac_uiIndex); break;
			case CameraUnion::FLOAT: ResizeCameras(*voCameras[i]->fCamera, NewDimensions, ac_uiIndex); break;
			}
		}

		voWindows[ac_uiIndex]->ToggleFullscreen();
	}

	template <typename T>
	void ResizeCameras(Camera<T>& a_Camera, const System::Size2D<unsigned int>& ac_uiDimensions, const unsigned int ac_uiIndex)
	{
		if (a_Camera.GetWindowIndex() == ac_uiIndex)
		{
			System::Size2D<T> Scale = {
				((float)voWindows[a_Camera.GetWindowIndex()]->GetDimensions().W / (float)a_Camera.GetDimensions().W) * 100,
				((float)voWindows[a_Camera.GetWindowIndex()]->GetDimensions().H / (float)a_Camera.GetDimensions().H) * 100 };

			System::Size2D<T> SizeOffset = { Scale.W * (T)ac_uiDimensions.W / 100, Scale.H * (T)ac_uiDimensions.H / 100 };

			a_Camera.Resize(SizeOffset);
		}
	}

	void Draw()
	{
		for (int i = 0; i < voCameras.size(); ++i)
		{
			switch (voCameras[i]->Tag)
			{
			case CameraUnion::INT:	 UpdateCameras(*voCameras[i]->iCamera); break;
			case CameraUnion::FLOAT: UpdateCameras(*voCameras[i]->fCamera); break;
			}
		}
	}

	template <typename T>
	void UpdateCameras(Camera<T>& a_Camera)
	{
		a_Camera.Update();

		SDL_GL_MakeCurrent(voWindows[a_Camera.GetWindowIndex()]->GetWindow(), glContext);

		glViewport(
			a_Camera.GetScreenPos().X,
			a_Camera.GetScreenPos().Y,
			a_Camera.GetDimensions().W,
			a_Camera.GetDimensions().H);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(
			0.0f,
			a_Camera.GetResolution().W,
			a_Camera.GetResolution().H,
			0.0f, -1.0f, 1.0f);

		for (int i = 0; i < vglSurfaces.size(); ++i)
		{
			switch (vglSurfaces[i]->Tag)
			{
			case SurfaceUnion::INT:
			{
				if (vglSurfaces[i]->iGLSurface->bIsActive && a_Camera.GetWorldSpace() == vglSurfaces[i]->iGLSurface->uiWorldSpace)
				{
					DrawSurface(*vglSurfaces[i]->iGLSurface, a_Camera);
				}
				break;
			}
			case SurfaceUnion::FLOAT:
			{
				if (vglSurfaces[i]->fGLSurface->bIsActive && a_Camera.GetWorldSpace() == vglSurfaces[i]->fGLSurface->uiWorldSpace)
				{
					DrawSurface(*vglSurfaces[i]->fGLSurface, a_Camera);
				}
				break;
			}
			}
		}
	}

	template <typename T, typename U>
	void DrawSurface(const GLSurface<T>& ac_glSurface, Camera<U>& a_Camera)
	{
		glPushMatrix(); // Save the current matrix.

		glTranslatef(
			ac_glSurface.Pos.X + (ac_glSurface.Center.X - ac_glSurface.OffsetD.W / 2),
			ac_glSurface.Pos.Y + (ac_glSurface.Center.Y - ac_glSurface.OffsetD.H / 2),
			0.0f);
		glScalef(ac_glSurface.Scale.W, ac_glSurface.Scale.H, 0.0f);
		glRotatef(ac_glSurface.Rotation, 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-ac_glSurface.Pos.X - (ac_glSurface.Center.X - ac_glSurface.OffsetD.W / 2),
			-ac_glSurface.Pos.Y - (ac_glSurface.Center.Y - ac_glSurface.OffsetD.H / 2), 0.0f);

		glTranslatef(a_Camera.GetResolution().W / 2, a_Camera.GetResolution().H / 2, 0.0f);
		glScalef(a_Camera.GetZoom().W, a_Camera.GetZoom().H, 0.0f);
		glRotatef(a_Camera.GetRotation(), 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-a_Camera.GetWorldPos().X,
			-a_Camera.GetWorldPos().Y, 0.0f);

		GLfloat glVertices[] = {
			(float)ac_glSurface.OffsetP.X / (float)ac_glSurface.Dimensions.W,
			(float)ac_glSurface.OffsetP.Y / (float)ac_glSurface.Dimensions.H,

			((float)ac_glSurface.OffsetP.X / (float)ac_glSurface.Dimensions.W) + ((float)ac_glSurface.OffsetD.W / (float)ac_glSurface.Dimensions.W),
			(float)ac_glSurface.OffsetP.Y / (float)ac_glSurface.Dimensions.H,

			((float)ac_glSurface.OffsetP.X / (float)ac_glSurface.Dimensions.W) + ((float)ac_glSurface.OffsetD.W / (float)ac_glSurface.Dimensions.W),
			((float)ac_glSurface.OffsetP.Y / (float)ac_glSurface.Dimensions.H) + ((float)ac_glSurface.OffsetD.H / (float)ac_glSurface.Dimensions.H),

			(float)ac_glSurface.OffsetP.X / (float)ac_glSurface.Dimensions.W,
			((float)ac_glSurface.OffsetP.Y / (float)ac_glSurface.Dimensions.H) + ((float)ac_glSurface.OffsetD.H / (float)ac_glSurface.Dimensions.H)
		};

		GLfloat glPosition[] = {
			ac_glSurface.Pos.X - (ac_glSurface.OffsetD.W / 2),
			ac_glSurface.Pos.Y - (ac_glSurface.OffsetD.H / 2),

			ac_glSurface.Pos.X + (ac_glSurface.OffsetD.W / 2),
			ac_glSurface.Pos.Y - (ac_glSurface.OffsetD.H / 2),

			ac_glSurface.Pos.X + (ac_glSurface.OffsetD.W / 2),
			ac_glSurface.Pos.Y + (ac_glSurface.OffsetD.H / 2),

			ac_glSurface.Pos.X - (ac_glSurface.OffsetD.W / 2),
			ac_glSurface.Pos.Y + (ac_glSurface.OffsetD.H / 2)
		};

		glBindTexture(GL_TEXTURE_2D, ac_glSurface.Surface);
		glColor4ub(ac_glSurface.Color.Red, ac_glSurface.Color.Green, ac_glSurface.Color.Blue, ac_glSurface.Color.Alpha);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, glVertices);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glPosition);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}

	bool SortLayer(SurfaceUnion* ac_pglLeft, SurfaceUnion* ac_pglRight)
	{
		switch (ac_pglLeft->Tag)
		{
		case SurfaceUnion::INT:
		{
			switch (ac_pglRight->Tag)
			{
			case SurfaceUnion::INT:
			{
				if (ac_pglLeft->iGLSurface->uiWorldSpace == ac_pglRight->iGLSurface->uiWorldSpace)
					return ac_pglLeft->iGLSurface->Layer < ac_pglRight->iGLSurface->Layer;
				break;
			}
			case SurfaceUnion::FLOAT:
			{
				if (ac_pglLeft->iGLSurface->uiWorldSpace == ac_pglRight->fGLSurface->uiWorldSpace)
					return ac_pglLeft->iGLSurface->Layer < ac_pglRight->fGLSurface->Layer;
				break;
			}
			}
			break;
		}
		case SurfaceUnion::FLOAT:
		{
			switch (ac_pglRight->Tag)
			{
			case SurfaceUnion::INT:
			{
				if (ac_pglLeft->fGLSurface->uiWorldSpace == ac_pglRight->iGLSurface->uiWorldSpace)
					return ac_pglLeft->fGLSurface->Layer < ac_pglRight->iGLSurface->Layer;
				break;
			}
			case SurfaceUnion::FLOAT:
			{
				if (ac_pglLeft->fGLSurface->uiWorldSpace == ac_pglRight->fGLSurface->uiWorldSpace)
					return ac_pglLeft->fGLSurface->Layer < ac_pglRight->fGLSurface->Layer;
				break;
			}
			}
			break;
		}
		}

		return false;
	}
	bool SortCamera(SurfaceUnion* ac_pglLeft, SurfaceUnion* ac_pglRight)
	{
		switch (ac_pglLeft->Tag)
		{
		case SurfaceUnion::INT:
		{
			switch (ac_pglRight->Tag)
			{
			case SurfaceUnion::INT:	  return ac_pglLeft->iGLSurface->uiWorldSpace < ac_pglRight->iGLSurface->uiWorldSpace; break;
			case SurfaceUnion::FLOAT: return ac_pglLeft->iGLSurface->uiWorldSpace < ac_pglRight->fGLSurface->uiWorldSpace; break;
			}
			break;
		}
		case SurfaceUnion::FLOAT:
		{
			switch (ac_pglRight->Tag)
			{
			case SurfaceUnion::INT:	  return ac_pglLeft->fGLSurface->uiWorldSpace < ac_pglRight->iGLSurface->uiWorldSpace; break;
			case SurfaceUnion::FLOAT: return ac_pglLeft->fGLSurface->uiWorldSpace < ac_pglRight->fGLSurface->uiWorldSpace; break;
			}
			break;
		}
		}
	}

	void PushSurface(GLSurface<int>* a_glSurface)
	{
		SurfaceUnion* newSurface = new SurfaceUnion;

		newSurface->Tag = SurfaceUnion::INT;
		newSurface->iGLSurface = a_glSurface;

		vglSurfaces.push_back(newSurface);
	}
	void PushSurface(GLSurface<float>* a_glSurface)
	{
		SurfaceUnion* newSurface = new SurfaceUnion;

		newSurface->Tag = SurfaceUnion::FLOAT;
		newSurface->fGLSurface = a_glSurface;

		vglSurfaces.push_back(newSurface);
	}

	void PushCamera(Camera<int>* a_Camera)
	{
		CameraUnion* newCameraUnion = new CameraUnion;

		newCameraUnion->Tag = CameraUnion::INT;
		newCameraUnion->iCamera = a_Camera;

		voCameras.push_back(newCameraUnion);
	}
	void PushCamera(Camera<float>* a_Camera)
	{
		CameraUnion* newCameraUnion = new CameraUnion;

		newCameraUnion->Tag = CameraUnion::FLOAT;
		newCameraUnion->fCamera = a_Camera;

		voCameras.push_back(newCameraUnion);
	}

	void Flip()
	{
		for (int i = 0; i < voWindows.size(); ++i)
		{
			SDL_GL_MakeCurrent(voWindows[i]->GetWindow(), glContext);
			voWindows[i]->Flip();
		}
	}

	void Quit()
	{
		voWindows.clear();
		voCameras.clear();
		vglSurfaces.clear();
	}
}