#include <iostream>

#include <unistd.h>

#include "xpointer.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

void testPoints() {
    std::cout << "Testing X" << std::endl;
    Display *display = XOpenDisplay(NULL);
    if(display == NULL){
        std::cerr << "Failed to detect main display!" << std::endl;
        exit(EXIT_FAILURE);
    }

    int screenNumber = DefaultScreen(display);
    std::cerr << "Default screen: " << screenNumber << std::endl;
    Window rootWin = DefaultRootWindow(display);
    
    XSelectInput(display, rootWin, KeyReleaseMask);
    
    std::cout << "5:5" << std::endl;
    XWarpPointer(display, None, rootWin, 0, 0, 0, 0, 100, 100);
    XFlush(display);
    sleep(1);
    std::cout << "50:5" << std::endl;
    XWarpPointer(display, None, rootWin, 0, 0, 0, 0, 200, 200);
    XFlush(display);
    sleep(1);
    std::cout << "5:50" << std::endl;
    XWarpPointer(display, rootWin, rootWin, 0, 0, 0, 0, 5, 50);
    XFlush(display);
    sleep(1);
    std::cout << "50:50" << std::endl;
    XWarpPointer(display, rootWin, rootWin, 0, 0, 0, 0, 50, 50);
    XFlush(display);
    sleep(1);
    XCloseDisplay(display);
}
