# Game
Kevin Wang

2023

## Build Details
Built on Visual Studio 2022

APIs: Win32, Direct2D

Additional dependencies: d2d1.lib, DWrite.lib, windowscodecs.lib

SubSystem: Windows (/SUBSYSTEM:WINDOWS)

## Additional Specifications

Resolution: 600 by 800 pixels

The artwork is done on a 300 by 400 canvas and scaled up using the Nearest Neighbor algorithm.

Software used for artwork: Aseprite

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

### Bounds Entry

The overworld is segmented into discrete bounds that are linked to each other.
This is to improve performance by reducing the number of objects loaded per tick (I am unsure of Direct2D's performance in relation to Win32).
This is also to make the world more easily digestible and less overwhelming to the player.

When switching areas, the player sprite's location is changed, a new area is loaded, and the camera is adjusted. This causes a visual hiccup in the graphics.
To address this, I added a blackout transition sequence to area changes. It has the added bonus of making the switch easier on the eyes.

Each bounds object as an integer ID. In the class constructor, the object uses its ID to register itself to a static ```std::unordered_map<int,Bounds*>```.
This makes it easier to load any area.

Here's a demo of all the things discussed so far.

https://user-images.githubusercontent.com/103074297/226948753-8cb0e03b-aea3-413f-a948-eb8d270930d6.mp4

### Dialogue

Dialogue is triggered by pressing [E] while in contact with a Dialogue object (drawn in pink for now).
[E] loads the next line or exits the dialogue if there are no more lines. [Q] exits the dialogue.

Because the render target itself is transformed to center on the player, drawing the dialogue box using float literals would have it render on the wrong part of the screen.
One option is to use the player's position and the "following area" to dynamically determine the appropriate coordinates. I did not do this.

My solution is to buffer the transform applied to the render target, transform the render target with the identity matrix, draw the dialogue as normal, and finally restore the original transformation. Here is some example code showing how this can be done:

```C++
// N.B.: this is not what appears in the source code

ID2D1_MATRIX_3x2_F * transform_buffer; //Direct2D uses matrices for transforms

rtarget->getTransform(&transform_buffer) //load the current transform into transform_buffer
rtarget->setTransform(D2D1::IdentityMatrix()); //use the identity transform

/*
* the render target is now in its "original" position
* the dialogue can be drawn using float literals
* with the expected outcome, regardless of player position
*/

rtarget->setTransform(transform_buffer) //restore the appropriate transform for the camera position
```

All of this happens within one cycle, i.e. between: ```rtarget->beginDraw()``` and ```rtarget->endDraw()```, so this process is invisible.

Demo:

https://user-images.githubusercontent.com/103074297/226950849-7caf5b58-ee91-4890-940f-25f705d49724.mp4

### Putting it Together

For my own convenience more than anything else, each space (I called them "bounds" for some reason) is loaded from a text file at the beginning of the program.
The files are of this form:

```
200.0
600.0
150.0
450.0

2
1 300.0 400.0 620.0 280.0 670.0 330.0 0
2 200.0 280.0 50.0 0.0 170.0 30.0 1

1
dialogue00.txt 150.0 460.0 180.0 490.0

50.0 0.0
50.0 550.0
550.0 550.0
550.0 400.0
720.0 400.0
720.0 70.0
170.0 70.0
170.0 0.0
```

The first four lines determine the "following area" of the camera.

The next chunk determines entry points to other areas. It is formatted as such:
* the ID of the area to go to (one value)
	* For some reason, the ID of *this* bounds object isn't passed in the text file. If you're still reading this, know that I plan to fix this.
	* There is no automated way to create two-way links between areas; it must be done individually for each. While this gives the programmer (me) much more flexibility, it is also error-prone.
* the position that the player sprite will be taken to (two values)
* the collision area of the entry point in the current area (four values)
* whether or not the entry point triggers on contact or also requires pressing [E] (one value)

The next chunk describes the dialogue:
* the text file from which to load the dialogue
* the collision area of the dialogue object

The final chunk describes the borders of the area.

There will eventually be more things.

### Menu

The menus are drawn using the same principle as with the dialogue with respect to preserving the camera position.

Other than that, development is in-progress. Because I did not plan out the entire game before I started coding, I don't actually know *exactly* what will be required of my menu structure.

Therefore, I plan on making the menu system more robust than I think it will need to be. I would rather have an overbuilt menu system rather than realize that what I have isn't enough 75% of the way through the development process when I get a new idea.

You can see some in-progress stuff now. It is a mess.

