#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

Display *display;
Window rootWin;

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
        
        std::string message = msg->get_payload();
        std::vector<std::string> parsedMessage = split(message, ":");
        
        int x = std::stoi(parsedMessage[0]);
        int y = std::stoi(parsedMessage[1]);
        
        XWarpPointer(display, None, rootWin, 0, 0, 0, 0, x, y);
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
