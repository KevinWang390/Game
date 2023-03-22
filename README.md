# Game
Kevin Wang

2023

## Build Details
Built on Visual Studio 2022

APIs: Win32, Direct2D

Additional dependencies: d2d1.lib, DWrite.lib, windowscodecs.lib

SubSystem: Windows (/SUBSYSTEM:WINDOWS)

## About

This is an in-progress, 2D, top-down game built for Windows systems.
As of now, this project is more of a programming exercise and is not intended for distribution to consumers. This may change.
Direct2D (part of DirectX) is a graphics rendering API; it does not provide explicitly game-related functionality.
My implementation is unlikely to represent good programming or design practices.

## Notes on Implementation

### Input Handling

There is a delay between the input read from the initial keystroke and subsequent repeated inputs from holding down the key.
While this is appropriate for typing text, it is not appropriate for things like player sprite movement.
For this reason, this program ignores all messages that result from holding down a key.
The input handling happens in WindowProc(), the message handler for the game window:

```C++
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if ((lParam >> 30) & 0x1) break; //if key is held, ignore
		ic->receive_keydown(wParam); //receive input
		break;
	case WM_KEYUP:
		ic->receive_keyup(wParam);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
```
* param: hWnd -- the window that receives the message
* param: message -- the message itself, e.g. a keystroke, lifting a key, clicking the exit button, etc.
* param: wParam -- in the case of a keystroke, the Windows Virtual Key Code of that key
* param: lParam -- miscellaneous information

Both WPARAM and LPARAM are integers. The 30th bit of lParam indicates if the message was sent by a keystroke or a key being held.
In the latter case, the message is ignored.

Win32 recognizes both pressing on and lifting up from a key, so each key corresponds to two different messages.
As such, each input-responsive class implements these functions:

```C++
void input_start(WPARAM wParam); //triggered upon WM_KEYDOWN
void input_end(WPARAM wParam); //triggered upon WM_KEYUP
```
This allows the program to be implemented independently of system or hardware constraints and specificities.

### Player Movement

Player sprite movement is 8-directional and controlled by WASD.
Because keys are never held, WASD keystrokes change the velocity of the player rather than the position.
The Velocity class has separate x and y components defaulting to 0; if both components are nonzero, the speed is normalized.

For example, WM_KEYDOWN on [D] increments the x component from 0 to 1. WM_KEYUP on [D] decrements the x component from 1 to 0.
The opposite goes for [A], and the same setup goes for [W] and [S]. This works well when pressing multiple keys.

### Camera

The camera is centered on the player sprite while the player sprite remains within the bounds of a "following area", which is just an invisible rectangle.
The dimensions of the rectangle are specific to each space.
Smaller spaces may have a rectangle of area 0, meaning that the camera will not move at all.

The render target itself is transformed based on the position of the player.
This leads to challenges when rendering overlays that look the same regardless of the player's location, such as menus, dialogue, etc.
However, transforming a geometry requires creating a new ID2D1TransformedGeometry for each loaded geometry with every frame, which I wanted to avoid. 

### Space entry

The overworld is segmented into discrete chunks that are linked to each other.
This is to improve performance by reducing the number of objects loaded per tick (I am unsure of Direct2D's performance in relation to Win32).
This is also to make the world more easily digestible and less overwhelming to the player.

When switching spaces, the player sprite's location is changed, a new space is loaded, and the camera is adjusted. This causes a visual hiccup in the graphics.
To address this, I added a blackout transition sequence to space changes. It has the added bonus of making the switch easier on the eyes.

Here's a demo of all the things discussed so far.

https://user-images.githubusercontent.com/103074297/226948753-8cb0e03b-aea3-413f-a948-eb8d270930d6.mp4




