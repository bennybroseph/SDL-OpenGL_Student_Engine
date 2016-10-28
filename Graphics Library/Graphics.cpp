#include "include/Graphics.h"

#include <algorithm> // Holds the 'sort()' function

#include <SDL/SDL_image.h> // Required for loading images from a file

namespace Graphics
{
	std::vector<SDL_DisplayMode> sdlDisplayMode;
	SDL_GLContext glContext = nullptr;
	unsigned int uiNumDisplays;

	List<Window*> voWindows;
	List<Camera*> voCameras;

	std::vector<GLSurface*> glSurfaces;

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

		for (auto i = 0; i < uiNumDisplays; ++i)
		{
			SDL_DisplayMode sdlTemp;
			if (SDL_GetDisplayMode(i, i, &sdlTemp) != 0)
				printf("SDL_GetDisplayMode failed: %s", SDL_GetError());
			else
				sdlDisplayMode.push_back(sdlTemp);
		}

		return 0;
	}

	bool NewWindow(
		const UVector2 &	resolution,
		const bool			isFullscreen,
		const UVector2 &	dimensions,
		const char *		title,
		const unsigned		monitorIndex)
	{
		voWindows.push_back(new Window(
			resolution,
			isFullscreen,
			dimensions,
			title,
			monitorIndex,
			sdlDisplayMode));

		if (glContext == nullptr)
		{
			glContext = SDL_GL_CreateContext(voWindows[0]->GetWindow());

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			SDL_GL_SetSwapInterval(-1);

			glViewport(0, 0, dimensions.x, dimensions.y);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(0.0f, resolution.x, resolution.y, 0.0f, -1.0f, 1.0f);
		}

		return true;
	}

	void Resize(
		const UVector2 &	ac_uiNewDimensions,
		const unsigned int	ac_uiNewMonitorIndex,
		const unsigned int	ac_uiIndex)
	{
		ResizeCameras(ac_uiNewDimensions, ac_uiIndex);

		voWindows[ac_uiIndex]->Resize(ac_uiNewDimensions, ac_uiNewMonitorIndex);
	}
	void ToggleFullscreen(unsigned int ac_uiIndex)
	{
		UVector2 NewDimensions;
		NewDimensions.x = (!voWindows[ac_uiIndex]->GetIsFullscreen()) ? (unsigned int)sdlDisplayMode[ac_uiIndex].w : voWindows[ac_uiIndex]->GetWindowedDimensions().x;
		NewDimensions.y = (!voWindows[ac_uiIndex]->GetIsFullscreen()) ? (unsigned int)sdlDisplayMode[ac_uiIndex].h : voWindows[ac_uiIndex]->GetWindowedDimensions().y;

		ResizeCameras(NewDimensions, ac_uiIndex);

		voWindows[ac_uiIndex]->ToggleFullscreen();
	}

	void NewCamera(
		const Vector2 &	ac_ScreenPos,
		const Vector2 &	ac_WorldPos,
		const Vector2 &	ac_RelativePos,
		const Vector2 &	ac_Dimensions,
		const Vector2 &	ac_Zoom,
		float			ac_Rotation,
		const bool		ac_bIsScrolling,
		const Vector2 &	ac_Velocity,
		unsigned int	ac_uiWindowIndex,
		unsigned int	ac_uiWorldSpace)
	{
		Vector2 SizeOffset ={ ac_Dimensions.x * (float)voWindows[ac_uiWindowIndex]->GetDimensions().x / 100, ac_Dimensions.y * (float)voWindows[ac_uiWindowIndex]->GetDimensions().y / 100 };
		Vector2 ScreenOffset ={
			ac_ScreenPos.x * (float(voWindows[ac_uiWindowIndex]->GetDimensions().x) - SizeOffset.x) / 100,
			abs(ac_ScreenPos.y - 100) * (float(voWindows[ac_uiWindowIndex]->GetDimensions().y) - SizeOffset.y) / 100 };

		Vector2 Resolution ={
			(float)voWindows[ac_uiWindowIndex]->GetResolution().x * ((float)SizeOffset.x / (float)voWindows[ac_uiWindowIndex]->GetDimensions().x),
			(float)voWindows[ac_uiWindowIndex]->GetResolution().y * ((float)SizeOffset.y / (float)voWindows[ac_uiWindowIndex]->GetDimensions().y) };

		Camera* newCamera = new Camera(ScreenOffset, ac_WorldPos, ac_RelativePos, SizeOffset, Resolution, ac_Zoom, ac_Rotation, ac_bIsScrolling, ac_Velocity, ac_uiWindowIndex, ac_uiWorldSpace);
		PushCamera(newCamera);
	}
	void ResizeCameras(const UVector2& ac_uiDimensions, unsigned int ac_uiIndex)
	{
		for (auto camera : voCameras)
		{
			if (camera->GetWindowIndex() == ac_uiIndex)
			{
				Vector2 Scale ={
					(float)voWindows[camera->GetWindowIndex()]->GetDimensions().x / (float)camera->GetDimensions().x * 100,
					(float)voWindows[camera->GetWindowIndex()]->GetDimensions().y / (float)camera->GetDimensions().y * 100 };

				Vector2 SizeOffset ={ Scale.x * (float)ac_uiDimensions.x / 100, Scale.y * (float)ac_uiDimensions.y / 100 };

				camera->Resize(SizeOffset);
			}
		}
	}

	void Draw()
	{
		UpdateCameras();
	}

	void UpdateCameras()
	{
		for (auto camera : voCameras)
		{
			camera->Update();

			SDL_GL_MakeCurrent(voWindows[camera->GetWindowIndex()]->GetWindow(), glContext);

			glViewport(
				camera->GetScreenPos().x,
				camera->GetScreenPos().y,
				camera->GetDimensions().x,
				camera->GetDimensions().y);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(
				0.0f,
				camera->GetResolution().x,
				camera->GetResolution().y,
				0.0f, -1.0f, 1.0f);

			for (auto surface : glSurfaces)
				if (surface->bIsActive && camera->GetWorldSpace() == surface->uiWorldSpace)
					DrawSurface(*surface, *camera);
		}
	}

	void DrawSurface(const GLSurface& ac_glSurface, Camera& a_Camera)
	{
		glPushMatrix(); // Save the current matrix.

		glTranslatef(
			ac_glSurface.Pos.x + (ac_glSurface.Center.x - ac_glSurface.OffsetD.x / 2),
			ac_glSurface.Pos.y + (ac_glSurface.Center.y - ac_glSurface.OffsetD.y / 2),
			0.0f);
		glScalef(ac_glSurface.Scale.x, ac_glSurface.Scale.y, 0.0f);
		glRotatef(ac_glSurface.Rotation, 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-ac_glSurface.Pos.x - (ac_glSurface.Center.x - ac_glSurface.OffsetD.x / 2),
			-ac_glSurface.Pos.y - (ac_glSurface.Center.y - ac_glSurface.OffsetD.y / 2), 0.0f);

		glTranslatef(a_Camera.GetResolution().x / 2, a_Camera.GetResolution().y / 2, 0.0f);
		glScalef(a_Camera.GetZoom().x, a_Camera.GetZoom().y, 0.0f);
		glRotatef(a_Camera.GetRotation(), 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-a_Camera.GetWorldPos().x,
			-a_Camera.GetWorldPos().y, 0.0f);

		GLfloat glVertices[] ={
			(float)ac_glSurface.OffsetP.x / (float)ac_glSurface.Dimensions.x,
			(float)ac_glSurface.OffsetP.y / (float)ac_glSurface.Dimensions.y,

			((float)ac_glSurface.OffsetP.x / (float)ac_glSurface.Dimensions.x) + ((float)ac_glSurface.OffsetD.x / (float)ac_glSurface.Dimensions.x),
			(float)ac_glSurface.OffsetP.y / (float)ac_glSurface.Dimensions.y,

			((float)ac_glSurface.OffsetP.x / (float)ac_glSurface.Dimensions.x) + ((float)ac_glSurface.OffsetD.x / (float)ac_glSurface.Dimensions.x),
			((float)ac_glSurface.OffsetP.y / (float)ac_glSurface.Dimensions.y) + ((float)ac_glSurface.OffsetD.y / (float)ac_glSurface.Dimensions.y),

			(float)ac_glSurface.OffsetP.x / (float)ac_glSurface.Dimensions.x,
			((float)ac_glSurface.OffsetP.y / (float)ac_glSurface.Dimensions.y) + ((float)ac_glSurface.OffsetD.y / (float)ac_glSurface.Dimensions.y)
		};

		GLfloat glPosition[] ={
			ac_glSurface.Pos.x - (ac_glSurface.OffsetD.x / 2),
			ac_glSurface.Pos.y - (ac_glSurface.OffsetD.y / 2),

			ac_glSurface.Pos.x + (ac_glSurface.OffsetD.x / 2),
			ac_glSurface.Pos.y - (ac_glSurface.OffsetD.y / 2),

			ac_glSurface.Pos.x + (ac_glSurface.OffsetD.x / 2),
			ac_glSurface.Pos.y + (ac_glSurface.OffsetD.y / 2),

			ac_glSurface.Pos.x - (ac_glSurface.OffsetD.x / 2),
			ac_glSurface.Pos.y + (ac_glSurface.OffsetD.y / 2)
		};

		glBindTexture(GL_TEXTURE_2D, ac_glSurface.Surface);
		glColor4ub(ac_glSurface.Color.r, ac_glSurface.Color.g, ac_glSurface.Color.b, ac_glSurface.Color.a);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, glVertices);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glPosition);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}

	bool SortLayer(GLSurface* ac_pglLeft, GLSurface* ac_pglRight)
	{
		if (ac_pglLeft->uiWorldSpace == ac_pglRight->uiWorldSpace)
			return ac_pglLeft->Layer < ac_pglRight->Layer;

		return false;
	}
	bool SortCamera(GLSurface* ac_pglLeft, GLSurface* ac_pglRight)
	{
		return ac_pglLeft->uiWorldSpace < ac_pglRight->uiWorldSpace;
	}

	GLSurface* LoadSurface(const char* ac_szFilename)
	{
		SDL_Surface* sdlSurface;

		sdlSurface = IMG_Load(ac_szFilename);
		if (sdlSurface == nullptr)
		{
			printf("SDL_Error: %s\n", SDL_GetError());

			GLSurface *glSurface = nullptr;
			return glSurface;
		}

		return LoadSurface(*sdlSurface);
	}
	GLSurface* LoadSurface(SDL_Surface& a_sdlSurface)
	{
		GLSurface* glSurface = new GLSurface;

		glGenTextures(1, &glSurface->Surface);
		glBindTexture(GL_TEXTURE_2D, glSurface->Surface);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_sdlSurface.w, a_sdlSurface.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_sdlSurface.pixels);

		glSurface->Pos ={ NULL, NULL };
		glSurface->OffsetP ={ NULL, NULL };

		glSurface->Dimensions.x = a_sdlSurface.w;
		glSurface->Dimensions.y = a_sdlSurface.h;

		glSurface->Center.x = glSurface->Dimensions.x / 2.0f;
		glSurface->Center.y = glSurface->Dimensions.y / 2.0f;

		glSurface->OffsetD.x = glSurface->Dimensions.x;
		glSurface->OffsetD.y = glSurface->Dimensions.y;

		glSurface->Rotation = NULL;
		glSurface->Scale ={ 1, 1 };

		glSurface->Color ={ 255, 255, 255, 255 };

		glSurface->Layer = LayerType::BACKGROUND;

		glSurface->uiWorldSpace = 0;

		glSurface->bIsActive = true;

		SDL_FreeSurface(&a_sdlSurface);

		PushSurface(glSurface);
		std::sort(glSurfaces.begin(), glSurfaces.end(), SortCamera);
		std::sort(glSurfaces.begin(), glSurfaces.end(), SortLayer);

		return glSurface;
	}

	void PushSurface(GLSurface* a_glSurface)
	{
		glSurfaces.push_back(a_glSurface);
	}

	void PushCamera(Camera* a_Camera)
	{
		voCameras.push_back(a_Camera);
	}

	void DrawRect(const Vector2 &ac_Pos, const Vector2 &ac_Size, const Vector4 &ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.r, ac_Color.g, ac_Color.b, ac_Color.a);
		glBegin(GL_QUADS);
		{
			//Bottom-left vertex (corner)
			glVertex3f(ac_Pos.x, ac_Pos.y, 0.0f); //Vertex Coords

												  //Bottom-right vertex (corner)
			glVertex3f(ac_Pos.x + ac_Size.x, ac_Pos.y, 0.f);

			//Top-right vertex (corner)
			glVertex3f(ac_Pos.x + ac_Size.x, ac_Pos.y + ac_Size.y, 0.f);

			//Top-left vertex (corner)
			glVertex3f(ac_Pos.x, ac_Pos.y + ac_Size.y, 0.f);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawLine(const Vector2 &ac_Begin, const Vector2 &ac_End, const Vector4 &ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.r, ac_Color.g, ac_Color.b, ac_Color.a);

		glBegin(GL_LINES);
		{
			glVertex2f(ac_Begin.x, ac_Begin.y);
			glVertex2f(ac_End.x, ac_End.y);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawPoint(const Vector2& ac_Pos, const Vector4 &ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.r, ac_Color.g, ac_Color.b, ac_Color.a);

		glBegin(GL_POINTS);
		glVertex2f(ac_Pos.x, ac_Pos.y);
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawRing(const Vector2 &ac_Center, float ac_Radius, float ac_Quality, const Vector4 &ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.r, ac_Color.g, ac_Color.b, ac_Color.a);

		glBegin(GL_LINE_LOOP);
		{
			for (int i = 0; i <= ac_Quality; ++i)
			{
				glVertex2f(
					ac_Center.x + cosf(i * (PI * 2) / ac_Quality)*ac_Radius,
					ac_Center.y + sinf(i * (PI * 2) / ac_Quality)*ac_Radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawCircle(const Vector2 &ac_Center, float ac_Radius, float ac_Quality, const Vector4 &ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.r, ac_Color.g, ac_Color.b, ac_Color.a);

		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex2f(ac_Center.x, ac_Center.y); // center of circle
			for (int i = 0; i <= ac_Quality; ++i)
			{
				glVertex2f(
					ac_Center.x + cosf(i * (PI * 2) / ac_Quality)*ac_Radius,
					ac_Center.y + sinf(i * (PI * 2) / ac_Quality)*ac_Radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}

	void Flip()
	{
		for (auto window : voWindows)
		{
			SDL_GL_MakeCurrent(window->GetWindow(), glContext);
			window->Flip();
		}
	}

	void Quit()
	{
		voWindows.clear();
		voCameras.clear();
		glSurfaces.clear();
	}
}
