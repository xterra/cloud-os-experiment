#include <iostream>

#include <unistd.h>

#include "xpointer.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

void testPoints() {
    std::cout << "Testing X";
    Display *display = XOpenDisplay(NULL);
    if(display == NULL){
        std::cerr << "Failed to detect main display!" << std::endl;
        exit(EXIT_FAILURE);
    }
    XWarpPointer(display, None, None, 0, 0, 0, 0, 5, 5);
    sleep(1);
    XWarpPointer(display, None, None, 0, 0, 0, 0, 50, 5);
    sleep(1);
    XWarpPointer(display, None, None, 0, 0, 0, 0, 5, 50);
    sleep(1);
    XWarpPointer(display, None, None, 0, 0, 0, 0, 50, 50);
    XCloseDisplay(display);
}
