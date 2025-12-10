// Kood võetud https://github.com/nullyks/Arduino_UNO_R4_server/blob/main/WebServer_Button_simple/WebServer_Button_simple.ino


/*
  UNO R4 WiFi + aWOT (nupud, ilma API-ta, 303 redirect)

  Iga nupu vajutus POSTib /g, /y või /r.
  Server kutsub vastava funktsiooni ja suunab 303-ga tagasi /.
*/

#include <WiFiS3.h>
#include <aWOT.h>

// 1) Wi-Fi seaded
const char* WIFI_NIMI   = "UNO_R4";
const char* WIFI_PAROOL = "arduino123";

// 2) Server
WiFiServer server(80);
Application app;

// 3) Riistvarafunktsioonid (asenda oma koodiga)
void roheline() { Serial.print("roheline="); Serial.println("vajutati!");}
void kollane() { Serial.print("kollane="); Serial.println("vajutati!");}
void punane() { Serial.print("punane="); Serial.println("vajutati!");}

// 4) Esileht – kolm vormi-nuppu (POST)
void naitaEsilehte(Request &req, Response &res) {
  res.set("Content-Type", "text/html; charset=utf-8");
  res.println("<!doctype html>");
  res.println("<html lang='et'>");
  res.println("<meta charset='utf-8'>");
  res.println("<meta name='viewport' content='width=device-width, initial-scale=1.4'>");
  res.println("<title>Valgusfoor</title>");
  res.println("<h1>Valgusfoor</h1>");
  res.println("<form action='/g' method='post'><button type='submit'>Roheline</button></form>");
  res.println("<form action='/y' method='post'><button type='submit'>Kollane </button></form>");
  res.println("<form action='/r' method='post'><button type='submit'>Punane  </button></form>");
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

void vajutaG(Request &req, Response &res) {
  roheline();
  redirectHome(res);
}
void vajutaY(Request &req, Response &res) {
  kollane();
  redirectHome(res);
}
void vajutaR(Request &req, Response &res) {
  punane();
  redirectHome(res);
}

// 6) setup/loop
void setup() {
  Serial.begin(9600);
  Serial.println("Käivitan Wi-Fi AP...");
  int tulemus = WiFi.beginAP(WIFI_NIMI, WIFI_PAROOL);
  Serial.print("AP staatus: "); Serial.println(tulemus);
  Serial.print("Mine aadressile: http://"); Serial.println(WiFi.localIP());

  app.get("/",  naitaEsilehte);
  app.post("/g", vajutaG);
  app.post("/y", vajutaY);
  app.post("/r", vajutaR);

  server.begin();
  Serial.println("Veebiserver kuulab pordil 80.");
}

void loop() {
  WiFiClient klient = server.available();
  if (!klient) return;
  app.process(&klient);
  klient.stop();
}