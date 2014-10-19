#define _USE_MATH_DEFINES

#define _WIN32_WINNT 0x0500 //RIGHT
#include <windows.h>

#include <iostream>
#include "myo/myo.hpp"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <vector>
#include "vk.h"

using namespace std;

void setUp(INPUT *ip)
{
    // Set up a generic keyboard event.
    ip->type = INPUT_KEYBOARD;
    ip->ki.wScan = 0; // hardware scan code for key
    ip->ki.time = 0;
    ip->ki.dwExtraInfo = 0;
}
bool pressKey(int keyCode, string message="") // press and release
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

bool press2Key(int keyCode1, int keyCode2, string message="")
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

bool mouseClick(bool left, bool release, string message="")
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

bool mouseRelease(INPUT *ip, string message="")
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
    SetCursorPos(x, y);
    cout << "Cursor placed: (" << x << ", " << y << ")" << endl;
    Sleep(200);
    return 1;
}

// Classes that inherit from myo::DeviceListener
class DataCollector : public myo::DeviceListener {
private:
    int i, j, k;
    std::vector<myo::Pose> tab;
    std::vector<myo::Pose> copy;
    std::vector<myo::Pose> paste;

public:
    DataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
        tab.push_back(myo::Pose::fingersSpread);
        tab.push_back(myo::Pose::waveOut);
        copy.push_back(myo::Pose::fingersSpread);
        copy.push_back(myo::Pose::waveIn);
        paste.push_back(myo::Pose::fist);
        paste.push_back(myo::Pose::fingersSpread);
        i=j=k=0;
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
    }

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
    }

    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
        //print();
        std::cout << std::endl;
        std::cout << i << " " << j << " " << k << std::endl;
        if (pose == tab[i]) {
            //std::cout << "switch";
            i++;
            if (i == 2) {
                myo->vibrate(myo::Myo::vibrationShort);
                std::cout << "switch" << std::endl;
                press2Key(VK_MENU, VK_TAB);
                i = 0;
            }
        } else if (pose != myo::Pose::rest){
            i = 0;
        }
        if (pose == copy[j]) {
            //std::cout << "copy";
            j++;
            if (j == 2) {
                myo->vibrate(myo::Myo::vibrationShort);
                std::cout << "copy" << std::endl;
                press2Key(VK_CONTROL, VK_C);
                j = 0;
            }
        } else if (pose != myo::Pose::rest){
            j = 0;
        }
        if (pose == paste[k]) {
        //std::cout << "paste";
            k++;
            if (k == 2) {
                myo->vibrate(myo::Myo::vibrationShort);
                std::cout << "pasted" << std::endl;
                press2Key(VK_CONTROL, VK_V);
                k = 0;
            }
        } else if (pose != myo::Pose::rest){
            k = 0;
        }
    }

    // onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmRecognized(myo::Myo*
		myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;
    }

    // onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmLost(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.

    // We define this function to print the current values that were updated by the on...() functions above.
    void print()
    {
        // Clear the current line
        std::cout << '\r';

        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
                  << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
                  << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';

        if (onArm) {
            // Print out the currently recognized pose and which arm Myo is being worn on.

            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            std::string poseString = currentPose.toString();

            std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
        }

        std::cout << std::flush;
    }

    // These values are set by onArmRecognized() and onArmLost() above.
    bool onArm;
    myo::Arm whichArm;

    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
    myo::Pose currentPose;
};


int main(int argc, char** argv)
{
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {

    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    myo::Hub hub("com.example.hello-myo");

    std::cout << "Attempting to find a Myo..." << std::endl;

    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo::Myo* myo = hub.waitForMyo(10000);

    // If waitForAnyMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

    // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
    DataCollector collector = DataCollector();

    // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
    // Hub::run() to send events to all registered device listeners.
    hub.addListener(&collector);

    // Finally we enter our main loop.
    while (1) {
        // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
        // In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
        hub.run(1000/20);
        // After processing events, we call the print() member function we defined above to print out the values we've
        // obtained from any events that have occurred.
        collector.print();
    }

    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
