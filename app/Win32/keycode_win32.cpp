#include "keycode_win32.h"
#include <windows.h>

KeyCode ConvertKeyCode_Win32(int virtualKey)
{
    switch (virtualKey)
    {
        case 'A': return KEYCODE_A;
        case 'B': return KEYCODE_B;
        case 'C': return KEYCODE_C;
        case 'D': return KEYCODE_D;
        case 'E': return KEYCODE_E;
        case 'F': return KEYCODE_F;
        case 'G': return KEYCODE_G;
        case 'H': return KEYCODE_H;
        case 'I': return KEYCODE_I;
        case 'J': return KEYCODE_J;
        case 'K': return KEYCODE_K;
        case 'L': return KEYCODE_L;
        case 'M': return KEYCODE_M;
        case 'N': return KEYCODE_N;
        case 'O': return KEYCODE_O;
        case 'P': return KEYCODE_P;
        case 'Q': return KEYCODE_Q;
        case 'R': return KEYCODE_R;
        case 'S': return KEYCODE_S;
        case 'T': return KEYCODE_T;
        case 'U': return KEYCODE_U;
        case 'V': return KEYCODE_V;
        case 'W': return KEYCODE_W;
        case 'X': return KEYCODE_X;
        case 'Y': return KEYCODE_Y;
        case 'Z': return KEYCODE_Z;

        case '0': return KEYCODE_0;
        case '1': return KEYCODE_1;
        case '2': return KEYCODE_2;
        case '3': return KEYCODE_3;
        case '4': return KEYCODE_4;
        case '5': return KEYCODE_5;
        case '6': return KEYCODE_6;
        case '7': return KEYCODE_7;
        case '8': return KEYCODE_8;
        case '9': return KEYCODE_9;

        case VK_RETURN: return KEYCODE_RETURN;
        case VK_ESCAPE: return KEYCODE_ESCAPE;
        case VK_BACK: return KEYCODE_BACKSPACE;
        case VK_TAB: return KEYCODE_TAB;
        case VK_SPACE: return KEYCODE_SPACE;

        case VK_OEM_MINUS: return KEYCODE_MINUS;
        case VK_OEM_PLUS: return KEYCODE_EQUALS;
        case VK_OEM_4: return KEYCODE_LEFTBRACKET;
        case VK_OEM_6: return KEYCODE_RIGHTBRACKET;
        case VK_OEM_5: return KEYCODE_BACKSLASH;
        case VK_OEM_1: return KEYCODE_SEMICOLON;
        case VK_OEM_7: return KEYCODE_APOSTROPHE;
        case VK_OEM_3: return KEYCODE_GRAVE;
        case VK_OEM_COMMA: return KEYCODE_COMMA;
        case VK_OEM_PERIOD: return KEYCODE_PERIOD;
        case VK_OEM_2: return KEYCODE_SLASH;

        case VK_CAPITAL: return KEYCODE_CAPSLOCK;

        case VK_F1: return KEYCODE_F1;
        case VK_F2: return KEYCODE_F2;
        case VK_F3: return KEYCODE_F3;
        case VK_F4: return KEYCODE_F4;
        case VK_F5: return KEYCODE_F5;
        case VK_F6: return KEYCODE_F6;
        case VK_F7: return KEYCODE_F7;
        case VK_F8: return KEYCODE_F8;
        case VK_F9: return KEYCODE_F9;
        case VK_F10: return KEYCODE_F10;
        case VK_F11: return KEYCODE_F11;
        case VK_F12: return KEYCODE_F12;

        case VK_SNAPSHOT: return KEYCODE_PRINTSCREEN;
        case VK_SCROLL: return KEYCODE_SCROLLLOCK;
        case VK_PAUSE: return KEYCODE_PAUSE;
        case VK_INSERT: return KEYCODE_INSERT;
        case VK_HOME: return KEYCODE_HOME;
        case VK_PRIOR: return KEYCODE_PAGEUP;
        case VK_DELETE: return KEYCODE_DELETE;
        case VK_END: return KEYCODE_END;
        case VK_NEXT: return KEYCODE_PAGEDOWN;
        case VK_RIGHT: return KEYCODE_RIGHT;
        case VK_LEFT: return KEYCODE_LEFT;
        case VK_DOWN: return KEYCODE_DOWN;
        case VK_UP: return KEYCODE_UP;

        case VK_NUMLOCK: return KEYCODE_NUMLOCKCLEAR;
        case VK_DIVIDE: return KEYCODE_KP_DIVIDE;
        case VK_MULTIPLY: return KEYCODE_KP_MULTIPLY;
        case VK_SUBTRACT: return KEYCODE_KP_MINUS;
        case VK_ADD: return KEYCODE_KP_PLUS;
        case VK_NUMPAD1: return KEYCODE_KP_1;
        case VK_NUMPAD2: return KEYCODE_KP_2;
        case VK_NUMPAD3: return KEYCODE_KP_3;
        case VK_NUMPAD4: return KEYCODE_KP_4;
        case VK_NUMPAD5: return KEYCODE_KP_5;
        case VK_NUMPAD6: return KEYCODE_KP_6;
        case VK_NUMPAD7: return KEYCODE_KP_7;
        case VK_NUMPAD8: return KEYCODE_KP_8;
        case VK_NUMPAD9: return KEYCODE_KP_9;
        case VK_NUMPAD0: return KEYCODE_KP_0;
        case VK_DECIMAL: return KEYCODE_KP_PERIOD;

        case VK_LCONTROL: return KEYCODE_LCTRL;
        case VK_LSHIFT: return KEYCODE_LSHIFT;
        case VK_LMENU: return KEYCODE_LALT;
        case VK_LWIN: return KEYCODE_LGUI;
        case VK_RCONTROL: return KEYCODE_RCTRL;
        case VK_RSHIFT: return KEYCODE_RSHIFT;
        case VK_RMENU: return KEYCODE_RALT;
        case VK_RWIN: return KEYCODE_RGUI;

        default:
            return KEYCODE_UNKNOWN;
    }
}