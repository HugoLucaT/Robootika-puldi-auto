# Projekti dokumentatsioon

## 1. Projekti eesmärk ja seadme lühikirjeldus
Projekti eesmärk on luua väikene autonoomne puldiauto, mida saab juhtida täielikult veebilehitseja kaudu. Auto tekitab ise Wi-Fi tugijaama (AP-režiim), kuhu kasutaja saab telefonist või arvutist ühenduda. See lahendab olukorra, kus pole vaja eraldi füüsilist pulti ega Bluetooth-rakendust – piisab vaid veebilehitsejast, mis töötab kõigis seadmetes.

Süsteemi põhifunktsioonid:
- auto edasi-tagasi liikumine;
- pööramine vasakule ja paremale;
- kiiruse reguleerimine (PWM);
- STOP nupp;
- "Go back home" funktsioon – auto suudab samm-sammult tagasi liikuda mööda sama teekonda, mida kasutaja eelnevalt manuaalselt läbis.

## 2. Sisendite loetelu
2.1 Kasutaja veebi kaudu tehtavad sisendid
- Nupp “Up” → POST /u → alustab edasi liikumist
- Nupp “Down” → POST /d → alustab tagurdamist
- Nupp “Left” → POST /l → pöörab vasakule
- Nupp “Right” → POST /r → pöörab paremale
- Nupp “Stop” → POST /stop → peatab auto
- Kiiruse liugur (speed slider) → GET /speed/:value → muudab PWM mootori kiirust
- “Reset Home” → POST /resetHome → puhastab salvestatud liikumiskäsud
- “Go back to home” → POST /home → sõidab sama teed tagasi alguspunkti

2.2 Riistvaralised sisendid
- Kokkupõrkeandur (collision sensor)
Füüsiline sisend: takistuslüliti / bumper switch
Arduino sisend: digipin 2
Funktsioon: kui tuvastab kokkupõrke → auto peatub automaatselt

## 3. Väljundite loetelu
Mootorid liiguvad edasi → DC mootorid (PWM kiiruse juhtimine)
Mootorid pööravad vasakule/paremale → vasaku/parema mootori suud pööratakse ümber
Auto tagurdab → mõlema mootori suund pööratakse ümber
Auto peatub → mootoritele antakse 0 signaal
Kiiruse muutus → PWM signaal mootoritele (speedPinL, speedPinR)
“Go Home” funktsioon → auto mängib tagurpidi läbi kõik kasutaja käsud koos sama ajastusega

## 4. Nõuded loodavale seadmele
Liikumine
- Kui vajutatakse "Up", siis autod sõidab edasi seni, kuni vajutatakse mõnd muud nuppu.
- Kui vajutatakse "Down", auto liigub tagurpidi.
- Kui vajutatakse "Left", auto pöörab vasakule (parem mootor edasi, vasak mootor tagasi).
- Kui vajutatakse "Right", auto pöörab paremale.
- Kui vajutatakse "Stop", auto peatub koheselt.

Kiirus
Kui kasutaja muudab speed slider väärtust, siis:
- PWM väärtus seatakse uueks kiiruseks (100–255) muudatus rakendub kohe liikumisele.

Kokkupõrkeanduri loogika
Kui collision sensor aktiveerub, siis:
- auto peatub automaatselt
- liikumist ei jätkata enne, kui kasutaja vajutab uut nuppu

Liikumise mälufunktsioon (Home)
Iga liikumiskäsk salvestatakse järjekorda koos ajastusega.
Kui vajutatakse “Go back to home”, siis:
- auto lõpetab praeguse liikumise;
- auto kordab käske vastupidises järjekorras;
- iga liigutus kestab sama kaua kui algselt;
- suunad on vastupidised (edasi → tagasi, vasak → parem jne).

Kui vajutatakse “Reset Home”, kustutatakse kogu salvestatud liikumisajalugu.

## 5. Süsteemi füüsiliste komponentide loetelu
- Mikrokontroller: Arduino Uno R4 Wifi
- Toiteplokk / aku: 2x 3.7v 18650
- Mootor / servo / aktuaatorid: 4x OSOYOO BasicMover DC motor
- Nuppud: 2x microswitch
- Draiverplaadid / moodulid: OSOYOO model X motor driver module
- Kinnitused ja mehhaanika: Upper Chassis for V2.0 OSOYOO Robot Car, Lower car chassis for OSOYOO V2.1 robot car
- 21 juhet
- 4 ratast

## 6. Ühendusskeem
/hardware/scheme.fzz

## 7. Süsteemi juhtiv kood (või pseudokood)
Koodi põhiloogika:
- Arduino loob Wi-Fi AP võrgu nimega "Puldiauto".
- aWOT veebiserver kuulab pordil 80.
- Avaleht näitab HTML nuppe: Up, Down, Left, Right, Stop, Speed, Home.
- Iga POST-päring vallandab vastava funktsiooni (go_Advance, go_Left jne).
- Kiiruse muutmine toimub REST stiilis päringuga /speed/:value.
- Kõik liikumiskäsud salvestatakse massiivi home[] koos kestusega.
- Home funktsioon mängib käsud vastupidises järjekorras tagasi.
