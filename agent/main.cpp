#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XTest.h>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

Display *display;
Window rootWin;


enum remoteControlType {
    UNKNOWN_CONTROL,
    MOUSE,
    TOUCH,
    KEYBOARD
};

enum remoteControlEventType {
    RESET,
    MOUSEUP,
    MOUSEDOWN,
    MOUSEMOVE,
    TOUCHSTART,
    TOUCHEND,
    TOUCHCANCEL,
    TOUCHMOVE,
    KEYDOWN,
    KEYUP
};

std::vector<remoteControlEventType> eventTypeAssociation = {
    RESET,
    MOUSEUP,
    MOUSEDOWN,
    MOUSEMOVE,
    TOUCHSTART,
    TOUCHEND,
    TOUCHCANCEL,
    TOUCHMOVE,
    KEYDOWN,
    KEYUP
};

std::map<remoteControlEventType, remoteControlType> eventControlAssociation = {
    { RESET, UNKNOWN_CONTROL },
    { MOUSEUP, MOUSE },
    { MOUSEDOWN, MOUSE },
    { MOUSEMOVE, MOUSE },
    { TOUCHSTART, TOUCH },
    { TOUCHEND, TOUCH },
    { TOUCHCANCEL, TOUCH },
    { TOUCHMOVE, TOUCH },
    { KEYDOWN, KEYBOARD },
    { KEYUP, KEYBOARD }
};

std::vector<std::string> split(std::string str,std::string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

class RemoteControlEvent {
    public:
        RemoteControlEvent(std::string payload) {
            setRawData(payload);
            std::vector<std::string> parsedRawData = split(payload, ":");
            setEventType(parsedRawData[0]);
            setTimestamp(parsedRawData[1]);
            setValue(parsedRawData);
        }
        bool isValid() {
            return valid;
        }
        std::string getRawData() {
            return rawData;
        }
        int getTimestamp() {
            return timestamp;
        }
        int getControlValues() {
            return timestamp;
        }
        remoteControlEventType getEventType() {
            return eventType;
        }
        remoteControlType getControlType() {
            return controlType;
        }
        std::vector<std::string> getValue() {
            return value;
        }
    private:
        bool valid = true;
        std::string rawData;
        int timestamp = 0;
        std::vector<std::string> value;
        remoteControlType controlType = UNKNOWN_CONTROL;
        remoteControlEventType eventType = RESET;
        void setRawData(std::string data) {
            rawData = data;
        }
        void setEventType(std::string typeNumber) {
            try {
                setEventType(std::stoi(typeNumber));
            } catch (...) {
                valid = false;
            }
        }
        void setEventType(int typeNumber) {
            try {
                eventType = eventTypeAssociation[typeNumber];
                setControlType(eventType);
            } catch (...) {
                valid = false;
            }
        }
        void setControlType(remoteControlEventType eventType) {
            try {
                controlType = eventControlAssociation[eventType];
            } catch (...) {
                valid = false;
            }
        }
        void setTimestamp(std::string timestamp) {
            try {
                setTimestamp(std::stoi(timestamp));
            } catch (...) {
                valid = false;
            }
        }
        void setTimestamp(int timestampValue) {
            timestamp = timestampValue;
        }
        void setValue(std::vector<std::string> data) {
            value.clear();
            for (int i = 0; i < data.size(); i++) {
                if (i > 1) {
                    value.push_back(data[i]);
                }
            }
        }
};


int convertKey(std::string key) {
    if (key.size() > 1) {
        char *key_cstr = &key[0];
        KeySym keysym = XStringToKeysym(key_cstr);
        return XKeysymToKeycode(display, keysym);
        return 0;
    } else {
        return XKeysymToKeycode(display, key.at(0));
    }
}


// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
//    std::cout << "on_message called with hdl: " << hdl.lock().get()
//              << " and message: " << msg->get_payload()
//              << std::endl;

    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        
        RemoteControlEvent event = RemoteControlEvent(msg->get_payload());
        std::vector<std::string> eventValue = event.getValue();
        remoteControlEventType eventType = event.getEventType();
        
        // XSendEvent(dis, PointerWindow,True,NoEventMask,e);
        // XAllowEvents(dis, ReplayPointer, CurrentTime);
        // XSync(dis, 0);
        
        if (eventType == MOUSEMOVE || eventType == TOUCHMOVE) {
            XWarpPointer(display, None, rootWin, 0, 0, 0, 0, std::stoi(eventValue[0]), std::stoi(eventValue[1]));
        } else if (eventType == MOUSEDOWN || eventType == TOUCHSTART) {
            XTestFakeButtonEvent(display, 1, True, CurrentTime);
//            XTestFakeButtonEvent(display, 1, False, CurrentTime);
//            XEvent ev;
//            ev.xbutton.type = ButtonPress;
//            ev.xbutton.type = ButtonRelease;
//            ev.xkey.window = rootWin;
//            ev.xkey.root = ev.xkey.subwindow = None;
//            ev.xbutton.x = std::stoi(eventValue[0]);
//            ev.xbutton.y = std::stoi(eventValue[1]);
//            ev.xbutton.x_root = 0;
//            ev.xbutton.y_root = 0;
//            XSendEvent(display, rootWin, True, ButtonPress, &ev);
        } else if (eventType == MOUSEUP || eventType == TOUCHEND || eventType == TOUCHCANCEL) {
            XTestFakeButtonEvent(display, 1, False, CurrentTime);
        } else if (eventType == KEYDOWN) {
            int key = convertKey(eventValue[0]);
            std::cout << "Key " << key << std::endl;
            if (key != 0) {
                XTestFakeKeyEvent(display, key, True, CurrentTime);
            }
//            XTestFakeKeyEvent(display, std::stoi(eventValue[0]), True, CurrentTime);
//            XTestFakeKeyEvent(display, XKeysymToKeycode(display, eventValue[0].at(0)), True, CurrentTime);
//            XEvent ev;
//            KeySym key;
//            ev.xkey.type = KeyPress;
//            ev.xkey.window = rootWin;
//            ev.xkey.root = ev.xkey.subwindow = None;
//            ev.xkey.time = 0;
//            ev.xkey.x = ev.xkey.y = ev.xkey.x_root = ev.xkey.y_root = 0;
//            ev.xkey.state = 0;
//            key = XStringToKeysym("7");
//            ev.xkey.keycode = XKeysymToKeycode(display,key);
//            ev.xkey.same_screen = True;
//            XSendEvent(display, rootWin, True, KeyPressMask, &ev);
        } else if (eventType == KEYUP) {
            int key = convertKey(eventValue[0]);
            if (key != 0) {
                XTestFakeKeyEvent(display, key, False, CurrentTime);
            }
        }
        
        XFlush(display);
        
        s->send(hdl, msg->get_payload(), msg->get_opcode());
        
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

int main() {
    
    display = XOpenDisplay(NULL);
    if(display == NULL){
        std::cerr << "Failed to detect main display!" << std::endl;
        exit(EXIT_FAILURE);
    }
    int screenNumber = DefaultScreen(display);
    std::cerr << "Default screen: " << screenNumber << std::endl;
    rootWin = DefaultRootWindow(display);
    XSelectInput(display, rootWin, KeyReleaseMask);
    
    // Create a server endpoint
    server echo_server;
    int port = 9002;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));

        // Listen on port 9002
        echo_server.listen(port);

        // Start the server accept loop
        echo_server.start_accept();
        
        std::cout << "Started and listening port " << port << std::endl;

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
    
}
