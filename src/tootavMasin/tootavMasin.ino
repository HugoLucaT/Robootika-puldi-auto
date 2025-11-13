/*
  UNO R4 WiFi + aWOT (nupud, ilma API-ta, 303 redirect)

  Iga nupu vajutus POSTib /g, /y või /r.
  Server kutsub vastava funktsiooni ja suunab 303-ga tagasi /.
*/

#include <WiFiS3.h>
#include <aWOT.h>

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

void go_Advance(void) //Edasi
{
  Serial.print(speed);
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
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
void stop_Stop() //Peatu
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
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
  res.println("<input type='range' id='speedSlider' min='0' max='255' value='200' oninput='this.nextElementSibling.value=this.value'>");
  res.println("<output>200</output>");
  res.println("<button type='button' onclick='setSpeed()'>Set Speed</button>");

  res.println("<script>");
  res.println("function setSpeed() {");
  res.println("  var val = document.getElementById('speedSlider').value;");
  res.println("  fetch('/speed/' + val);");
  res.println("}");
  res.println("</script>");
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
  redirectHome(res);
}
void left(Request &req, Response &res) {
  go_Left();
  redirectHome(res);
}
void right(Request &req, Response &res) {
  go_Right();
  redirectHome(res);
}
void back(Request &req, Response &res) {
  go_Back();
  redirectHome(res);
}
void stop(Request &req, Response &res) {
  stop_Stop();
  redirectHome(res);
}
void setSpeed(Request &req, Response &res) {
  Serial.print("11");
  char value[8];

  if(req.route("value", value, sizeof(value))){
    speed = atoi(value);
  }

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


  server.begin();
  Serial.println("Veebiserver kuulab pordil 80.");
}


void loop() {
  WiFiClient klient = server.available();
  if (!klient) return;
  app.process(&klient);
  klient.stop();
}