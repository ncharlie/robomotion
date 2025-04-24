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

void setup(void) {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    WiFi.softAP(ap_ssid, ap_password);

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

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
    <p>Speed: <span id="speed">loading...</span> rpm</p>
    <div style="display: flex; gap: 10px; margin-top: 10px;">
       <button onclick="setSpeed('s1')">1</button>
       <button onclick="setSpeed('s2')">2</button>
       <button onclick="setSpeed('s3')">3</button>
       <button onclick="setSpeed('s4')">4</button>
    </div>
    <p>Heading: <span id="heading">loading...</span> degree</p>
    <p>Set Heading: <input type="number" id="targetHeading" min="0" max="359">
    <button onclick="setHeading()">Go</button>
    </p>

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
//      let keysPressed = {};
//
//      document.addEventListener("keydown", function(event) {
//          keysPressed[event.key] = true;
//
//          if (keysPressed["ArrowUp"] && keysPressed["ArrowLeft"]) move("lf");
//          else if (keysPressed["ArrowUp"] && keysPressed["ArrowRight"]) move("rf");
//          else if (keysPressed["ArrowDown"] && keysPressed["ArrowLeft"]) move("lb");
//          else if (keysPressed["ArrowDown"] && keysPressed["ArrowRight"]) move("rb");
//          else if (event.key === "ArrowUp") move("fw");
//          else if (event.key === "ArrowDown") move("bw");
//          else if (event.key === "ArrowLeft") move("lw");
//          else if (event.key === "ArrowRight") move("rw");
//          else if (event.key === "o" || event.key === "O") move("st");
//      });
//
//      document.addEventListener("keyup", function(event) {
//          delete keysPressed[event.key];
//          if (
//            !keysPressed["ArrowUp"] &&
//            !keysPressed["ArrowDown"] &&
//            !keysPressed["ArrowLeft"] &&
//            !keysPressed["ArrowRight"]
//        ) {
//            move("st");  // send stop command
//        }
//      });

      async function move(dir) {
          await fetch("/" + dir);
      }

      async function setSpeed(speedCode) {
          await fetch("/" + speedCode);
          document.getElementById("speed").textContent = speedCode;
      } 

      async function setHeading() {
          const heading = document.getElementById("targetHeading").value;
          if (heading === "") return;
      
          await fetch(`/setHeading?value=${heading}`);  // Template literal
      }

      async function fetchData() {
          let response = await fetch("/data");
          let data = await response.json();
          let csv = data["d"];
          const myArray = csv.split(",");
          document.getElementById("heading").innerHTML = myArray[1] / 10;
          document.getElementById("speed").innerHTML = myArray[0];
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
        sprintf(buffer, "{\"d\":\"%s\"}", rd);
        server.send(200, "text/json", buffer);
    });

    server.on("/st", []() { Serial2.print("ST\n"); });
    server.on("/fw", []() { Serial2.print("FW\n"); });
    server.on("/bw", []() { Serial2.print("BW\n"); });
    server.on("/rw", []() { Serial2.print("RW\n"); });
    server.on("/rf", []() { Serial2.print("RF\n"); });
    server.on("/rb", []() { Serial2.print("RB\n"); });
    server.on("/rt", []() { Serial2.print("RT\n"); });
    server.on("/lw", []() { Serial2.print("LW\n"); });
    server.on("/lf", []() { Serial2.print("LF\n"); });
    server.on("/lb", []() { Serial2.print("LB\n"); });
    server.on("/lt", []() { Serial2.print("LT\n"); });
    server.on("/s1", []() { Serial2.print("S1\n"); });
    server.on("/s2", []() { Serial2.print("S2\n"); });
    server.on("/s3", []() { Serial2.print("S3\n"); });
    server.on("/s4", []() { Serial2.print("S4\n"); });
    server.on("/setHeading", HTTP_GET, []() {
        if (server.hasArg("value")) {
            String value = server.arg("value");
            Serial2.print("H" + value + "\n");  // e.g., H120\n
            server.send(200, "text/plain", "Heading command sent");
        } else {
            server.send(400, "text/plain", "Missing heading value");
        }
    });



    server.collectAllHeaders();
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
    delay(2);
    if (Serial2.available()) {
        String data = Serial2.readString();
        data.trim();
        rd = data;
    }
}
