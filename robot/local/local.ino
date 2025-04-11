#include <Middlewares.h>
#include <WebServer.h>
#include <WiFi.h>

const char *ap_ssid = "ESP32_Demo";
const char *ap_password = "";

String rd = "";
WebServer server(80);

LoggingMiddleware logger;
CorsMiddleware cors;
AuthenticationMiddleware auth;

volatile double temp = 24.0;
volatile double humid = 45.6;

void setup(void) {
    Serial.begin(115200);

    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    WiFi.softAP(ap_ssid, ap_password);

    Serial.print("IP address: ");
    Serial.println(WiFi.AP.localIP());

    cors.setOrigin("http://192.168.4.1");
    cors.setMethods("POST,GET,OPTIONS,DELETE");
    cors.setHeaders("X-Custom-Header");
    cors.setAllowCredentials(false);
    cors.setMaxAge(600);

    auth.setUsername("admin");
    auth.setPassword("admin");
    auth.setRealm("My Super App");
    auth.setAuthMethod(DIGEST_AUTH);
    auth.setAuthFailureMessage("Authentication Failed");

    server.addMiddleware(&cors);

    server.on("/", []() {
        server.send(200, "text/html",
                    R"html(
    <!DOCTYPE html>  
    <html>
      <head>
        <title>IoT</title>
      </head>
      <body>
        <h1>Welcome to dashboard</h1>
        <p>Hello, World!</p>
        <p>Speed: <span id="speed">loading...</span> rpm<p>
        <p>Heading: <span id="heading">loading...</span> degree<p>
        <button onclick="move('lt')">CCW</button>
        <button onclick="move('rt')">CW</button>
        <br/>
        <button onclick="move('lf')">Q</button>
        <button onclick="move('fw')">W</button>
        <button onclick="move('rf')">E</button>
        <br/>
        <button onclick="move('lw')">A</button>
        <button onclick="move('st')">O</button>
        <button onclick="move('rw')">D</button>
        <br/>
        <button onclick="move('lb')">Z</button>
        <button onclick="move('bw')">X</button>
        <button onclick="move('rb')">C</button>
      </body>
      <script>
          async function fetchData() {
            let response = await fetch("/data");
            let data = await response.json();
            let csv = data["d"];
            const myArray = csv.split(",");
            document.getElementById("heading").innerHTML = myArray[1]/10;
            document.getElementById("speed").innerHTML = myArray[0];
            // document.getElementById("heading").innerHTML = data["h"];
            // document.getElementById("speed").innerHTML = data["s"];
          }
          async function move(dir) {
            let response = await fetch("/" + dir);
          }
          async function fw() {
            let response = await fetch("/fw");
          }
          async function bw() {
            let response = await fetch("/bw");
          }
          async function rt() {
            let response = await fetch("/rt");
          }
          async function lt() {
            let response = await fetch("/lt");
          }
          setInterval(fetchData, 2500);
      </script>
    </html>)html");
    });

    server.onNotFound([]() {
        server.send(404, "text/plain", "Page not found");
    });

    server.on("/data", []() {
        char buffer[50];
        // sprintf(buffer, "{\"s\":%d,\"h\":%d}", speed, heading);
        sprintf(buffer, "{\"d\":\"%s\"}", rd);
        server.send(200, "text/json", buffer);
    });
    server.on("/st", []() {
        Serial2.print("ST\n");
    });
    server.on("/fw", []() {
        Serial2.print("FW\n");
    });
    server.on("/bw", []() {
        Serial2.print("BW\n");
    });

    server.on("/rw", []() {
        Serial2.print("RW\n");
    });
    server.on("/rf", []() {
        Serial2.print("RF\n");
    });
    server.on("/rb", []() {
        Serial2.print("RB\n");
    });
    server.on("/rt", []() {
        Serial2.print("RT\n");
    });

    server.on("/lw", []() {
        Serial2.print("LW\n");
    });
    server.on("/lf", []() {
        Serial2.print("LF\n");
    });
    server.on("/lb", []() {
        Serial2.print("LB\n");
    });
    server.on("/lt", []() {
        Serial2.print("LT\n");
    });

    server.collectAllHeaders();
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
    delay(2);  // allow the cpu to switch to other tasks
    if (Serial2.available()) {
        String data = Serial2.readString();
        data.trim();
        // Serial.println(data);
        rd = data;
    }
}
