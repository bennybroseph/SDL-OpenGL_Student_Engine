#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "Window.h"
#include "Camera.h"

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

	struct GLSurface
	{
		GLuint Surface;

		Vector2 Pos;
		Vector2 OffsetP;
		Vector2 Center;

		Vector2 Dimensions;
		Vector2 OffsetD;
		Vector2 Scale;
		float Rotation;

		Vector4 Color;

		LayerType Layer;

		unsigned int uiWorldSpace;

		bool bIsActive;
	};

	extern std::vector<GLSurface*> glSurfaces;

	extern List<Camera*> voCameras; // The vector that holds each type of 'Camera' object
	extern List<Window*> voWindows; // The vector that holds each 'Window' object

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
	bool NewWindow(
		const UVector2 &	resolution,				// The window's internal resolution
		const bool			isFullscreen,			// Whether or not the window should be full screen on creation
		const UVector2 &	dimensions,				// The window's width and height
		const char *		title = "New Window",	// The window's title
		const unsigned		monitorIndex = 0);		// Which monitor the window should be created on

	// - Allows a window in the 'voWindows' vector to change Size
	void Resize(
		const UVector2 &	ac_uiNewDimensions,		// The new Size of the Window
		const unsigned int	ac_uiNewMonitorIndex,	// The monitor the Window should be placed on
		const unsigned int	ac_uiIndex);			// Which window to change

	// - Toggles the window from it's current state of full screen
	void ToggleFullscreen(unsigned int ac_uiIndex);

	// - Creates a 'new Camera' object and passes it the following parameters
	void NewCamera(
		const Vector2 &	ac_ScreenPos		= { 0, 0 },		// Where the 'Camera' object should be placed relative to the screen
		const Vector2 &	ac_WorldPos			= { 0, 0 },		// Where in the World the 'Camera' object should start in the world. Serves as an offset to 'm_RelativePos'
		const Vector2 &	ac_RelativePos		= { 0, 0 },		// What point if any the 'Camera' should be anchored to, such as a player object's 'Point2D'
		const Vector2 &	ac_Dimensions		= { 100, 100 },	// The size of the 'Camera' object relative to the window size
		const Vector2 &	ac_Zoom				= { 1, 1 },		// The zoom in of the 'Camera' object
		float			ac_Rotation			= 0,			// The rotation of the 'Camera' object
		const bool		ac_bIsScrolling		= false,		// Whether or not the 'Camera' object scrolls
		const Vector2 &	ac_Velocity			= { 0, 0 },		// If the 'Camera' object scrolls, it needs a velocity as well
		unsigned int	ac_uiWindowIndex	= 0,			// Which window the 'Camera' object should affect
		unsigned int	ac_uiWorldSpace		= 0);			// Which world space the 'Camera' object exists in. This can be any positive number

	// - Makes sure when the window is resized that all 'Camera' objects resize to match it
	void ResizeCameras(const UVector2 &ac_uiDimensions, unsigned int ac_uiIndex);

	// - Draws all surfaces currently in the 'vglSurfaces' vector
	void Draw();

	// - Updates the view-port to match the current 'Camera' object used to draw and then draws all surfaces in its world space
	void UpdateCameras();

	// - Draws a 'GLSurface' in relation to a 'Camera' object
	void DrawSurface(const GLSurface &ac_glSurface, Camera &a_Camera);

	// - Sorts each surface based on its layer order
	bool SortLayer(GLSurface* ac_pglLeft, GLSurface* ac_pglRight);
	// - Sorts each surface based on its camera order
	bool SortCamera(GLSurface* ac_pglLeft, GLSurface* ac_pglRight);

	// - Loads a 'GLSurface' from a filename
	GLSurface* LoadSurface(const char* ac_szFilename);
	// - Loads a 'GLSurface' from an existing 'SDL_Surface'
	GLSurface* LoadSurface(SDL_Surface& a_sdlSurface);

	// - Pushes a 'GLSurface' of type int into the 'vglSurfaces' vector
	void PushSurface(GLSurface* a_glSurface);

	// - Pushes a 'Camera' of type int into the 'voCamera' vector
	void PushCamera(Camera* a_Camera);

	// - Draws a colored rectangle at the specified position with a given width and height
	void DrawRect(const Vector2 &ac_Pos, const Vector2 &ac_Size, const Vector4 &ac_Color);
	// - Draws a colored line at with a given beginning and end
	void DrawLine(const Vector2 &ac_Begin, const Vector2 &ac_End, const Vector4 &ac_Color);
	// - Draws a colored pixel with a given beginning and end
	void DrawPoint(const Vector2 &ac_Pos, const Vector4 &ac_Color);
	// - Draws a colored ring with a given center, radius, and quality modifier
	void DrawRing(const Vector2 &ac_Center, float ac_Radius, float ac_Quality, const Vector4 &ac_Color);
	// - Draws a colored circle with a given center, radius, and quality modifier
	void DrawCircle(const Vector2 &ac_Center, float ac_Radius, float ac_Quality, const Vector4 &ac_Color);

	void Flip(); // Clears the buffer of all windows to allow all the new information to be displayed

	void Quit();
}

#endif // _GRAPHICS_H_
