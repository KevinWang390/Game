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
* param: message -- the message itself, i.e. a keystroke, lifting a key, clicking the exit button, etc.
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
