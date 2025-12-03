/*
  UNO R4 WiFi + aWOT (nupud, ilma API-ta, 303 redirect)

  Iga nupu vajutus POSTib /g, /y või /r.
  Server kutsub vastava funktsiooni ja suunab 303-ga tagasi /.
*/

#include <WiFiS3.h>
#include <aWOT.h>

#define collision 2
#define speedPinR 9
#define RightMotorDirPin1  12
#define RightMotorDirPin2  11
#define speedPinL 6
#define LeftMotorDirPin1  7
#define LeftMotorDirPin2  8

// 1) Wi-Fi seaded
const char* WIFI_NIMI   = "Puldiauto";
const char* WIFI_PAROOL = "arduino123";

// 2) Server
WiFiServer server(80);
Application app;



// 3) Riistvarafunktsioonid
int speed = 200;
struct RobotOrder {
    int command;
    unsigned long time;
};

int maxCommands = 50;
RobotOrder home[20];
int homeCount = 0;
unsigned long lastTime = 0; 

void go_Advance(int t=0) //Edasi
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
  delay(t);
}
void go_Left(int t=0)  //Pööra vasakule
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
  delay(t);
}
void go_Right(int t=0)  //Pööra paremale
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
  delay(t);
}
void go_Back(int t=0)  //Tagurda
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
  delay(t);
}
void stop_Stop(int t=0) //Peatu
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
  delay(t);
}

// Koju minemise funktsioon
void pushCommand(int cmd) {
  if (homeCount < maxCommands) {
    unsigned long now = millis();
    unsigned long dt = now - lastTime;

    if (homeCount == 0) {
      lastTime = now;
    } else {
      home[homeCount - 1].time = dt;
    }

    home[homeCount].command = cmd;

    lastTime = now;
    homeCount++;
  }
}

// 4) Esileht – kolm vormi-nuppu (POST)
void frontPage(Request &req, Response &res) {
  res.set("Content-Type", "text/html; charset=utf-8");
  res.println("<!doctype html>");
  res.println("<html lang='et'>");
  res.println("<meta charset='utf-8'>");
  res.println("<meta name='viewport' content='width=device-width, initial-scale=1.4'>");
  res.println("<title>Puldi auto</title>");
  res.println("<h1>Puldi auto</h1>");
  res.println("<form action='/u' method='post'><button type='submit'>Up</button></form>");
  res.println("<form action='/l' method='post'><button type='submit'>Left</button></form>");
  res.println("<form action='/r' method='post'><button type='submit'>Right</button></form>");
  res.println("<form action='/d' method='post'><button type='submit'>Down</button></form>");
  res.println("<form action='/stop' method='post'><button type='submit'>Stop</button></form>");
  res.println("<label>Speed:</label>");
  res.println("<input type='range' id='speedSlider' min='100' max='255' value='"+ String(speed)+ "' oninput='this.nextElementSibling.value=this.value'>");
  res.println("<output>200</output>");
  res.println("<button type='button' onclick='setSpeed()'>Set Speed</button>");

  res.println("<script>");
  res.println("function setSpeed() {");
  res.println("  var val = document.getElementById('speedSlider').value;");
  res.println("  fetch('/speed/' + val);");
  res.println("}");
  res.println("</script>");
  res.println("<form action='/resetHome' method='post'><button type='submit'>Reset Home</button></form>");
  res.println("<form action='/home' method='post'><button type='submit'>Go back to home</button></form>");
  res.println("</html>");

  
}

// 5) Handlerid – kutsuvad funktsiooni ja suunavad tagasi esilehele
void redirectHome(Response &res) {
  res.status(303);              
  res.set("Location", "/");     // sihtkoht
  res.set("Content-Type", "text/plain; charset=utf-8");
  res.println("OK");
  res.end();
}

void up(Request &req, Response &res) {
  go_Advance();
  pushCommand(1);
  redirectHome(res);
}
void left(Request &req, Response &res) {
  go_Left();
  pushCommand(2);
  redirectHome(res);
}
void right(Request &req, Response &res) {
  go_Right();
  pushCommand(3);
  redirectHome(res);
}
void back(Request &req, Response &res) {
  go_Back();
  pushCommand(4);
  redirectHome(res);
}
void stop(Request &req, Response &res) {
  stop_Stop();
  pushCommand(5);
  redirectHome(res);
}

void setSpeed(Request &req, Response &res) {
  char value[8];

  if(req.route("value", value, sizeof(value))){
    speed = atoi(value);

    analogWrite(speedPinL, speed);
    analogWrite(speedPinR, speed);
  }
  redirectHome(res);
}

void resetHome(Request &req, Response &res){
  homeCount = 0;
  redirectHome(res);
}

void returnHome(Request &req, Response &res){
  pushCommand(5);
  for (int i = homeCount; i >= 0; i--) {
    switch (home[i].command){
      case 1:
        go_Back(home[i].time);
        Serial.print("End of back");
        break;
      case 2:
        go_Right(home[i].time);
        Serial.print("End of right");
        break;
      case 3:
        go_Left(home[i].time);
        Serial.print("End of left");
        break;
      case 4:
        go_Advance(home[i].time);
        Serial.print("End of foward");
        break;
      case 5:
        stop_Stop(100);
        break;
    }
  }
  stop_Stop();
  homeCount = 0;
  redirectHome(res);
}

void init_GPIO()
{
	pinMode(RightMotorDirPin1, OUTPUT); 
	pinMode(RightMotorDirPin2, OUTPUT); 
	pinMode(speedPinL, OUTPUT);  
 
	pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT); 
  pinMode(speedPinR, OUTPUT); 
	stop_Stop();
}

// 6) setup/loop
void setup() {
  init_GPIO();
  pinMode(collision,INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Käivitan Wi-Fi AP...");
  int tulemus = WiFi.beginAP(WIFI_NIMI, WIFI_PAROOL);
  Serial.print("AP staatus: "); Serial.println(tulemus);
  Serial.print("Mine aadressile: http://"); Serial.println(WiFi.localIP());

  app.get("/",  frontPage);
  app.post("/u", up);
  app.post("/l", left);
  app.post("/r", right);
  app.post("/d", back);
  app.post("/stop", stop);
  app.get("/speed/:value", setSpeed);
  app.post("/resetHome", resetHome);
  app.post("/home", returnHome);


  server.begin();
  Serial.println("Veebiserver kuulab pordil 80.");
}


void loop() {
  if(!digitalRead(collision)){
    stop_Stop();
  }
  WiFiClient klient = server.available();
  if (!klient) return;
  app.process(&klient);
  klient.stop();
}