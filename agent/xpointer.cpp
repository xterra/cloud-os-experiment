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
    
    int i;
    for (i=0;i<=20;i++) {
        std::cout << i << ":" << i << std::endl;
        XWarpPointer(display, None, rootWin, 0, 0, 0, 0, i*5+10, i*5+10);
        XFlush(display);
        sleep(1);
    }
    
    XCloseDisplay(display);
}
