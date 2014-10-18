#define WINVER 0x0500
#include <windows.h>
#include "vk.h"
#include <string>
#include <iostream>

using namespace std;

void setUp(INPUT *ip)
{
    // Set up a generic keyboard event.
    ip->type = INPUT_KEYBOARD;
    ip->ki.wScan = 0; // hardware scan code for key
    ip->ki.time = 0;
    ip->ki.dwExtraInfo = 0;
}
bool pressKey(int keyCode, string message=NULL) // press and release
{
    INPUT ip;
    setUp(&ip);

    // press the key
    ip.ki.wVk = keyCode;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));

    cout << message << endl;
    Sleep(300);

    return true;
}

bool press2Key(int keyCode1, int keyCode2, string message=NULL)
{
    INPUT ip1;
    INPUT ip2;

    setUp(&ip1);
    setUp(&ip2);

    // press the key
    ip1.ki.wVk = keyCode1;
    ip1.ki.dwFlags = 0;
    SendInput(1, &ip1, sizeof(INPUT));

    // press the key
    ip2.ki.wVk = keyCode2;
    ip2.ki.dwFlags = 0;
    SendInput(1, &ip2, sizeof(INPUT));

    // Release the key
    ip1.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip1, sizeof(INPUT));

    // Release the key
    ip2.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip2, sizeof(INPUT));

    cout << message << endl;
    Sleep(300);

    return true;
}

bool mouseClick(bool left, bool release, string message=NULL)
{
    INPUT ip;
    setUp(&ip);

    // press the key
    if (left)
    {
        ip.ki.wVk = VK_LBUTTON;
    } else {
        ip.ki.wVk = VK_RBUTTON;
    }
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));

    // release
    if (release)
    {
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        return true;
    } else {
        return false;
    }
    cout << message << endl;
    Sleep(300);
}

bool mouseRelease(INPUT *ip, string message=NULL)
{
    // null pointer, no such input
    if (!ip)
    {
        return false;
    } else {
        ip->ki.dwFlags = KEYEVENTF_KEYUP;
        return true;
    }
    cout << message << endl;
    Sleep(300);
}

// place cursor to location x, y
bool placeCursor(int x, int y)
{
    SetCursorPos(pt.x, pt.y);
    cout << "Cursor placed: (" << x << ", " << y << ")" << endl;
    Sleep(200);
    return 1;
}
