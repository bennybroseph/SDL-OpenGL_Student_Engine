#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "Window.h"
#include "Camera.h"

#include <algorithm> // Holds the 'sort()' function

#include <SDL_image.h> // Required for loading images from a file

namespace Graphics
{
	enum LayerType
	{
		BACKGROUND, // Lowest Layer
		INLINEFORE,	// Layer is behind the Mid-ground, but acts as if it is in line with the Foreground (currently not working)
		MIDGROUND,
		FOREGROUND,

		FOUNDATION,
		STRUCTURE,
		OVERLAY,

		ALWAYS_TOP // Highest Layer
	};

	template <typename T>
	struct GLSurface
	{
		GLuint Surface;

		System::Point2D<T> Pos;
		System::Point2D<T> OffsetP;
		System::Point2D<T> Center;

		System::Size2D<T> Dimensions;
		System::Size2D<T> OffsetD;
		System::Size2D<T> Scale;
		T Rotation;

		System::Color<T> Color;

		LayerType Layer;

		unsigned int uiWorldSpace;

		bool bIsActive;
	};

	struct SurfaceUnion
	{
		enum { INT, FLOAT }Tag;
		union
		{
			GLSurface<int>*	  iGLSurface;
			GLSurface<float>* fGLSurface;
		};
	};
	extern std::vector<SurfaceUnion*> vglSurfaces;

	struct CameraUnion // Allows for int || float 'Camera' objects to be stored in the same vector
	{
		enum { INT, FLOAT }Tag; // Since it is an unnamed struct, 'Tag' becomes a member variable of type enum
		union
		{
			Camera<int>*   iCamera; // The int type of 'Camera'
			Camera<float>* fCamera; // The float type of 'Camera'
		};
	};
	extern std::vector<CameraUnion*>	voCameras; // The vector that holds each type of 'Camera' object
	extern std::vector<Window*>			voWindows; // The vector that holds each 'Window' object

	// - Sets up the Graphics namespace to be used. Must be called before using any free functions
	bool Init(); 

	/* - Creates a 'new Window' and pushes it into the 'voWindows' vector
	   Parameters:
	   - The window's internal resolution
	   - Whether or not the window should be full screen on creation
	   - The window's width and height
	   - The window's title -- Default = "New Window"
	   - Which monitor the window should be created on -- Default = 0
	*/
	void NewWindow(
		const System::Size2D<unsigned int>& ac_iResolution,				// The window's internal resolution
		const bool						    ac_bFullscreen,				// Whether or not the window should be full screen on creation
		const System::Size2D<unsigned int>& ac_iDimensions,				// The window's width and height
		const char*							ac_szTitle = "New Window",	// The window's title
		const unsigned int					ac_uiMonitorIndex = 0);		// Which monitor the window should be created on

	// - Allows a window in the 'voWindows' vector to change Size
	void Resize(
		const System::Size2D<unsigned int>& ac_uiNewDimensions,		// The new Size of the Window
		const unsigned int					ac_uiNewMonitorIndex,	// The monitor the Window should be placed on
		const unsigned int					ac_uiIndex);			// Which window to change

	// - Toggles the window from it's current state of full screen
	void ToggleFullscreen(const unsigned int ac_uiIndex); 

	// - Creates a 'new Camera' object and passes it the following parameters
	template <typename T = float>
	void NewCamera(
		const System::Point2D<T>&	   ac_ScreenPos = { 0, 0 },			// Where the 'Camera' object should be placed relative to the screen
		const System::Point2D<T>&	   ac_WorldPos = { 0, 0 },			// Where in the World the 'Camera' object should start in the world. Serves as an offset to 'm_RelativePos'
		const System::Point2D<T>&	   ac_RelativePos = { 0, 0 },		// What point if any the 'Camera' should be anchored to, such as a player object's 'Point2D'
		const System::Size2D<T>&	   ac_Dimensions = { 100, 100 },	// The size of the 'Camera' object relative to the window size
		const System::Size2D<T>&	   ac_Zoom = { 1, 1 },				// The zoom in of the 'Camera' object
		const T						   ac_Rotation = 0,					// The rotation of the 'Camera' object
		const bool					   ac_bIsScrolling = false,			// Whether or not the 'Camera' object scrolls
		const System::AngularVel<T>&   ac_Velocity = { 0, 0 },			// If the 'Camera' object scrolls, it needs a velocity as well
		const unsigned int			   ac_uiWindowIndex = 0,			// Which window the 'Camera' object should affect
		const unsigned int			   ac_uiWorldSpace = 0);			// Which world space the 'Camera' object exists in. This can be any positive number

	// - Makes sure when the window is resized that all 'Camera' objects resize to match it
	template <typename T>
	void ResizeCameras(Camera<T>& a_Camera, const System::Size2D<unsigned int>& ac_uiDimensions, const unsigned int ac_uiIndex);

	// - Draws all surfaces currently in the 'vglSurfaces' vector
	void Draw();

	// - Updates the view-port to match the current 'Camera' object used to draw and then draws all surfaces in its world space
	template <typename T>
	void UpdateCameras(Camera<T>& a_Camera);

	// - Draws a 'GLSurface' in relation to a 'Camera' object
	template <typename T, typename U>
	void DrawSurface(const GLSurface<T>& ac_glSurface, Camera<U>& a_Camera);

	// - Sorts each surface based on its layer order
	bool SortLayer(SurfaceUnion* ac_pglLeft, SurfaceUnion* ac_pglRight);
	// - Sorts each surface based on its camera order
	bool SortCamera(SurfaceUnion* ac_pglLeft, SurfaceUnion* ac_pglRight);

	// - Loads a 'GLSurface' from a filename
	template <typename T = float>
	GLSurface<T>* LoadSurface(const char* ac_szFilename);
	// - Loads a 'GLSurface' from an existing 'SDL_Surface'
	template <typename T = float>
	GLSurface<T>* LoadSurface(SDL_Surface& a_sdlSurface);

	// - Pushes a 'GLSurface' of type int into the 'vglSurfaces' vector
	void PushSurface(GLSurface<int>* a_glSurface);
	// - Pushes a 'GLSurface' of type float into the 'vglSurfaces' vector
	void PushSurface(GLSurface<float>* a_glSurface);

	// - Pushes a 'Camera' of type int into the 'voCamera' vector
	void PushCamera(Camera<int>* a_Camera);
	// - Pushes a 'Camera' of type int into the 'voCamera' vector
	void PushCamera(Camera<float>* a_Camera);

	// - Draws a colored rectangle at the specified position with a given width and height
	template <typename T = float>
	void DrawRect(const System::Point2D<T>& ac_Pos, const System::Size2D<T>& ac_Size, const System::Color<T>& ac_Color);
	// - Draws a colored line at with a given beginning and end
	template <typename T = float>
	void DrawLine(const System::Point2D<T>& ac_Begin, const System::Point2D<T>& ac_End, const System::Color<T>& ac_Color);
	// - Draws a colored pixel with a given beginning and end
	template <typename T = float>
	void DrawPoint(const System::Point2D<T>& ac_Pos, const System::Color<T>& ac_Color);
	// - Draws a colored ring with a given center, radius, and quality modifier
	template <typename T = float>
	void DrawRing(const System::Point2D<T> ac_Center, const T ac_Radius, const T ac_Quality, const System::Color<T>& ac_Color);
	// - Draws a colored circle with a given center, radius, and quality modifier
	template <typename T>
	void DrawCircle(const System::Point2D<T> ac_Center, const T ac_Radius, const T ac_Quality, const System::Color<T>& ac_Color);

	void Flip(); // Clears the buffer of all windows to allow all the new information to be displayed

	void Quit();
}

// All templated functions that need to be called outside of the namespace go here
namespace Graphics
{
	template <typename T>
	void NewCamera(
		const System::Point2D<T>&	   ac_ScreenPos,
		const System::Point2D<T>&	   ac_WorldPos,
		const System::Point2D<T>&	   ac_RelativePos,
		const System::Size2D<T>&	   ac_Dimensions,
		const System::Size2D<T>&	   ac_Zoom,
		const T						   ac_Rotation,
		const bool					   ac_bIsScrolling,
		const System::AngularVel<T>&   ac_Velocity,
		const unsigned int			   ac_uiWindowIndex,
		const unsigned int			   ac_uiWorldSpace)
	{
		System::Size2D<T> SizeOffset = { ac_Dimensions.W * (T)voWindows[ac_uiWindowIndex]->GetDimensions().W / 100, ac_Dimensions.H * (T)voWindows[ac_uiWindowIndex]->GetDimensions().H / 100 };
		System::Point2D<T> ScreenOffset = {
			ac_ScreenPos.X * (T(voWindows[ac_uiWindowIndex]->GetDimensions().W) - SizeOffset.W) / 100,
			abs(ac_ScreenPos.Y - 100) * (T(voWindows[ac_uiWindowIndex]->GetDimensions().H) - SizeOffset.H) / 100 };

		System::Size2D<T> Resolution = {
			(T)voWindows[ac_uiWindowIndex]->GetResolution().W * ((float)SizeOffset.W / (float)voWindows[ac_uiWindowIndex]->GetDimensions().W),
			(T)voWindows[ac_uiWindowIndex]->GetResolution().H * ((float)SizeOffset.H / (float)voWindows[ac_uiWindowIndex]->GetDimensions().H) };

		Camera<T>* newCamera = new Camera<T>(ScreenOffset, ac_WorldPos, ac_RelativePos, SizeOffset, Resolution, ac_Zoom, ac_Rotation, ac_bIsScrolling, ac_Velocity, ac_uiWindowIndex, ac_uiWorldSpace);
		PushCamera(newCamera);
	}

	template <typename T>
	GLSurface<T>* LoadSurface(const char* ac_szFilename)
	{
		SDL_Surface* sdlSurface;

		sdlSurface = IMG_Load(ac_szFilename);
		if (sdlSurface == NULL)
		{
			printf("SDL_Error: %s\n", SDL_GetError());

			GLSurface<T> *glSurface = nullptr;
			return glSurface;
		}

		return LoadSurface<T>(*sdlSurface);
	}
	template <typename T>
	GLSurface<T>* LoadSurface(SDL_Surface& a_sdlSurface)
	{
		GLSurface<T>* glSurface = new GLSurface<T>;

		glGenTextures(1, &glSurface->Surface);
		glBindTexture(GL_TEXTURE_2D, glSurface->Surface);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_sdlSurface.w, a_sdlSurface.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_sdlSurface.pixels);

		glSurface->Pos = { NULL, NULL };
		glSurface->OffsetP = { NULL, NULL };

		glSurface->Dimensions.W = a_sdlSurface.w;
		glSurface->Dimensions.H = a_sdlSurface.h;

		glSurface->Center.X = glSurface->Dimensions.W / 2.0f;
		glSurface->Center.Y = glSurface->Dimensions.H / 2.0f;

		glSurface->OffsetD.W = glSurface->Dimensions.W;
		glSurface->OffsetD.H = glSurface->Dimensions.H;

		glSurface->Rotation = NULL;
		glSurface->Scale = { 1, 1 };

		glSurface->Color = { 255, 255, 255, 255 };

		glSurface->Layer = LayerType::BACKGROUND;

		glSurface->uiWorldSpace = 0;

		glSurface->bIsActive = true;

		SDL_FreeSurface(&a_sdlSurface);

		PushSurface(glSurface);
		std::sort(vglSurfaces.begin(), vglSurfaces.end(), SortCamera);
		std::sort(vglSurfaces.begin(), vglSurfaces.end(), SortLayer);

		return glSurface;
	}

	template <typename T>
	void DrawRect(const System::Point2D<T>& ac_Pos, const System::Size2D<T>& ac_Size, const System::Color<T>& ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.Red, ac_Color.Green, ac_Color.Blue, ac_Color.Alpha);
		glBegin(GL_QUADS);
		{
			//Bottom-left vertex (corner)
			glVertex3f(ac_Pos.X, ac_Pos.Y, 0.0f); //Vertex Coords

												  //Bottom-right vertex (corner)
			glVertex3f(ac_Pos.X + ac_Size.W, ac_Pos.Y, 0.f);

			//Top-right vertex (corner)
			glVertex3f(ac_Pos.X + ac_Size.W, ac_Pos.Y + ac_Size.H, 0.f);

			//Top-left vertex (corner)
			glVertex3f(ac_Pos.X, ac_Pos.Y + ac_Size.H, 0.f);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	template <typename T>
	void DrawLine(const System::Point2D<T>& ac_Begin, const System::Point2D<T>& ac_End, const System::Color<T>& ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.Red, ac_Color.Green, ac_Color.Blue, ac_Color.Alpha);

		glBegin(GL_LINES);
		{
			glVertex2f(ac_Begin.X, ac_Begin.Y);
			glVertex2f(ac_End.X, ac_End.Y);
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	template <typename T>
	void DrawPoint(const System::Point2D<T>& ac_Pos, const System::Color<T>& ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.Red, ac_Color.Green, ac_Color.Blue, ac_Color.Alpha);

		glBegin(GL_POINTS);
		glVertex2f(ac_Pos.X, ac_Pos.Y);
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	template <typename T>
	void DrawRing(const System::Point2D<T> ac_Center, const T ac_Radius, const T ac_Quality, const System::Color<T>& ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.Red, ac_Color.Green, ac_Color.Blue, ac_Color.Alpha);

		glBegin(GL_LINE_LOOP);
		{
			for (int i = 0; i <= ac_Quality; ++i)
			{
				glVertex2f(
					ac_Center.X + cosf(i * (PI * 2) / ac_Quality)*ac_Radius,
					ac_Center.Y + sinf(i * (PI * 2) / ac_Quality)*ac_Radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
	template <typename T>
	void DrawCircle(const System::Point2D<T> ac_Center, const T ac_Radius, const T ac_Quality, const System::Color<T>& ac_Color)
	{
		glPushMatrix(); // Save the current matrix.

		glBindTexture(GL_TEXTURE_2D, NULL);

		glColor4ub(ac_Color.Red, ac_Color.Green, ac_Color.Blue, ac_Color.Alpha);

		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex2f(ac_Center.X, ac_Center.Y); // center of circle
			for (int i = 0; i <= ac_Quality; ++i)
			{
				glVertex2f(
					ac_Center.X + cosf(i * (PI * 2) / ac_Quality)*ac_Radius,
					ac_Center.Y + sinf(i * (PI * 2) / ac_Quality)*ac_Radius);
			}
		}
		glEnd();

		glPopMatrix(); // Reset the current matrix to the one that was saved.
	}
}
#endif // _GRAPHICS_H_
