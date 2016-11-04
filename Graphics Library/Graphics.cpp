#include "include/Graphics.h"

#include <algorithm> // Holds the 'sort()' function

#include <SDL/SDL_image.h> // Required for loading images from a file

namespace Graphics
{
	std::vector<SDL_DisplayMode> sdlDisplayMode;
	SDL_GLContext glContext = nullptr;
	unsigned int uiNumDisplays;

	List<Window*> windows;
	List<Camera*> cameras;

	std::vector<GLSurface *> glSurfaces;

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
		const UVector2 &	dimensions,
		bool				isFullscreen,
		const char *		title,
		unsigned			monitorIndex)
	{
		windows.push_back(new Window(
			resolution,
			isFullscreen,
			dimensions,
			title,
			monitorIndex,
			sdlDisplayMode));

		if (glContext == nullptr)
		{
			glContext = SDL_GL_CreateContext(windows[0]->GetWindow());

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
		const UVector2 &	newDimensions,
		unsigned 			index,
		unsigned 			newMonitorIndex)
	{
		ResizeCameras(newDimensions, index);

		windows[index]->Resize(newDimensions, newMonitorIndex);
	}

	void ToggleFullscreen(unsigned index)
	{
		UVector2 newDimensions;

		newDimensions.x =
			!windows[index]->GetIsFullscreen()
			? static_cast<unsigned>(sdlDisplayMode[index].w)
			: windows[index]->GetWindowedDimensions().x;

		newDimensions.y =
			!windows[index]->GetIsFullscreen()
			? static_cast<unsigned>(sdlDisplayMode[index].h)
			: windows[index]->GetWindowedDimensions().y;

		ResizeCameras(newDimensions, index);

		windows[index]->ToggleFullscreen();
	}

	void NewCamera(
		const Vector2 &	screenPosition,
		const Vector2 &	worldPosition,
		const Vector2 &	relativePosition,
		const Vector2 &	size,
		const Vector2 &	zoom,
		float			rotation,
		bool			isScrolling,
		const Vector2 &	velocity,
		unsigned 		windowIndex,
		unsigned		worldSpace)
	{
		Vector2 sizeOffset(
			size.x * static_cast<float>(windows[windowIndex]->GetDimensions().x) / 100,
			size.y * static_cast<float>(windows[windowIndex]->GetDimensions().y) / 100);

		Vector2 screenOffset(
			screenPosition.x * (float(windows[windowIndex]->GetDimensions().x) - sizeOffset.x) / 100,

			abs(screenPosition.y - 100)
			* (float(windows[windowIndex]->GetDimensions().y) - sizeOffset.y) / 100);

		Vector2 resolution(
			static_cast<float>(windows[windowIndex]->GetResolution().x)
			* (static_cast<float>(sizeOffset.x)
				/ static_cast<float>(windows[windowIndex]->GetDimensions().x)),

			static_cast<float>(windows[windowIndex]->GetResolution().y)
			* (static_cast<float>(sizeOffset.y)
				/ static_cast<float>(windows[windowIndex]->GetDimensions().y)));

		auto newCamera =
			new Camera(
				screenOffset,
				worldPosition,
				relativePosition,
				sizeOffset,
				resolution,
				zoom,
				rotation,
				isScrolling,
				velocity,
				windowIndex,
				worldSpace);

		PushCamera(newCamera);
	}
	void ResizeCameras(const UVector2 &size, unsigned index)
	{
		for (auto camera : cameras)
		{
			if (camera->GetWindowIndex() == index)
			{
				Vector2 scale(
					static_cast<float>(windows[camera->GetWindowIndex()]->GetDimensions().x)
					/ static_cast<float>(camera->GetDimensions().x) * 100,

					static_cast<float>(windows[camera->GetWindowIndex()]->GetDimensions().y)
					/ static_cast<float>(camera->GetDimensions().y) * 100);

				Vector2 sizeOffset(
					scale.x * static_cast<float>(size.x) / 100,
					scale.y * static_cast<float>(size.y) / 100);

				camera->Resize(sizeOffset);
			}
		}
	}

	void Draw()
	{
		UpdateCameras();
	}

	void UpdateCameras()
	{
		for (auto camera : cameras)
		{
			camera->Update();

			SDL_GL_MakeCurrent(windows[camera->GetWindowIndex()]->GetWindow(), glContext);

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
				if (surface->isActive && camera->GetWorldSpace() == surface->worldSpace)
					DrawSurface(*surface, *camera);
		}
	}

	void DrawSurface(const GLSurface& glSurface, const Camera& camera)
	{
		glPushMatrix(); // Save the current matrix.

		glTranslatef(
			glSurface.position.x + (glSurface.center.x - glSurface.offsetSize.x / 2),
			glSurface.position.y + (glSurface.center.y - glSurface.offsetSize.y / 2),
			0.0f);
		glScalef(glSurface.scale.x, glSurface.scale.y, 0.0f);
		glRotatef(glSurface.rotation, 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-glSurface.position.x - (glSurface.center.x - glSurface.offsetSize.x / 2),
			-glSurface.position.y - (glSurface.center.y - glSurface.offsetSize.y / 2), 0.0f);

		glTranslatef(camera.GetResolution().x / 2, camera.GetResolution().y / 2, 0.0f);
		glScalef(camera.GetZoom().x, camera.GetZoom().y, 0.0f);
		glRotatef(camera.GetRotation(), 0.0f, 0.0f, 1.0f);
		glTranslatef(
			-camera.GetWorldPos().x,
			-camera.GetWorldPos().y, 0.0f);

		GLfloat glVertices[] ={
			glSurface.offsetPosition.x / glSurface.size.x,
			glSurface.offsetPosition.y / glSurface.size.y,

			glSurface.offsetPosition.x / glSurface.size.x + glSurface.offsetSize.x / glSurface.size.x,
			glSurface.offsetPosition.y / glSurface.size.y,

			glSurface.offsetPosition.x / glSurface.size.x + glSurface.offsetSize.x / glSurface.size.x,
			glSurface.offsetPosition.y / glSurface.size.y + glSurface.offsetSize.y / glSurface.size.y,

			glSurface.offsetPosition.x / glSurface.size.x,
			glSurface.offsetPosition.y / glSurface.size.y + glSurface.offsetSize.y / glSurface.size.y
		};

		GLfloat glPosition[] ={
			glSurface.position.x - glSurface.offsetSize.x / 2,
			glSurface.position.y - glSurface.offsetSize.y / 2,

			glSurface.position.x + glSurface.offsetSize.x / 2,
			glSurface.position.y - glSurface.offsetSize.y / 2,

			glSurface.position.x + glSurface.offsetSize.x / 2,
			glSurface.position.y + glSurface.offsetSize.y / 2,

			glSurface.position.x - glSurface.offsetSize.x / 2,
			glSurface.position.y + glSurface.offsetSize.y / 2
		};

		glBindTexture(GL_TEXTURE_2D, glSurface.handle);
		glColor4ub(glSurface.colour.r, glSurface.colour.g, glSurface.colour.b, glSurface.colour.a);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, glVertices);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glPosition);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}

	bool SortLayer(GLSurface* left, GLSurface* right)
	{
		if (left->worldSpace == right->worldSpace)
			return left->layer < right->layer;

		return false;
	}
	bool SortCamera(GLSurface* left, GLSurface* right)
	{
		return left->worldSpace < right->worldSpace;
	}

	GLSurface* LoadSurface(const char* filename)
	{
		auto sdlSurface = IMG_Load(filename);
		if (!sdlSurface)
		{
			printf("SDL_Error: %s\n", SDL_GetError());

			return nullptr;
		}

		return LoadSurface(*sdlSurface);
	}
	GLSurface* LoadSurface(SDL_Surface& sdlSurface)
	{
		auto glSurface = new GLSurface;

		int mode;
		// work out what format to tell glTexImage2D to use...
		if (sdlSurface.format->BytesPerPixel == 3) // RGB 24bit
			mode = GL_RGB;
		else if (sdlSurface.format->BytesPerPixel == 4) // RGBA 32bit
			mode = GL_RGBA;
		else
		{
			SDL_FreeSurface(&sdlSurface);
			return nullptr;
		}

		glGenTextures(1, &glSurface->handle);
		glBindTexture(GL_TEXTURE_2D, glSurface->handle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(
			GL_TEXTURE_2D, 0, mode, sdlSurface.w, sdlSurface.h, 0, mode, GL_UNSIGNED_BYTE, sdlSurface.pixels);

		glSurface->position ={ NULL, NULL };
		glSurface->offsetPosition ={ NULL, NULL };

		glSurface->size.x = sdlSurface.w;
		glSurface->size.y = sdlSurface.h;

		glSurface->center.x = glSurface->size.x / 2.0f;
		glSurface->center.y = glSurface->size.y / 2.0f;

		glSurface->offsetSize.x = glSurface->size.x;
		glSurface->offsetSize.y = glSurface->size.y;

		glSurface->rotation = NULL;
		glSurface->scale ={ 1, 1 };

		glSurface->colour ={ 255, 255, 255, 255 };

		glSurface->layer = LayerType::BACKGROUND;

		glSurface->worldSpace = 0;

		glSurface->isActive = true;

		SDL_FreeSurface(&sdlSurface);

		PushSurface(glSurface);

		std::sort(glSurfaces.begin(), glSurfaces.end(), SortCamera);
		std::sort(glSurfaces.begin(), glSurfaces.end(), SortLayer);

		return glSurface;
	}

	void PushSurface(GLSurface* glSurface)
	{
		glSurfaces.push_back(glSurface);
	}

	void PushCamera(Camera* camera)
	{
		cameras.push_back(camera);
	}

	void DrawRect(const Vector2 &start, const Vector2 &end, const Vector4 &colour)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		auto size = Vector2(end.x - start.x, end.y - start.y);

		glColor4ub(colour.r, colour.g, colour.b, colour.a);
		glBegin(GL_QUADS);
		{
			//Bottom-left vertex (corner)
			glVertex3f(start.x, start.y, 0.0f); //Vertex Coords

			//Bottom-right vertex (corner)
			glVertex3f(start.x + size.x, start.y, 0.f);

			//Top-right vertex (corner)
			glVertex3f(start.x + size.x, start.y + size.y, 0.f);

			//Top-left vertex (corner)
			glVertex3f(start.x, start.y + size.y, 0.f);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawLine(const Vector2 &head, const Vector2 &tail, const Vector4 &colour)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(colour.r, colour.g, colour.b, colour.a);

		glBegin(GL_LINES);
		{
			glVertex2f(head.x, head.y);
			glVertex2f(tail.x, tail.y);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawPoint(const Vector2& position, const Vector4 &colour)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(colour.r, colour.g, colour.b, colour.a);

		glBegin(GL_POINTS);
		glVertex2f(position.x, position.y);
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawRing(const Vector2 &center, float radius, float points, const Vector4 &colour)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(colour.r, colour.g, colour.b, colour.a);

		glBegin(GL_LINE_LOOP);
		{
			for (int i = 0; i <= points; ++i)
			{
				glVertex2f(
					center.x + cosf(i * (PI * 2) / points)*radius,
					center.y + sinf(i * (PI * 2) / points)*radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	void DrawCircle(const Vector2 &center, float radius, float points, const Vector4 &colour)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(colour.r, colour.g, colour.b, colour.a);

		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex2f(center.x, center.y); // center of circle
			for (int i = 0; i <= points; ++i)
			{
				glVertex2f(
					center.x + cosf(i * (PI * 2) / points)*radius,
					center.y + sinf(i * (PI * 2) / points)*radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}

	void Flip()
	{
		for (auto window : windows)
		{
			SDL_GL_MakeCurrent(window->GetWindow(), glContext);
			window->Flip();
		}
	}

	void Quit()
	{
		windows.clear();
		cameras.clear();
		glSurfaces.clear();
	}
}
