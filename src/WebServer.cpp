#include "WebServer.h"

#define WEBSERVER_PORT 80

WiFiServer server(WEBSERVER_PORT);

void WebServer::start()
{
    server.begin();

    Serial.print("Web server listening on port ");
    Serial.println(WEBSERVER_PORT);

    this->running = true;
}

void WebServer::stop()
{
    this->running = false;
}

void WebServer::loop()
{
    if (!running)
        return;

    WiFiClient client = server.available(); // listen for incoming clients
    if (client)
    {
        Serial.println("new client"); // print a message out the serial port
        String currentLine = "";      // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        // end of HTTP request
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        client.print("<h1>Hello World!</h1>");
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    {
                        Serial.println(currentLine);
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        client.stop(); // close the connection:
        Serial.println("client disconnected");
    }
}