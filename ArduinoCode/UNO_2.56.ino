////////////////
////////////////
////////////////
//Reizt den Uno ziemlich aus, Programmspeicher und Variablen sind zu 85% belegt
//Wenn der Programmspeicherplatz mit mehr als 85% belegt wird funktionieren die Relais nicht mehr, wohl wegen den unsigend longs die überlaufen
//mit lCD Keypad shield sind alle nutzbaren Pins außer A6 und A7 vom Uno belegt. mehr geht nicht.
//beim shield wurde der poti für den kontrast nach hinten verlegt und die icsp pins abgebrochen
//der button right wird nicht verwendet
/*Changelog
 * 2.01 Temperaturmessungen, Heizungsregelung, Buzzer, Timer, Button, Automatik Start, Automatik
 * 2.02 Automatik verbessert, allg. Verbesserungen
 * 2.03 Rührersteuerung integriert, Hopfentimer, Hopfengaben Reminder, Manuell, allg. Verbesserungen
 * 2.04 Signal LEDs für Blende, allg. Verbesserungen
 * 2.05 allg. Verbesserugen, mit GoBetwino angefangen
 * 2.06 GoBetwino integriert (Arduino sendet über USB LogDaten an gobetwino.exe und speichert sie in Datei; http://www.mikmo.dk/gobetwino.html), globale Variablen nachvollziehbarer gemacht, Changelog erstellt, allg. Verbesserungen
 *2.07 paar Bugs entfernt: LED Takt OK, Buzzer Takt OK;
 *2.08 LCD Keypad Shield Steuerung wieder eingebaut, Variablen im Sketch nach oben gestelllt, Keypad funktioniert, Manueller Modus bis auf Timer auch schon
 *2.09 manueller modus fertig, manuellen alarm und meldung eingebaut
 *2.10 manueller modus komplestt fertig, mit automaischen angefangen
 *2.11 viel rumprobieren und fehlersuche
 *2.12 manueller modus und automaisch modus sind komplett fertig und funktionieren 1a
 *2.2 alles läuft soweit ich es feststellen kann
 *2.21 versuche schnelleinstellung für hohe zahlen einzubauen
 *2.3 schnelleinstellung für hohe zahlen fertig eingebaut. alles soweit funktionsfähig. fertig???????
 *2.31 bugs bei automaischen und autokochen beseitigt
 *2.32 gobetwino überarbeitet
 *2.34 NTCs genauer machen
 *2.40 Keine bugs mehr gefunden, soweit ist alles bereit für die generalprobe. lediglich die ausgangswerte müsste man noch anpassen.
 *2.41 Referenzwiderstand von tempT auf 7640 Ohm geändert (ja der ist laut multimeter wirklich so gering)
 *2.42 Gradientenberechnung der Heizungsregelung angepasst. Gradient ist jetzt wirklich K/min. Automaischen, Eingemaischt wird jetzt nach erreichen der temperatur und nicht nach ablauf des ersten timers.
 *2.43 ausprobieren einer pid regelung: http://playground.arduino.cc/Code/PIDLibrary, hat nicht funktioniert
 *2.50 alles funktioniert. Programmspeicher darf 85% nicht überschreiten, sonst funktioniert das relais nicht mehr
 *2.51 Maximaltemperatur auf 102°C erhöht, Referenzwiderstand des Temperatursensors auf 100°C kalibriert
 *2.52 Wenn Kochtemp >= 100 °C ist dann ist die Heizung ununterbrochen eingeschalten, hoch und runterpfeile in Statusmenüs entfernt, heizung ist jetzt mindestens für 5 sekunden an, Softwarenummer im statusmenue angezeigt
 *2.53 hoch und runterpfeile wieder im Statusmenü, temperatur auf eine nachkommastelle gerundet, fehler bei der anzeige behoben
 *2.54 Rührer aus wenn beim kochen hundert grad erreicht werden um den deckel zu wechseln, tempSollErreicht beim kochen für 99,5 grad festgelegt, eingabe max und min beim kochen angepasst, K wert auf 0,75 erhöht
 *2.55 Timer sollten die Laufzeit des Programms jetzt berücksichtigen und dadurch genauer sein, unnötige unsigned long Varibalen zu int gemacht, Gobetwino Ausgabe auf gerundete Temperaturwerte angepasst
 *2.56 timer änderungen von 2.55 wieder rückgängig gemacht, da der Timer so nur noch ungenauer wurde, pin belegung geändert, Referenzwiderstand von NTC der Elektronik geändert (= der des Topfes)
 */
 float software = 2.56;
 /*TODO:
 *  -Erledigt: Heizungsregelung einstellen dh. richtigen Kwert finden. 
 *  -Erledigt: Temperaturmessung kalibrieren dh. Referenzwiderstand richtig einstellen. mit 100°C kalibriert
 *  -Erledigt: Abkühlung druch das Malz mit einbeziehen, dh den timer vom zweiten automaischen Schritt gleich nach dem einmaischen zu starten und nicht erst wenn die temp erreicht wurde
 *  -Erledigt: min und max werte an reale werte anpassen 
 *  -Erledigt: gobetwino testen
 *  -Erledigt: standardmäßg rührer stufe 0 ist nervig, eigentlich muss immer gerührt werden
 *  -Erledigt (hat aber nicht funktioniert, messung war mit gemessenem Widerstand total falsch):genauen Referenzwiderstand messen
 *BUGS:
 *- Behoben: timer für die zweite rast wartet nicht auf die einmaischenfertig bestätigung sondern fängt gleich an
 *- Behoben: nach einem durchlauf manuell lässt sich kein zweiter starten
 *- Timer ist ungenau, geht etwa eine Minute pro Stunde nach, wahrscheinlich wegen der Laufzeit des Programms
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Hardware Ports
//shield
//PORTS OBEN (=die löcher für pins, von links nach rechts)
//D13, D12, D11, D3, D2, --, --, (die letzten beiden nicht benutzen, dann spinnt der arduino)
//PORTS UNTEN (=die löcher für pins, von links nach rechts)
//RST, 3.5V, 5V, GND, GND, Vin
//alles irgendwo anschließen (nur die tempPins brauchen analoge pins)
//dann die belegung über das arduino bsp "blinkwithoutdelay" bestimmen
//und hier entsprechend ändern
//
const byte keypadPin    = A0;  // Analog Keypad an A0
//tempmessung
const byte tempTPin = A1;
const byte tempIPin = A2;
//relais (einfach über bsp "blinkwithoutdelay" bestimmen)
const byte ruehrerPin1 = A5;
const byte ruehrerPin2 = A3;
const byte heizungPin = A4;
//buzzer
const byte buzzerPin = 2;
//leds (einfach über bsp "blinkwithoutdelay" bestimmen)
const byte ruehrerLed1 = 3;
const byte ruehrerLed2 = 11;
const byte heizungLed = 13;
const byte programmLed = 12;
//////////////////////////////
///////////////////////////////
///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Variablen
//
//////////////////////////////////////////////////////////////////////////////Eingabe
byte tempMin = 35; //minimal Temperatur
byte tempMax = 85;
byte tempKochenMin = 90;
byte tempKochenMax = 102;
byte timerMin = 5; //minimal Timer
byte timerMax = 180;
byte timerKochenMin = 45;
int zaehlerTakt = 150; //alle x ms steigt der zähler um eins
unsigned long zaehlerStart = 0;
byte button;
boolean buttonPressed = false;
boolean meldung = false;
boolean automatikBestaetigung = false;
//
//////////////////////////////////////////////////////////////////////////////TemperaturSensoren
long samplesT[20];
long samplesI[5];
unsigned long sensorPreviousTime = 0;
//////////////////////////////////////////////////////////////////////////////Heizungsregelung
float k = 0.75; //Gradientenfaktor
int heizungESV = 10000; //Pause zwischen Aus- und Einschalten der Heizung
int heizungMesszyklus = 5000; //Abstand der Temperaturmessung für die Gradientenberechnung
boolean heizungOFF = false;
byte tempSoll = tempMin;
byte tempSensorDaempfung = 0; //dämpft überschwingen vom Temperatursensor
unsigned long heizungVorherigeZeit = 0; //Zeit bei t-1
float heizungVorherigeTemp = 0; //Temp bei t-1
unsigned long heizungWarten = 0; //Wartezeitmessung Start
boolean tempSollErreicht = false;
//
/////////////////////////////////////////////////////////////////////Rührersteuerung
int ruehrerPause = 1000; //stufe 2 benötigt stufe 1 um zu starten, stufe 1 läuft ruehrerpause lang bis stufe 2 startet
boolean ruehrerStart = false;
boolean ruehrerOFF = false;
byte ruehrerStufe = 1;
boolean ruehrerStufe1Gewartet = false;
boolean ruehrerPauseGestartet = false;
unsigned long ruehrerPreviousMillis;
//
////////////////////////////////////////////////////////////////////////////Buzzer Takt
int buzzerFrequenz = 831; //buzzerFrequenz des Peips in Hz
byte buzzerDauer = 150; //Piep-Dauer in ms
byte buzzerIntervall = 200; //Pause zwischen Piep in ms
boolean alarmOFF = false;
boolean alarmON = false;
unsigned long buzzerPreviousMillis = 0;
boolean buzzerON = false;
//
/////////////////////////////////////////////////////////////////////Led Takt
int ledIntervall = 1000; //zeit welche die led aus und an ist
unsigned long ledPreviousMillis = 0;
boolean ledON = false;
//
//////////////////////////////////////////////////////////////////////////////////////Timer
boolean datenUebergeben = false;
int timerMinuten;  //minutenanzeige
boolean timerStart = false;
int timerManuell = timerMin;
byte timerSekunden = 0; //sekundenanzeige
unsigned long timerSet = timerMin;
unsigned long timerPreviousMillis = 0;
boolean timerEnde = false;
//
///////////////////////////////////////////////////////////////////////////////////Hopfen Timer
unsigned long timerHopfenSet = timerMin;
unsigned long timerHopfenPreviousMillis = 0;
boolean timerHopfenEnde = false;
int timerHopfenMinuten;
boolean timerHopfenStart = false;
byte timerHopfenSekunden;
//
//////////////////////////////////////////////////////////////////////////////////AutoMaischen Modus
boolean einmaischenPause = false;
boolean keinHopfenMehr = false;
byte autoMaischenStatus = 1; //für Statusanzeige auf display
boolean autoMaischenStartDruecken = false;
boolean autoMaischenStart = false;
boolean autoMaischenEnde = false;
boolean jetztLaeutern = false;
byte tempEinmaischen = tempMin;
byte tempStep1 = tempMin;   //StartTemperaturen
byte tempStep2 = tempMin; 
byte tempStep3 = tempMin; 
byte tempStep4 = tempMin; 
byte tempStep5 = tempMin; 
byte tempStep6 = tempMin; 
int timerStep1 = timerMin;  //StartTimer Min
int timerStep2 = timerMin;
int timerStep3 = timerMin;
int timerStep4 = timerMin;
int timerStep5 = timerMin;
int timerStep6 = timerMin;
boolean einmaischenJetzt = false;
boolean einmaischenFertig = false;
byte currentStep = 1;
byte letzterStep;
byte tempStepMax; //maximaltemperatur während automatik
int timerStepMax;  //gesamtdauer der automatik
boolean maximaMaischen = false;  //berechnet die drei vorherigen werte
//
//////////////////////////////////////////////////////////////////////////////////AutoKochen Modus
int timerKochen = timerKochenMin;
byte tempKochen = tempKochenMin;
byte autoKochenStatus = 1; //für Statusanzeige auf display
boolean autoKochenStartDruecken = false;
int timerHopfen1 = timerMin;
int hopfenAlarm1;
int hopfenAlarm2;
int hopfenAlarm3;
int hopfenAlarm4;
int timerHopfen2 = timerMin;
int timerHopfen3 = timerMin;
int timerHopfen4 = timerMin;
int timerHopfen5 = timerMin;
boolean maximaKochen = false;
boolean autoKochenStart = false;
boolean autoKochenEnde = false;
byte hopfen = 1;
byte letzterHopfen = 1;

byte autoKochenMenue = 1;
//
//////////////////////////////////////////////////////////////////////////////Manueller Modus
boolean manuellStart = false;
byte manuellTemp = tempMin;
boolean manuellEnde = false;
//
/////////////////////////////////////////////////////////////////////////////Status Menü
byte statusMenue = 1;
//
///////////////////////////////////////////////////////////////////////////////GoBetwino
boolean logGestartet = false;
int runtime;
int gobetwinoIntervall = 10000; //Übertragungsintervall
int logTime = 0;
unsigned long logPreviousMillis = 0;
//
////////////////////////////////////////////////////////////////////////////Userinterface
byte hMenueMax = 4;
byte manuellMenue = 1;
byte autoMaischenMenue = 1;
boolean hMenueON = true;
boolean nMenueON = false;
byte hMenue = 1;
byte nMenue = 1;
byte meldungsNummer = 1;
byte ae[8] ={ //Buchstabe "ä"
  B01010,
  B00000,
  B01110,
  B00001,
  B01111,
  B10001,
  B01111,};
byte ue[8] ={ //Buchstabe "ü"
  B01010,
  B00000,
  B10001,
  B10001,
  B10001,
  B10011,
  B01101,};
byte grad[8] ={ //Grad Zeichen
  B00010,
  B00101,
  B00010,
  B00000,
  B00000,
  B00000,
  B00000,};
byte pfeil[8] = { //Größer Zeichen, OK
  B00000,
  B00100,
  B00010,
  B00001,
  B00010,
  B00100,
  B00000,};
byte up[8] = { //Pfeil Zeichen hoch
  B00000,
  B00000,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,};
byte down[8] = { //Pfeil Zeichen runter
  B00000,
  B00000,
  B00000,
  B10001,
  B01010,
  B00100,
  B00000,};
byte soll[8] = { //Pfeil rechts
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,};
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////Bibliotheken einbinden
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define NONE       0  //kein Button wird gedrückt
#define RIGHT      1 //RIGHT (ungenutzt)
#define HOCH       2 //UP
#define RUNTER     3 //DOWN
#define OK         4 //LEFT
#define ZURUECK    5 //SELECT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////LCD Keypad Shield///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Button eingaben
int x;
int keypad() {
  
    x = analogRead(keypadPin);
    if (x > 1000) return NONE;
    else if (x < 200) return HOCH;
    else if (x < 520) return RUNTER;
    else if (x < 700) return OK;
    else if (x < 1000) return ZURUECK;
    else return NONE;
}
//Buttoneingabe verarbeiten
void eingabe() {
  button = keypad();
  switch(button) {
    //////////////////////
    ///////////////////////
    ////////////////////////
    /////////////////////////
    case ZURUECK: {  ///////////////Zurück
      if (!buttonPressed){
        buttonPressed = true;
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////allgeimein
        if (alarmON) { /////////////////////////////////////////////schaltet alarm aus bei Meldungen, identisch mit dem code bei zurück
          alarmOFF = true;
          alarmON = false;
          if (meldungsNummer == 1) {  //bestätigt das das programm fertig ist "Programm beendet"
              autoKochenMenue = 1;
              manuellMenue = 1;
          hMenueON = true;//zurück ins Hauptmenü
          nMenueON = false;
          }
          if (meldungsNummer == 2 && einmaischenJetzt) { //bestätigt das eingemaischt wird "jetzt einmaischen"
            einmaischenJetzt = false;
            meldung = true;
          }
          if (meldungsNummer == 3) { //bestätigt das geläutert wird "jetzt läutern", das program ist dann fertig und geht zurück ins hauptmenue
            meldungsNummer = 1;
            autoMaischenMenue = 1;
            jetztLaeutern = false;
            hMenueON = true;//zurück ins Hauptmenü
            nMenueON = false;
          }
          break;        
        }
        if (meldung && automatikBestaetigung) { //bricht den start der Automatik ab
          meldung = false;
          automatikBestaetigung = false;
          //lcd.clear();
          break;
        }
        //
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung Status
        //
        //
        //
        //
        if (!hMenueON && nMenueON && hMenue == 4) {// gehtwieder ins Hauptmenü
          if (statusMenue > 1) statusMenue--;
          else {
            hMenueON = true;
            nMenueON = false;
          }
          break;
        }
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung Manuell
        //
        //
        //
        if (!hMenueON && nMenueON && hMenue == 1) {
          if (meldung) {  //deaktiviert die meldung aber bleibt im manuellen Modus
              meldung = false;
              manuellMenue = 1;
              //lcd.clear();
              break;
          }
          //
          if (!meldung && !alarmON) {   //deaktiviert diesen part wenn alarm oder meldung ausgegeben wird
            if (!hMenueON && nMenueON && hMenue == 1) { //Verstellt das manuellMenü
              if (manuellMenue > 0) {
                if (manuellStart && manuellMenue == 5) manuellMenue = 3;
                else manuellMenue--;
              }
              if (manuellStart && manuellMenue == 0) {
                manuellMenue = 1;
                meldung = true;  //warnt vor beenden des manuellen modus
              }
              if (manuellMenue == 0) {  // gehtwieder ins Hauptmenü
                manuellMenue = 1;
                hMenueON = true;
                nMenueON = false;
              }
            //lcd.clear();
            break;  
            }
          }
        }
        //
        ////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////MenüSteuerung Automaischen
        //
        //
        //
        if (!hMenueON && nMenueON && hMenue == 2){
          if (!autoMaischenStart && autoMaischenMenue > 1) {  //Verstellt das automaischmenü
            if (autoMaischenStartDruecken) autoMaischenStartDruecken = false;
            autoMaischenMenue--;
            //lcd.clear();
            break;
          }
          if(autoMaischenMenue == 1) { //zurück ins Hauptmenü
            hMenueON = true;
            nMenueON = false;
            //lcd.clear();
            break;
          }
          if (autoMaischenStart && !meldung) {  // warnt vor Beenden der Automatik
            meldung = true;
            //lcd.clear();
            break;
          }
          if (autoMaischenStart && meldung && meldungsNummer != 2) {  //schließt die meldung ohne Automaik zu beenden
            meldung = false;
            //lcd.clear();
            break;
          }
        }
       //
        ////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////MenüSteuerung AutoKochen
        //
        //
        //
        if (!hMenueON && nMenueON && hMenue == 3){
          if (!autoKochenStart && autoKochenMenue > 1) {  //Verstellt das automaischmenü
            if (autoKochenStartDruecken) autoKochenStartDruecken = false;
            autoKochenMenue--;
            //lcd.clear();
            break;
          }
          if(autoKochenMenue == 1) { //zurück ins Hauptmenü
            hMenueON = true;
            nMenueON = false;
            //lcd.clear();
            break;
          }
          if (autoKochenStart && !meldung) {  // warnt vor Beenden der Automatik
            meldung = true;
            //lcd.clear();
            break;
          }
          if (autoKochenStart && meldung) {  //schließt die meldung ohne Automaik zu beenden
            meldung = false;
            //lcd.clear();
            break;
          }
        }
      }
      break;
    }
    //////////////////////
    ///////////////////////
    ////////////////////////
    /////////////////////////
    case OK: {      ///////////////OK
      if (!buttonPressed){
        buttonPressed = true;
        ////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////allg. Menüsteuerung
        //
        //
        //
        if (meldung) { //schließt meldungen die mit Ok bestätigt werden müssen
          if ((autoMaischenStart || manuellStart || autoKochenStart) && meldungsNummer != 2 && meldungsNummer != 3) { //bricht laufenden Modus ab und geht ins Hauptmenü, schaltet alles wieder auf anfang
            if (autoMaischenStart) autoMaischenEnde = true;                                                //"zum beenden ok drücken"
              autoMaischenMenue = 1;
            if (autoKochenStart) autoKochenEnde = true;
              autoKochenMenue = 1;
            if (manuellStart) manuellEnde = true;
              manuellMenue = 1;
              timerEnde = false;
              timerStart = false;
            meldungsNummer = 1;
            hMenueON = true; //geht zurück ins Hauptmenue
            nMenueON = false;
            //lcd.clear();
            break;
          }
          if (automatikBestaetigung) { //übersicht vor automatik start, startet bei ok automatik
            meldung = false;
            automatikBestaetigung = false;
            if (hMenue == 2) {
              ruehrerStart = true;
              autoMaischenStart = true;
            }
            if (hMenue == 3) autoKochenStart = true;
            //lcd.clear();
            break;
          }
          if (!einmaischenFertig) { //bestätigung das man mit dem einmaischen fertig ist
            ruehrerStart = true;
            meldung = false;
            einmaischenFertig = true;
            tempSollErreicht = true;
            meldungsNummer = 1;
            datenUebergeben = false;
            einmaischenPause = false;
            //lcd.clear();
            break;
          }
        }
        if (alarmON) { /////////////////////////////////////////////schaltet alarm aus bei Meldungen, identisch mit dem code bei zurück
          alarmOFF = true;
          alarmON = false;
          if (meldungsNummer == 1) {  //bestätigt das das programm fertig ist "Programm beendet"
              autoKochenMenue = 1;
              manuellMenue = 1;
          hMenueON = true;//zurück ins Hauptmenü
          nMenueON = false;
          }
          if (meldungsNummer == 2 && einmaischenJetzt) { //bestätigt das eingemaischt wird "jetzt einmaischen"
            einmaischenJetzt = false;
            meldung = true;
          }
          if (meldungsNummer == 3) { //bestätigt das geläutert wird "jetzt läutern", das program ist dann fertig und geht zurück ins hauptmenue
            autoMaischenMenue = 1;
            jetztLaeutern = false;
            meldungsNummer = 1;
            hMenueON = true;//zurück ins Hauptmenü
            nMenueON = false;
          }
          break;          
        }
        if (!meldung && !alarmON) { //deaktiviert diesen part wenn alarm oder meldung ausgegeben wird
          if (hMenueON && !nMenueON && !autoMaischenStart && !manuellStart && !autoKochenStart) {  //geht vom Hauptmenü ins Nebenmenü
            hMenueON = false;
            nMenueON = true;
            //lcd.clear();
            break;
          }
              //////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung Status
            //
            //
            //
            //
            if (!hMenueON && nMenueON && hMenue == 4) {// geht durchs Menü
              if (statusMenue < 4 ) statusMenue++;
              break;
            }
          ///////////////////////////////////////////////////////
          ////////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung Manuell
          //
          //
          //
            if (!hMenueON && nMenueON && hMenue == 1) {  //Verstellt das manuellmenü
              if (manuellMenue < 4) {
                if (manuellStart && manuellMenue == 3) manuellMenue = manuellMenue + 2;
                else manuellMenue++;
                break;
              }
            }
            if (!hMenueON && nMenueON && hMenue == 1 && manuellMenue == 4 && !manuellEnde) { //Startet den modus Manuell
              manuellStart = true;
              manuellMenue++;
              //lcd.clear();
              break;
            }
          ///////////////////////////////////////////////////
          //////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung AutoMaischen
          //
          //
          //
          if (!manuellStart && !autoKochenStart) {
            if (!hMenueON && nMenueON && hMenue == 2) {  //Verstellt den rührer während der automaischen an ist
              if (autoMaischenStart && autoMaischenStatus == 3) {
                if (ruehrerStufe < 2) ruehrerStufe++;
                else ruehrerStufe = 0;
                ruehrerStufe1Gewartet = false;
                //lcd.clear();
                break;
              }
              
              if (!autoMaischenStartDruecken && autoMaischenMenue < 19 && !autoMaischenStart)   autoMaischenMenue++;  //überprüft ob Automaischen gestartet werden soll
              else if (!autoMaischenStart) {
                automatikBestaetigung = true;
                autoMaischenStartDruecken = false;
                maximaMaischen = true;
                meldung = true;
                }
              //lcd.clear();
              break;
            }
          }
          ///////////////////////////////////////////////////
          //////////////////////////////////////////////////////////////////////////////////////////////////Menüsteuerung AutoKochen
          //
          //
          //
          if (!manuellStart && !autoMaischenStart) {
            if (!hMenueON && nMenueON && hMenue == 3) {  //Verstellt den rührer während der automaischen an ist
              if (autoKochenStart && autoKochenStatus == 4) {
                if (ruehrerStufe < 2) ruehrerStufe++;
                else ruehrerStufe = 0;
                ruehrerStufe1Gewartet = false;
                //lcd.clear();
                break;
              }
              
              if (!autoKochenStartDruecken && autoKochenMenue < 11 && !autoKochenStart)   autoKochenMenue++;  //überprüft ob Automaischen gestartet werden soll
              else if (!autoKochenStart) {
                automatikBestaetigung = true;
                autoKochenStartDruecken = false;
                maximaKochen = true;
                meldung = true;
                }
              //lcd.clear();
              break;
            }
          }
          
        }
        //lcd.clear();
        break;
      }
      break;
    }
    //////////////////////
    ///////////////////////
    
    ////////////////////////
    /////////////////////////
    case HOCH: {  ////////////////Hoch bzw +
      
      //if (!buttonPressed){
        //buttonPressed = true;
        if (!meldung && !alarmON) { //deaktiviert diesen part wenn alarm oder meldung ausgegeben wird
          ////////////////////////////////////////////////
          ////////////////////////////////////////////////////////////////////////////////////////////////////Verstellt das Hauptmenü
          //
          //
          //
          if (!buttonPressed){//
          if (hMenueON && !nMenueON) {
            if (hMenue > 1) hMenue--;
            else hMenue = hMenueMax;
            buttonPressed = true;//
          }
          }
          ////////////////////////////////////////////////
          ///////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Moudus Manuell
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 1) {
            if (manuellMenue == 2) { //Verstellt die Temperatur für Manuell+
              if (manuellTemp < tempKochenMax) {
                if (millis() - zaehlerStart >= zaehlerTakt) {
                  if (!buttonPressed) zaehlerStart = millis();
                  manuellTemp++;//
                }
              }
              else manuellTemp = tempMin;
            }
            if (manuellMenue == 3) { //Verstellt den Timer für Manuell+
              if (timerManuell < timerMax) {
                if (millis() - zaehlerStart >= zaehlerTakt){
                  if (!buttonPressed) zaehlerStart = millis();
                  timerManuell++;//
                }
              }
              else timerManuell = timerMin;
              datenUebergeben = false;
            }
            if (manuellMenue == 1) { //Verstellt die Rührerstufe+
              if (!buttonPressed) {//
              if (ruehrerStufe < 2) ruehrerStufe++;
              else ruehrerStufe = 0;
              ruehrerStufe1Gewartet = false;
              buttonPressed = true;//
            }
            
            }
          }
          ////////////////////////////////////////////////
          ///////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Modus AutoMaischen hoch
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 2) {  //definition für AutoMaischen modus
            if (autoMaischenStart) {
              if (!buttonPressed) {
              if (autoMaischenStatus > 1) autoMaischenStatus--;
              else autoMaischenStatus = 3;
              buttonPressed = true;
              }
            }
            if (autoMaischenStartDruecken) {
              if (!buttonPressed) {
                autoMaischenStartDruecken = false;
                buttonPressed = true;
            }
            }
            else if (!autoMaischenStart){
              switch(autoMaischenMenue) {
                case 1: { //Verstellt die Rührerstufe+
                  if (!buttonPressed) {
                  if (ruehrerStufe < 2) ruehrerStufe++;
                  else ruehrerStufe = 0;
                  ruehrerStufe1Gewartet = false;
                  buttonPressed = true;//
                  }
                  break;
                }
                case 2: {  //hoch von tempEinmaischen
                  if (tempEinmaischen < tempMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempEinmaischen++;
                    }
                  }
                  else tempEinmaischen = tempMin;
                  break;
                }
                case 3: {  //hoch von tempStep////////////////////////////////////Step1
                  if (tempStep1 < tempMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep1++;
                    }
                  }
                  else tempStep1 = tempMin;
                  break;
                }
                case 4: { // hoch von timerstep
                  if (timerStep1 < timerMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep1++;
                    }
                  }
                  else timerStep1 = timerMin;
                  break;
                }
                case 5: {  //hoch von tempStep///////////////////////////////////Step2
                  if (tempStep2 < tempMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt){
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep2++;
                    }
                  }
                  else tempStep2 = tempMin;
                  break;
                }
                case 6: { // hoch von timerstep
                  if (timerStep2 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep2++;
                    }
                  }
                  else timerStep2 = timerMin;
                  break;
                }
                case 7: { //wechselt auf start
                  if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 8: {  //hoch von tempStep///////////////////////////////Step3
                  if (tempStep3 < tempMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep3++;
                    }
                  }
                  else tempStep3 = tempMin;
                  break;
                }
                case 9: { // hoch von timerstep
                  if (timerStep3 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep3++;
                    }
                  }
                  else timerStep3 = timerMin;
                  break;
                }
                case 10: { //wechselt auf start
                  if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 11: {  //hoch von tempStep///////////////////////////Step4
                  if (tempStep4 < tempMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep4++;
                    }
                  }
                  else tempStep4 = tempMin;
                  break;
                }
                case 12: { // hoch von timerstep
                  if (timerStep4 < timerMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt){
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep4++;
                    }
                  }
                  else timerStep4 = timerMin;
                  break;
                }
                case 13: { //wechselt auf start
                  if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 14: {  //hoch von tempStep//////////////////////////////////Step5
                  if (tempStep5 < tempMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep5++;
                    }
                  }
                  else tempStep5 = tempMin;
                  break;
                }
                case 15: { // hoch von timerstep
                  if (timerStep5 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep5++;
                    }
                  }
                  else timerStep5 = timerMin;
                  break;
                }
                case 16: { //wechselt auf start
                  if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 17: {  //hoch von tempStep//////////////////////////////////Step6
                  if (tempStep6 < tempMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep6++;
                    }
                  }
                  else tempStep6 = tempMin;
                  break;
                }
                case 18: { // hoch von timerstep
                  if (timerStep6 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep6++;
                    }
                  }
                  else timerStep6 = timerMin;
                  break;
                }//bei case 19 gibts nichts zu verstellen
              }
            }
          }
          ////////////////////////////////////////////////
          ///////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Modus AutoKochen hoch
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 3) {  //definition für AutoKochen modus
            if (autoKochenStart) {
              if (!buttonPressed) {
              if (autoKochenStatus > 1) autoKochenStatus--;
              else autoKochenStatus = 4;
              buttonPressed = true;
              }
            }
            if (autoKochenStartDruecken) {
              if (!buttonPressed) {
              autoKochenStartDruecken = false;
              buttonPressed = true;
              }
            }
            else if (!autoKochenStart){  
              switch(autoKochenMenue) {
                case 1: { //Verstellt die Rührerstufe+
                  if (!buttonPressed) {//
                  if (ruehrerStufe < 2) ruehrerStufe++;
                  else ruehrerStufe = 0;
                  ruehrerStufe1Gewartet = false;
                  buttonPressed = true;//
                  }
                  break;
                }
                case 2: {  //hoch von tempKochen////////////////////////////////////kochtemperatur
                  if (tempKochen < tempKochenMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempKochen++;
                    }
                  }
                  else tempKochen = tempKochenMin;
                  break;
                }
                case 3: { // hoch von kochdauer
                  if (timerKochen < timerMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerKochen++;
                    }
                  }
                  else timerKochen = timerKochenMin;
                  break;
                }
                case 4: { // hoch von timer hopfen1
                  if (timerHopfen1 < timerMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen1++;
                    }
                  }
                  else timerHopfen1 = timerMin;
                  break;
                }
                case 5: { //wechselt auf start
                  if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 6: { // hoch von timer hopfen2
                  if (timerHopfen2 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen2++;
                    }
                  }
                  else timerHopfen2 = timerMin;
                  break;
                }
                case 7: { //wechselt auf start
                  if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 8: { // hoch von timerhopfen3
                  if (timerHopfen3 < timerMax){
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen3++;
                    }
                  }
                  else timerHopfen3 = timerMin;
                  break;
                }
                case 9: { //wechselt auf start
                  if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                  break;
                }
                case 10: { // hoch von timerhopfen4
                  if (timerHopfen4 < timerMax) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen4++;
                    }
                  }
                  else timerHopfen4 = timerMin;
                  break;
                }//bei case 11 gibts nichts zu verstellen
              }
            //}
          }
        }
      }
      break;
    }
    //////////////////////
    ///////////////////////
    ////////////////////////
    /////////////////////////
    case RUNTER: {  ////////////////////////////////Runter bzw -
      //if (!buttonPressed){
        //buttonPressed = true;
        if (!meldung && !alarmON) { //deaktiviert diesen part wenn alarm oder meldung ausgegeben wird
          ///////////////////////////////////////////////////
          /////////////////////////////////////////////////////////////////////////////////////////////////////////allg Menüsteuerung
          //
          //
          //
          if (!buttonPressed){
          if (hMenueON && !nMenueON) {//Verstellt das Hauptmenü
            if (hMenue < hMenueMax) hMenue++;
            else hMenue = 1;
            buttonPressed = true;
          }
          }
          //////////////////////////////////////////////////////
          ///////////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Modus Manuell
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 1) {  //Verstellt die Temperatur für Manuel-
            if (manuellMenue == 2){
            if (manuellTemp > tempMin) {
              if (millis() - zaehlerStart >= zaehlerTakt) {
                if (!buttonPressed) zaehlerStart = millis();
                manuellTemp--;
              }
            }
            else manuellTemp = tempKochenMax;
          }
            if (manuellMenue == 3){ //Verstellt den Timer für Manuell-
            if (timerManuell > timerMin) {
              if (millis() - zaehlerStart >= zaehlerTakt){
                  if (!buttonPressed) zaehlerStart = millis();
                  timerManuell--;
              }
            }
            else timerManuell = timerMax;
            datenUebergeben = false;
          }
          if (manuellMenue == 1) { //Verstellt die Rührerstufe-
            if (!buttonPressed) {//
            if (ruehrerStufe > 0) ruehrerStufe--; 
            else ruehrerStufe = 2;
            buttonPressed = true;//
          }
        }
          }
          ////////////////////////////////////////////////////
          //////////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Modus Automaischen
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 2) {  //definition für AutoMaischen modus
            if (autoMaischenStart) {
              if (!buttonPressed) {
              if (autoMaischenStatus < 3) autoMaischenStatus++;
              else autoMaischenStatus = 0;
              buttonPressed = true;
              }
            }
            if (autoMaischenStartDruecken) {
              if (!buttonPressed) {
              autoMaischenStartDruecken = false;
              buttonPressed = true;
              }
            }
              else if (!autoMaischenStart) {
                switch(autoMaischenMenue) {
                  case 1: { //Verstellt die Rührerstufe+
                  if (!buttonPressed) {//
                  if (ruehrerStufe > 0) ruehrerStufe--;
                  else ruehrerStufe = 2;
                  ruehrerStufe1Gewartet = false;
                  buttonPressed = true;//
                  }
                  break;
                }
                case 2: {  // von tempEinmaischen
                  if (tempEinmaischen > tempMin) {
                    if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempEinmaischen--;
                    }
                  }
                  else tempEinmaischen = tempMax;
                  break;
                }
                  case 3: {  // von tempStep////////////////////////////////////Step1
                    if (tempStep1 > tempMin){
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep1--;
                      }
                    }
                    else tempStep1 = tempMax;
                    break;
                  }
                  case 4: { // von timerstep
                    if (timerStep1 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep1--;
                      }
                    }
                    else timerStep1 = timerMax;
                    break;
                  }
                  case 5: {  // von tempStep////////////////////////////////////Step2
                    if (tempStep2 > tempMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep2--;
                      }
                    }
                    else tempStep2 = tempMax;
                    break;
                  }
                  case 6: { // von timerstep
                    if (timerStep2 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep2--;
                      }
                    }
                    else timerStep2 = timerMax;
                    break;
                  }
                  case 7: { //wechselt auf start
                    if (!buttonPressed) {
                      autoMaischenStartDruecken = true;
                      buttonPressed = true;
                      }
                    break;
                  }
                  case 8: {  // von tempStep////////////////////////////////////Step3
                    if (tempStep3 > tempMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep3--;
                      }
                    }
                    else tempStep3 = tempMax;
                    break;
                  }
                  case 9: { // von timerstep
                    if (timerStep3 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep3--;
                      }
                    }
                    else timerStep3 = timerMax;
                    break;
                  }
                  case 10: { //wechselt auf start
                    if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 11: {  // von tempStep////////////////////////////////////Step4
                    if (tempStep4 > tempMin){
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep4--;
                      }
                    }
                    else tempStep4 = tempMax;
                    break;
                  }
                  case 12: { //  von timerstep
                    if (timerStep4 > timerMin){
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep4--;
                      }
                    }
                    else timerStep4 = timerMax;
                    break;
                  }
                  case 13: { //wechselt auf start
                   if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 14: {  //von tempStep////////////////////////////////////Step5
                    if (tempStep5 > tempMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep5--;
                      }
                    }
                    else tempStep5 = tempMax;
                    break;
                  }
                  case 15: { // von timerstep
                    if (timerStep5 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep5--;
                      }
                    }
                    else timerStep5 = timerMax;
                    break;
                  }
                  case 16: { //wechselt auf start
                    if (!buttonPressed) {
                  autoMaischenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 17: {  //von tempStep////////////////////////////////////Step6
                    if (tempStep6 > tempMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempStep6--;
                      }
                    }
                    else tempStep6 = tempMax;
                    break;
                  }
                  case 18: { //  von timerstep
                    if (timerStep6 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerStep6--;
                      }
                    }
                    else timerStep6 = timerMax;
                    break;
                  }//in case 19 gibt es nichts zu verstellen
                }
              }
            
          }
          ////////////////////////////////////////////////////
          //////////////////////////////////////////////////////////////////////////////////////////verstellt Variablen im Modus AutoKochen
          //
          //
          //
          if (!hMenueON && nMenueON && hMenue == 3) {  //definition für AutoMaischen modus
            if (autoKochenStart) {
              if (!buttonPressed) {
              if (autoKochenStatus < 4) autoKochenStatus++;
              else autoKochenStatus = 0;
              buttonPressed = true;
              }
            }
            if (autoKochenStartDruecken) {
              if (!buttonPressed) {
              autoKochenStartDruecken = false;
              buttonPressed = true;
              }
            }
              else if (!autoKochenStart) {
                switch(autoKochenMenue) {
                  case 1: { //Verstellt die Rührerstufe+
                  if (!buttonPressed) {//
                  if (ruehrerStufe >0) ruehrerStufe--;
                  else ruehrerStufe = 2;
                  ruehrerStufe1Gewartet = false;
                  buttonPressed = true;//
                  }
                }
                  case 2: {  //hoch von tempKochen
                    if (tempKochen > tempKochenMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      tempKochen--;
                      }
                    }
                    else tempKochen = tempKochenMax;
                    break;
                  }
                  case 3: { // hoch von timerstep
                    if (timerKochen > timerKochenMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerKochen--;
                      }
                    }
                    else timerKochen = timerMax;
                    break;
                  }
                  case 4: { // hoch von timer
                    if (timerHopfen1 > timerMin){
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen1--;
                      }
                    }
                    else timerHopfen1 = timerMax;
                    break;
                  }
                  case 5: { //wechselt auf start
                    if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 6: { // hoch von timerstep
                    if (timerHopfen2 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen2--;
                      }
                    }
                    else timerHopfen2 = timerMax;
                    break;
                  }
                  case 7: { //wechselt auf start
                    if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 8: { // hoch von timerstep
                    if (timerHopfen3 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen3--;
                      }
                    }
                    else timerHopfen3 = timerMax;
                    break;
                  }
                  case 9: { //wechselt auf start
                    if (!buttonPressed) {
                  autoKochenStartDruecken = true;
                  buttonPressed = true;
                  }
                    break;
                  }
                  case 10: { // hoch von timerstep
                    if (timerHopfen4 > timerMin) {
                      if (millis() - zaehlerStart >= zaehlerTakt) {
                      if (!buttonPressed) zaehlerStart = millis();
                      timerHopfen4--;
                      }
                    }
                    else timerHopfen4 = timerMax;
                    break;
                  } //in case 11 gibt es nichts zu verstellen
                }
              
            
          }
        }
      }
      break;
    }
    delay(10);
    case NONE: {
      buttonPressed = false;
      break;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////Temperaturmessungen///////////////////////////////////////////////////////////
/////https://learn.adafruit.com/thermistor/using-a-thermistor///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float tempT() { //Temperatur am Topfboden
  byte i;
  // take N samples in a row, with a slight delay
  if (millis() - sensorPreviousTime >= 10) {
    for (i=0; i< 10; i++) {
     samplesT[i] = analogRead(tempTPin);
    }
  sensorPreviousTime = millis();
  }
  // average all the samples out
  float iaverage = 0;
  for (i=0; i< 10; i++) {
     iaverage += samplesT[i];
  }
  iaverage /= 10; 
  // convert the value to resistance
  iaverage = 1023.0 / iaverage - 1;
  iaverage = 9560.0 / iaverage; //Referenzwiderstand
  float isteinhart;  //Berechnet Widerstand in Temperatur um
  isteinhart = iaverage / 10000.0;     // (R/Ro)
  isteinhart = log(isteinhart);                  // ln(R/Ro)
  isteinhart /= 3435;                   // 1/B * ln(R/Ro) //3435 ist der BWert des NTC 
  isteinhart += 1.0 / (25 + 273.15); // + (1/To)
  isteinhart = 1.0 / isteinhart;                 // Invert
  isteinhart -= 273.15;                         // convert to C
  isteinhart *= 10.0;
  isteinhart = round(isteinhart); // rundet auf eine nachkommastelle
  isteinhart /= 10.0;  
  return isteinhart;
}
float tempI() { //Temperatur im Innenraum
  uint8_t i;
  float iaverage;
  // take N samples in a row, with a slight delay
  if (millis() - sensorPreviousTime >= 10) {
    for (i=0; i< 5; i++) {
     samplesI[i] = analogRead(tempIPin);
    }
  sensorPreviousTime = millis();
  }
  // iaverage all the samples out
  iaverage = 0;
  for (i=0; i< 5; i++) {
     iaverage += samplesI[i];
  }
  iaverage /= 5; 
  // convert the value to resistance
  iaverage = 1023.0 / iaverage - 1;
  iaverage = 9560.0 / iaverage; 
  float isteinhart;  //Berechnet Widerstand in Temperatur um
  isteinhart = iaverage / 10000.0;     // (R/Ro)
  isteinhart = log(isteinhart);                  // ln(R/Ro)
  isteinhart /= 3435;                   // 1/B * ln(R/Ro) //3435 ist der BWert des NTC 
  isteinhart += 1.0 / (25 + 273.15); // + (1/To)
  isteinhart = 1.0 / isteinhart;                 // Invert
  isteinhart -= 273.15;                         // convert to C
  return isteinhart;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////Heizungsregelung//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean heizung() {
float igradient; 
  if (autoMaischenStart || autoKochenStart || manuellStart) {
      if (millis() - heizungVorherigeZeit >= heizungMesszyklus) {  //berechnet Gradienten in K/min
              igradient = ((tempT() - heizungVorherigeTemp) / (millis() - heizungVorherigeZeit) * 60000.0); //iGradient in K/min
              if (igradient < 0) igradient = 0.1;
              heizungVorherigeZeit = millis();
              heizungVorherigeTemp = tempT();
      }
      if ((tempT() > (tempSoll - 0.5)) || (tempT() >= 99.5)) {
        tempSensorDaempfung++;
        if (tempSensorDaempfung > 20) tempSollErreicht = true;
      }
      if (tempSoll >= 100) return true;
      if (millis() - heizungWarten >= heizungESV){  //schaltet heizung ein
          if (tempT() <= (tempSoll  - (igradient * k))) return true;
      }
      if (tempT() > (tempSoll + (igradient * k))) {  //schaltet die Heizung aus wenn temp überschwingt
            heizungWarten = millis();
            return false;
      }
  }
  else return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////Rührersteuerung///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ruehrerKILL() { //schaltet alle Variablen wieder auf anfang beim ausschalten des Rührers
  if (ruehrerOFF) {
    ruehrerOFF = false;
    ruehrerStufe = 1;
    ruehrerStufe1Gewartet = false;
  }
}
boolean ruehrerStufe1(){ //schaltet rührer stufe 1 ein
  ruehrerKILL();
  if (ruehrerStart){
    if (ruehrerStufe == 1 || ruehrerPauseGestartet) return true;
    else return false;
  }
  else return false;
}
boolean ruehrerStufe2() { //schaltet rührer stufe 2 ein, benötigt stufe 1 um zu starten
  ruehrerKILL();
  if (ruehrerStart) {
    if (ruehrerStufe == 2){
      if (!ruehrerStufe1Gewartet) {
        if (!ruehrerPauseGestartet) {
          ruehrerPreviousMillis = millis();
          ruehrerPauseGestartet = true;
        }
        if (millis() - ruehrerPreviousMillis >= ruehrerPause && ruehrerStufe1) {
          ruehrerStufe1Gewartet = true;
        }
      }
      if (ruehrerStufe1Gewartet){
        ruehrerPauseGestartet = false;
        return true;
      }
    }
    else return false;
  }
  else return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////Buzzer Takt///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alarmKILL(){
  if (alarmOFF) {
    alarmOFF = false;
    alarmON = false;
  }
}
boolean buzzer() {
  alarmKILL();
  if (millis() - buzzerPreviousMillis >= buzzerIntervall) {
    buzzerPreviousMillis = millis();
    if (buzzerON) buzzerON = false;
      else buzzerON = true;
  }
  return buzzerON;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////LED Takt/////////////////////////////////////////////////////////////////////
/////////////////////////////////////für programmLed/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean ledTakt() {
  if (millis() - ledPreviousMillis >= ledIntervall)  {
    ledPreviousMillis = millis();
    if (ledON) ledON = false;
      else ledON = true;
  }
   return ledON; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////LED und buzzer Test/////////////////////////////////////////////////////////////////////
/////////////////////////////////////über StatusMenü/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Einstellungen
boolean ledTest() {
  if (hMenue == 4 && nMenueON && !hMenueON && statusMenue == 4) return true;
  else return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////Timer/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timer(){
  timerMinuten = timerSet/60;
  if (timerStart && tempSollErreicht && !timerEnde) { //Verstellt den Timer wenn Automatik 
    unsigned long icurrentTimerMillis = millis();
    if (icurrentTimerMillis - timerPreviousMillis >= 2000) timerPreviousMillis = icurrentTimerMillis;
    if (icurrentTimerMillis - timerPreviousMillis >= 1000) {
       timerSet--;
       timerMinuten = timerSet/60;  //Minutenanzeige für den Status des Modus
       timerSekunden = timerSet - timerMinuten*60;   //Sekundenanzeige für den Status des Modus
       timerPreviousMillis = icurrentTimerMillis;
       if (manuellStart) timerManuell = timerMinuten;
      }
  }
  if (timerSet == 0){
    timerEnde = true;
    timerSet = timerMin;
  }
  if (timerEnde){  //alles wird auf anfang gesetzt
    tempSollErreicht = false;
    tempSensorDaempfung = 0;
    timerStart = false;
    timerSekunden = 0;  //für die anzeige
    timerManuell = timerMin;
    timerSet = timerMin;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////Manueller Modus/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void manuell() {
  if (manuellStart && !autoMaischenStart && !autoKochenStart) {
      if (!datenUebergeben) {
      timerSet = timerManuell*60;
      datenUebergeben = true;
      }
      heizungOFF = false;
      ruehrerStart = true;
      timerStart = true;
      tempSoll = manuellTemp;
      if (timerEnde){
        alarmON = true;
        meldungsNummer = 1;
        timerEnde = false;
        manuellEnde = true;
      }
      if (manuellEnde) {  //schaltet alles wieder auf Anfang
        ruehrerOFF = true;
        ruehrerStart = false;
        logTime = 0;
        logGestartet = false;
        manuellStart = false;
        manuellEnde = false;
        meldung = false;
        datenUebergeben = false;
        autoKochenEnde = false;
        //neu von hier bis timerStart
        tempSollErreicht = false;
        tempSensorDaempfung = 0;
        timerStart = false;
        timerSekunden = 0;  //für die anzeige
        timerSet = timerMin;
        timerStart = false;
      }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////AutoMaischen Modus////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autoMaischen() {
  if (maximaMaischen) { //Bestimmt Rahmenwerte für die Automatik
    if (autoMaischenMenue == 7) letzterStep = 2;  //bestimmt letzten Step anhand der menüposition
    else if (autoMaischenMenue == 10) letzterStep = 3;
    else if (autoMaischenMenue == 13) letzterStep = 4;
    else if (autoMaischenMenue == 16) letzterStep = 5;
    else if (autoMaischenMenue == 19) letzterStep = 6;
    tempStepMax = tempStep1;
    timerStepMax = timerStep1;
    if (letzterStep == 2) { //Bestimmt den letzten Schritt der Automatik
      tempStepMax = tempStep2;  //für die zusammefassung vor dem automatikstart
      timerStepMax = timerStep1 + timerStep2;  //same
    }
    if (letzterStep == 3) {
      tempStepMax = tempStep3;
      timerStepMax = timerStep1 + timerStep2 + timerStep3;
    }
    if (letzterStep == 4) {
      tempStepMax = tempStep4;
      timerStepMax = timerStep1 + timerStep2 + timerStep3 + timerStep4;
    }
    if (letzterStep == 5) {
      tempStepMax = tempStep5;
      timerStepMax = timerStep1 + timerStep2 + timerStep3 + timerStep4 + timerStep5;
    }
    if (letzterStep == 6) {
      tempStepMax = tempStep6;
      timerStepMax = timerStep1 + timerStep2 + timerStep3 + timerStep4 + timerStep5 + timerStep6;
    }    
    maximaMaischen = false;  //bestimmt die Werte nur einmal
  }
  if (autoMaischenStart && !manuellStart && !autoKochenStart) {  //hier startet es erst richtig
      //////////////////////////////////////////////////Timen des Temperaturprofils
       heizungOFF = false;  //schaltet zum start alles an
        if (!einmaischenFertig) {
          if (!datenUebergeben) {
            tempSoll = tempEinmaischen;
            timerSet = timerStep1*60;
            datenUebergeben = true;
          }
          if (tempSollErreicht && !einmaischenPause) {
                alarmON = true;  //alarm zum einmaischen
                meldungsNummer = 2;
                einmaischenJetzt = true;  //pausiert die ganze automatik bis eingemaischt wurde, aka es bestätigt wurde
                ruehrerStart = false;  //rührer ausschalten zum Einmaischen, geheizt wird weiter
                tempSollErreicht = false;
                einmaischenPause = true;
           }
        }
       if (einmaischenFertig) {
        timerStart = true;
        switch(currentStep) {
          case 1: {  //automaisch schritt 1
            if (!datenUebergeben) {
              tempSoll = tempStep1;
              datenUebergeben = true;
            }
            break;
          }
          case 2: { //automaisch schritt 2
            if (!datenUebergeben) {  
              timerSet = timerStep2*60;
              tempSoll = tempStep2;
              datenUebergeben = true;
            }
            break;
          }
          case 3: {//automaisch schritt 3
            if (!datenUebergeben) {  
              timerSet = timerStep3*60;
              tempSoll = tempStep3;
              datenUebergeben = true;
            }
            break;
          }
          case 4: {//automaisch schritt 4
            if (!datenUebergeben) {  
              timerSet = timerStep4*60;
              tempSoll = tempStep4;
              datenUebergeben = true;
            }
            break;
          }
          case 5: {//automaisch schritt 5
            if (!datenUebergeben) {  
              timerSet = timerStep5*60;
              tempSoll = tempStep5;
              datenUebergeben = true;
            }
            break;
          }
          case 6: {//automaisch schritt 6
            if (!datenUebergeben) {  
              timerSet = timerStep6*60;
              tempSoll = tempStep6;
              datenUebergeben = true;
            }
            break;
          }
        }
        if (timerEnde) {  ///////////weiter zu nächsten Step
            timerEnde = false;
            if (currentStep < letzterStep) {
              currentStep++;
              datenUebergeben = false;
            }
            else autoMaischenEnde = true;
          }
        }
      }
    if (autoMaischenEnde){  //Schaltet alles auf Anfang wenn die Automatik vorbei ist, beendet sie und aktiviert den läuteralarm
      autoMaischenStatus = 1; //für Statusanzeige auf display
      autoMaischenStartDruecken = false;
      autoMaischenStart = false;
      autoMaischenEnde = false;
      if (!meldung) {
        jetztLaeutern = true;  //alarm zum läutern
        alarmON = true;
        meldungsNummer = 3;
      }
      logGestartet = false;  //beendet log aufzeichnung
      logTime = 0;
      einmaischenJetzt = false; //wieder auf anfang
      einmaischenFertig = false;
      currentStep = 1;
      letzterStep = 1;
      tempStepMax = 0;
      timerStepMax = 0;
      timerSet = timerMin;
      maximaMaischen = false;
      heizungOFF = true;  //schaltet heizung usw. aus
      ruehrerOFF = true;  
      ruehrerStart = false;
      timerEnde = false;
      meldung = false;
      datenUebergeben = false;
    }
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////Hopfen Timer/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hopfentimer(){
  timerHopfenMinuten = timerHopfenSet/60;
  if (timerHopfenStart && tempSollErreicht && !timerHopfenEnde) { //Verstellt den Timer wenn Automatik 
    unsigned long icurrentHopfenTimerMillis = millis();
    if (icurrentHopfenTimerMillis - timerHopfenPreviousMillis >= 2000) timerPreviousMillis = icurrentHopfenTimerMillis;
    if (icurrentHopfenTimerMillis - timerHopfenPreviousMillis >= (1000 - runtime/1000)) {
       timerHopfenSet--;
       timerHopfenMinuten = timerHopfenSet/60;  //Minutenanzeige für den Status des Modus
       timerHopfenSekunden = timerHopfenSet - timerHopfenMinuten*60;   //Sekundenanzeige für den Status des Modus
       timerHopfenPreviousMillis = icurrentHopfenTimerMillis;
      }
  }
  if (timerHopfenSet == 0){
    timerHopfenEnde = true;
    timerHopfenSet = 60;
  }
  if (timerHopfenEnde){  //alles wird auf anfang gesetzt
    timerHopfenStart = false;
    timerHopfenSekunden = 0;  //für die anzeige
    timerHopfenSet = 60;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////AutoKochen Modus////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autoKochen() {
  if (maximaKochen) {  //bestimmt Rahmenbedingugen
    if (autoKochenMenue == 5) letzterHopfen = 1;  //bestimmt letzten hopfen anhand der menüposition
    else if (autoKochenMenue == 7) letzterHopfen = 2;
    else if (autoKochenMenue == 9) letzterHopfen = 3;
    else if (autoKochenMenue == 11) letzterHopfen = 4;
    hopfenAlarm1 = timerKochen - timerHopfen1;  //berechnet hopfenalarme aka. die abstände zwischen alarmen
    hopfenAlarm2 = timerKochen - timerHopfen2 - hopfenAlarm1;
    hopfenAlarm3 = timerKochen - timerHopfen3 - hopfenAlarm2;
    hopfenAlarm4 = timerKochen - timerHopfen4 - hopfenAlarm3;
    timerSet = timerKochen * 60; //übergibt den timer für die gesamtzeit genau einmal
    maximaKochen = false;
  }
  if (autoKochenStart && !manuellStart && !autoMaischenStart) {  //programmanfang
      
        timerStart = true;  
        timerHopfenStart = true;
      hopfentimer(); //startet Timer für die einzellnen Hopfen
      heizungOFF = false; //schaltet alles an
      if (!tempSollErreicht) ruehrerStart = true;
      else {
        ruehrerOFF = true;
        ruehrerStart = false;
      }
      tempSoll = tempKochen; //neue temp für heizung
      if (hopfen <= letzterHopfen && !keinHopfenMehr) {
        switch(hopfen) {
            case 1: {
              if (!datenUebergeben) {
                timerHopfenSet = hopfenAlarm1 * 60;
                datenUebergeben = true;
              }
              break;
            }
            case 2: {  
              if (!datenUebergeben) {
                timerHopfenSet = hopfenAlarm2*60;
                datenUebergeben = true;
              }
              break;
            }
            case 3: {  //Einmaischen (automaischen schritt 1
              if (!datenUebergeben) {
                timerHopfenSet = hopfenAlarm3*60;
                datenUebergeben = true;
              }
              break;
            }
            case 4: {  //Einmaischen (automaischen schritt 1
              if (!datenUebergeben) {
                timerHopfenSet = hopfenAlarm4*60;
                datenUebergeben = true;
              }
              break;
            }
          }
          if (timerHopfenEnde) {  ///////////Hopfengabe Alarm
                if (hopfen == letzterHopfen) keinHopfenMehr = true;
                timerHopfenEnde = false;
                datenUebergeben = false;
                alarmON = true;
                meldungsNummer = 4;
                if (!keinHopfenMehr) hopfen++;
          }
        }
        if (timerEnde) {  //schaltet die automatik aus wenn der timer abgelaufen ist
              autoKochenEnde = true;
            }
        if (autoKochenEnde){  //Schaltet alles auf Anfang wenn die Automatik vorbei ist
          autoKochenEnde = false;
          datenUebergeben = false;
          timerEnde = false;
          timerStart = false;
          if (!meldung){
            alarmON = true;
            meldungsNummer = 1;
          }
          timerSet = timerMin;
          autoKochenStart = false;
          heizungOFF = true;
          ruehrerOFF = true;
          ruehrerStart = false;
          hopfen = 1;
          letzterHopfen = 1;
          logTime = 0;
          logGestartet = false;
          meldung = false;
          datenUebergeben = false;
          maximaKochen = false;
          meldung = false;
          keinHopfenMehr = false;
          manuellEnde = false;
          timerHopfenEnde = false;
        }
      }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////User Interface//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void userInterface() {
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////Meldungen die bestätigt werden müssen
 if (meldung && meldungsNummer != 2 && (manuellStart || autoMaischenStart || autoKochenStart)) {  
        lcd.home();
        lcd.print("  ZUM BEENDEN   ");  //meldung beim Abbrechen von Programmen
        lcd.setCursor(0,1);
        lcd.print("  OK DRUECKEN   ");
 }
 else if (alarmON && meldungsNummer == 1) {  //wenn programm fertig ist
       lcd.home();
       lcd.print("    PROGRAMM    ");
       lcd.setCursor(0,1);
       lcd.print("     FERTIG     ");
 }
 else if (alarmON && meldungsNummer == 2) {  //Meldung zum Einmaischen
       lcd.home();
       lcd.print("      JETZT     ");
       lcd.setCursor(0,1);
       lcd.print("   EINMAISCHEN  ");
 }
 else if (meldung && meldungsNummer == 2) {  
        lcd.home();
        lcd.print("NACH EINMAISCHEN");  //meldung beim Abbrechen von Programmen
        lcd.setCursor(0,1);
        lcd.print("  OK DRUECKEN   ");
 }
 else if (alarmON && meldungsNummer == 3) {  //Meldung zum Einmaischen
       lcd.home();
       lcd.print("     JETZT      ");
       lcd.setCursor(0,1);
       lcd.print("   ABMAISCHEN   ");
 }
 else if (alarmON && meldungsNummer == 4) {  //Meldung zur Hopfengabe
       lcd.home();
       lcd.print("      JETZT     ");
       lcd.setCursor(0,1);
       lcd.print("  ");
       if (!keinHopfenMehr) lcd.print(hopfen - 1);
       else lcd.print(hopfen);
       lcd.print(".HOPFENGABE  ");
 }
 else if (meldung && meldungsNummer == 2) {  
        lcd.home();
        lcd.print("NACH EINMAISCHEN");  //meldung beim Abbrechen von Programmen
        lcd.setCursor(0,1);
        lcd.print("  OK DRUECKEN   ");
 }
 if (meldung && automatikBestaetigung) {  //übersicht bevor die automatik startet
      lcd.home();
      if (hMenue == 2) {
        lcd.print("AutoM ");
        lcd.print(tempStepMax);lcd.write(byte(1));
        lcd.print("C Tmax.");
        lcd.setCursor(0,1);
        lcd.print("insg.");
        if (timerStepMax < 10) lcd.print("  ");
        else if (timerStepMax < 100) lcd.print(" ");
        lcd.print(timerStepMax);lcd.print("Min");
      }
      if (hMenue == 3){
        lcd.print("AutoK ");
        lcd.print(tempKochen);lcd.write(byte(1));
        lcd.print("C Tkoch");
        lcd.setCursor(0,1);
        lcd.print("insg.");
        if (timerKochen < 10) lcd.print("  ");
        else if (timerKochen < 100) lcd.print(" ");
        lcd.print(timerKochen);lcd.print("Min");
      }
      lcd.setCursor(12,1);
      lcd.write(byte(2));lcd.print("OK?");
 }
 if (!meldung && !alarmON){ //menüs sind deaktiviert wenn meldungen an sind
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  if (hMenueON && !nMenueON){///////////////////////////////////////////////////////////////////////////////Hauptmenü
    switch(hMenue) {
    case 1: {
      lcd.home();
      lcd.write(byte(2)); lcd.print("Manuell       ");lcd.write(byte(4));
      lcd.setCursor(0,1);
      lcd.print(" AutoMaischen  ");lcd.write(byte(5));
      break;
    }
    case 2: {
      lcd.home();
      lcd.print(" Manuell       ");lcd.write(byte(4));
      lcd.setCursor(0,1);
      lcd.write(byte(2)); lcd.print("AutoMaischen  ");lcd.write(byte(5));
      break;
    }
    case 3: {
      lcd.setCursor(0,0);
      lcd.write(byte(2)); lcd.print("AutoKochen    ");lcd.write(byte(4));
      lcd.setCursor(0,1);
      lcd.print(" Status        ");lcd.write(byte(5));
      break;
    }
    case 4: {
      lcd.setCursor(0,0);
      lcd.print(" AutoKochen    ");lcd.write(byte(4));
      lcd.setCursor(0,1);
      lcd.write(byte(2)); lcd.print("Status        ");lcd.write(byte(5));
      break;
    }
  }
 }
  if (!hMenueON && nMenueON) {////////////////////////////////////////////////////////////////////////Nebenmenüs
    ////////////////////////////////
    /////////////////////////////////
    //////////////////////////////////
    /////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////Manueller Modus
    if (hMenue == 1) { 
      switch(manuellMenue) {
        case 1: {  //Rührerstufe einstellen
          lcd.home();
          lcd.print("Manuell         ");
          lcd.setCursor(0,1);
          lcd.print("  R");lcd.write(byte(0));lcd.print("hrer ");
          lcd.write(byte(2)); 
          if (ruehrerStufe == 2) lcd.print("Stufe2");
          else if (ruehrerStufe == 1) lcd.print("Stufe1");
          else lcd.print("OFF   ");
          break;
        }
        case 2:  { //Temperatureinstellung
          lcd.home();
          lcd.print("Manuell         ");
          lcd.setCursor(0,1);
          lcd.print("   "); lcd.write(byte(2));  
          if (manuellTemp < 10) lcd.print("Temp.   ");//Stellt passend zur Temperatur die Anzeige ein
          else if (manuellTemp < 100) lcd.print("Temp.   ");
          else if (manuellTemp >= 100) lcd.print("Temp.  ");
          lcd.print(manuellTemp);
          lcd.write(byte(1));lcd.print("C");
          break;      
        }
        case 3: {  //Timereinstellung
          lcd.home();
          lcd.print("Manuell         ");
          lcd.setCursor(0,1);
          lcd.print("   "); lcd.write(byte(2)); 
          if (timerManuell < 10) lcd.print("Dauer   ");// stellt passend zur Zeit die Anzeige ein
          else if (timerManuell < 100) lcd.print("Dauer  ");
          else if (timerManuell >= 100) lcd.print("Dauer ");
          lcd.print(timerManuell);
          lcd.print("Min");
          break;
        }
        case 4: {  //Bestätigen für Start
          lcd.home();
          lcd.print("Manuell         ");
          lcd.setCursor(0,1);
          lcd.print("          ");lcd.write(byte(2));lcd.print("START");
          break;
        }
        case 5: {  //Übersicht während des Programms
          lcd.home();
          lcd.print("MANUELL  ");
          if (timerManuell < 10) lcd.print("  ");
          else if (timerManuell < 100) lcd.print(" ");
          lcd.print(timerManuell);lcd.print("m");
          if (timerSekunden < 10) lcd.print(" ");
          lcd.print(timerSekunden);lcd.print("s");
          lcd.setCursor(0,1);
          lcd.print(" ");
          if (tempT() < 100) lcd.print(" ");
          lcd.print(tempT(),1);
          lcd.write(byte(1));lcd.print("C");
          lcd.write(byte(6));
          lcd.print(tempSoll);lcd.print(".0");
          lcd.write(byte(1));lcd.print("C");
          break;
        }
      }
    }
    /////////////////////
    ///////////////////////
    //////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if (hMenue == 2) { ////////////////////////////////////////////////////////////////////////////////AutoMaischen Modus
      if (autoMaischenStartDruecken) { //Startanzeige in den Einstellungen
            lcd.home();
            if (autoMaischenMenue == 19) lcd.print("                 ");
            else {
              lcd.print(" n");lcd.write(byte(3));lcd.print("chste Rast   ");
            }
            lcd.setCursor(0,1);
            lcd.write(byte(2));
            lcd.print("START          ");
      }
      if (autoMaischenStart) {  //////////////////////////////////////////Die StatusDisplays während des Automaischens
        if (autoMaischenStatus == 1) {  //allgemeine Übersicht, istTemp, sollTemp, verbleibende Zeit
          lcd.home();
          lcd.print("NOW: ");
          lcd.print(currentStep);
          if (timerMinuten < 10) lcd.print(")   ");
          else if (timerMinuten < 100) lcd.print(")  ");
          else lcd.print(") ");
          lcd.print(timerMinuten); lcd.print("m");
          if (timerSekunden < 10) lcd.print(" ");
          lcd.print(timerSekunden);
          lcd.print("s");
          lcd.write(byte(4));
          lcd.setCursor(0,1);
          lcd.print("  ");
          if (tempT() < 10) lcd.print(" ");
          lcd.print(tempT(),1);lcd.write(byte(1));lcd.print("C");
          lcd.write(byte(6));
          lcd.print(tempSoll);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
          lcd.write(byte(5));
        }
        else if (autoMaischenStatus  == 2) { // zeigt den nächsten step an, solltemp, dauer
              lcd.home();
              lcd.print("NXT: ");
              if (currentStep < letzterStep){
                lcd.print(currentStep+1);
                lcd.print(") ");
              switch(currentStep) {
                case 1:{
                  if (timerStep2 < 10) lcd.print("  ");
                  else if (timerStep2 < 100) lcd.print(" ");
                  lcd.print(timerStep2);
                  lcd.print("m 0s");
                  lcd.write(byte(4));
                  lcd.setCursor(0,1);
                  if (!einmaischenFertig) lcd.print("einmai. ");  //anzeige wenn als nächstes eingemaischt werden muss
                  else lcd.print("       ");
                  lcd.write(byte(6));lcd.print(tempStep2);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
                  lcd.write(byte(5));
                  break;
                }
                case 2:{
                  if (timerStep3 < 10) lcd.print("  ");
                  else if (timerStep3 < 100) lcd.print(" ");
                  lcd.print(timerStep3);
                  lcd.print("m 0s");
                  lcd.write(byte(4));
                  lcd.setCursor(0,1);
                  lcd.print("         "); 
                  lcd.write(byte(6));lcd.print(tempStep3);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
                  lcd.write(byte(5));
                  break;
                }
                case 3:{
                  if (timerStep4 < 10) lcd.print("  ");
                  else if (timerStep4 < 100) lcd.print(" ");
                  lcd.print(timerStep4);
                  lcd.print("m 0s");
                  lcd.write(byte(4));
                  lcd.setCursor(0,1);
                  lcd.print("         "); 
                  lcd.write(byte(6));lcd.print(tempStep4);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
                  lcd.write(byte(5));
                  break;
                }
                case 4:{
                  if (timerStep5 < 10) lcd.print("  ");
                  else if (timerStep5 < 100) lcd.print(" ");
                  lcd.print(timerStep5);
                  lcd.print("m 0s");
                  lcd.write(byte(4));
                  lcd.setCursor(0,1);
                  lcd.print("         "); 
                  lcd.write(byte(6));lcd.print(tempStep5);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
                  lcd.write(byte(5));
                  break;
                }
                case 5:{
                  if (timerStep6 < 10) lcd.print("  ");
                  else if (timerStep6 < 100) lcd.print(" ");
                  lcd.print(timerStep6);
                  lcd.print("m 0s");
                  lcd.write(byte(4));
                  lcd.setCursor(0,1);
                  lcd.print("         ");
                  lcd.write(byte(6));lcd.print(tempStep6);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
                  lcd.write(byte(5));
                  break;
                }
              }
              
            }
            else { //wenn als nächstes geläutert werden muss
                lcd.print("          ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("abmai.         ");
                lcd.write(byte(5));
              }
          }
          else if (autoMaischenStatus  == 3){  //zeigt welche rührerstufe an ist
            lcd.home();
            switch (ruehrerStufe){
              case 1: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer Stufe1  ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("Stufe2  ");
                lcd.write(byte(5));
                 break;    
              }
            case 2: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer Stufe2  ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("OFF     ");
                lcd.write(byte(5));   
                 break;    
              }
            case 0: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer OFF     ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("Stufe1  ");
                lcd.write(byte(5));
                 break;    
              }
            }
          }
        }
      else if (!autoMaischenStartDruecken) {/////////////////////////////////////die Einstellungen für das Automaischen
        switch (autoMaischenMenue) {
          case 1: {  //Rührerstufe einstellen
          lcd.home();
          lcd.print("AutoMaischen    ");
          lcd.setCursor(0,1);
          lcd.print("  R");lcd.write(byte(0));lcd.print("hrer ");
          lcd.write(byte(2)); 
          if (ruehrerStufe == 2) lcd.print("Stufe2");
          else if (ruehrerStufe == 1) lcd.print("Stufe1");
          else lcd.print("OFF   ");
          break;
        }
        
        case 2: {  //EinmaischTemperatur einstellen
          lcd.home();
          lcd.print("Einmaischen bei ");
          lcd.setCursor(0,1);
          lcd.print("           ");
          lcd.write(byte(2));
          lcd.print(tempEinmaischen);
          lcd.write(byte(1));lcd.print("C");
          break;
        }
          case 3:  { //Temperatureinstellung/////////////////////////////////////////STEP1
            lcd.home();
            lcd.print("1) ");lcd.write(byte(2));
            lcd.print("Temp   ");
            lcd.print(tempStep1);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep1 < 10) lcd.print("    Dauer   ");
            else if (timerStep1 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep1);lcd.print("Min");
            break;
          }
          case 4:  { //Zeiteinstellung
            lcd.home();
            lcd.print("1)  Temp   ");
            lcd.print(tempStep1);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep1 < 10) lcd.print("Dauer   ");
            else if (timerStep1 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep1);lcd.print("Min");
            break;
          }
          case 5:  { //Temperatureinstellung////////////////////////////////////STEP2
            lcd.home();
            lcd.print("2) ");lcd.write(byte(2));lcd.print("Temp   ");
            lcd.print(tempStep2);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep2 < 10) lcd.print("    Dauer   ");
            else if (timerStep2 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep2);lcd.print("Min");
            break;
          }
          case 6:  { //Zeiteinstellung
            lcd.home();
            lcd.print("2)  Temp   ");
            lcd.print(tempStep2);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep2 < 10) lcd.print("Dauer   ");
            else if (timerStep2 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep2);lcd.print("Min");
            break;
          }
          case 7:  { //nächster schritt
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chste Rast   ");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 8:  { //Temperatureinstellung////////////////////////////STEP3
            lcd.home();
            lcd.print("3) ");lcd.write(byte(2));
            lcd.print("Temp   ");
            lcd.print(tempStep3);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep3 < 10) lcd.print("    Dauer   ");
            else if (timerStep3 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep3);lcd.print("Min");
            break;
          }
          case 9:  { //Zeiteinstellung
            lcd.home();
            lcd.print("3)  Temp   ");
            lcd.print(tempStep3);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep3 < 10) lcd.print("Dauer   ");
            else if (timerStep3 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep3);lcd.print("Min");
            break;
          }
          case 10:  { //nächster schritt
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chste Rast   ");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 11:  { //Temperatureinstellung////////////////////////////STEP4
            lcd.home();
            lcd.print("4) ");lcd.write(byte(2));
            lcd.print("Temp   ");
            lcd.print(tempStep4);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep4 < 10) lcd.print("    Dauer   ");
            else if (timerStep4 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep4);lcd.print("Min");
            break;
          }
          case 12:  { //Zeiteinstellung
            lcd.home();
            lcd.print("4)  Temp   ");
            lcd.print(tempStep4);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep4 < 10) lcd.print("Dauer   ");
            else if (timerStep4 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep4);lcd.print("Min");
            break;
          }
          case 13:  { //nächster schritt
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chste Rast   ");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 14:  { //Temperatureinstellung////////////////////////////STEP5
            lcd.home();
            lcd.print("5) ");lcd.write(byte(2));
            lcd.print("Temp   ");
            lcd.print(tempStep5);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep5 < 10) lcd.print("    Dauer   ");
            else if (timerStep5 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep5);lcd.print("Min");
            break;
          }
          case 15:  { //Zeiteinstellung
            lcd.home();
            lcd.print("5)  Temp   ");
            lcd.print(tempStep5);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep5 < 10) lcd.print("Dauer   ");
            else if (timerStep5 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep5);lcd.print("Min");
            break;
          }
          case 16:  { //nächster schritt
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chste Rast   ");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 17:  { //Temperatureinstellung////////////////////////////STEP6
            lcd.home();
            lcd.print("6) ");lcd.write(byte(2));lcd.print("Temp   ");
            lcd.print(tempStep6);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            if (timerStep6 < 10) lcd.print("    Dauer   ");
            else if (timerStep6 < 100) lcd.print("    Dauer  ");
            else lcd.print("    Dauer ");
            lcd.print(timerStep6);lcd.print("Min");
            break;
          }
          case 18:  { //Zeiteinstellung
            lcd.home();
            lcd.print("6)  Temp   ");
            lcd.print(tempStep6);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("   ");lcd.write(byte(2));
            if (timerStep6 < 10) lcd.print("Dauer   ");
            else if (timerStep6 < 100) lcd.print("Dauer  ");
            else lcd.print("Dauer ");
            lcd.print(timerStep6);lcd.print("Min");
            break;
          }
          case 19:  { //Start
            lcd.home();
            lcd.print("                ");
            lcd.setCursor(0,1);
            lcd.write(byte(2));
            lcd.print("START          ");
            break;
          }
        }
      }
    }
    /////////////////////
    ///////////////////////
    //////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if (hMenue == 3) { //////////////////////////////////////////////////////////////////////////////////////AutoKochen Modus
      if (autoKochenStartDruecken) { //Startanzeige in den Einstellungen
            lcd.home();
            if (autoKochenMenue == 11) lcd.print("                 ");
            else {
              lcd.print(" n");lcd.write(byte(3));lcd.print("chster Hopfen");
            }
            lcd.setCursor(0,1);
            lcd.write(byte(2));
            lcd.print("START           ");
      }
      if (autoKochenStart) {  //////////////////////////////////////////Die StatusDisplays während des Autokochens
        if (autoKochenStatus == 1) {  //allgemeine Übersicht, istTemp, sollTemp, verbleibende Zeit
          lcd.home();
          if (hopfen <= letzterHopfen && !keinHopfenMehr){
            lcd.print("NXT: ");
            lcd.print(hopfen);
            lcd.print(") ");
            if (timerHopfenMinuten < 10) lcd.print("  ");
            else if (timerHopfenMinuten < 100) lcd.print(" ");
            lcd.print(timerHopfenMinuten); lcd.print("m");
            if (timerHopfenSekunden < 10) lcd.print(" ");
            lcd.print(timerHopfenSekunden);
            lcd.print("s");
          }
          else {
            lcd.print("Ende in ");
            if (timerMinuten < 10) lcd.print("  ");
            else if (timerMinuten < 100) lcd.print(" ");
            lcd.print(timerMinuten); lcd.print("m");
            if (timerSekunden < 10) lcd.print(" ");
            lcd.print(timerSekunden);
            lcd.print("s");
          }
          lcd.write(byte(4));
          lcd.setCursor(0,1);
          if (tempT() < 100) lcd.print(" ");
          lcd.print(tempT(),1);lcd.write(byte(1));lcd.print("C");
          lcd.write(byte(6));
          lcd.print(tempSoll);lcd.print(".0");lcd.write(byte(1));lcd.print("C");
          lcd.write(byte(5));
        }
        else if (autoKochenStatus  == 2) { // zeigt den nächsten step an, solltemp, dauer
              lcd.home();
              lcd.print("NXT: ");
              if (hopfen < letzterHopfen){
                lcd.print(hopfen + 1);
                lcd.print(")  Hopfen");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("     in ");
              switch(hopfen) {
                case 1:{
                  if (timerHopfenMinuten + hopfenAlarm2 < 10) lcd.print("  ");
                  else if (timerHopfenMinuten + hopfenAlarm2 < 100) lcd.print(" ");
                  lcd.print(timerHopfenMinuten + hopfenAlarm2);
                  lcd.print("m");
                  if (timerHopfenSekunden < 10) lcd.print(" ");
                  lcd.print(timerHopfenSekunden);
                  lcd.print("s");
                  lcd.write(byte(5));
                  break;
                }
                case 2:{
                  if (timerHopfenMinuten + hopfenAlarm3 < 10) lcd.print("  ");
                  else if (timerHopfenMinuten + hopfenAlarm3 < 100) lcd.print(" ");
                  lcd.print(timerHopfenMinuten + hopfenAlarm3);
                  lcd.print("m");
                  if (timerHopfenSekunden < 10) lcd.print(" ");
                  lcd.print(timerHopfenSekunden);
                  lcd.print("s");
                  lcd.write(byte(5));
                  break;
                }
                case 3:{
                  if (timerHopfenMinuten + hopfenAlarm3 < 10) lcd.print("  ");
                  else if (timerHopfenMinuten + hopfenAlarm3 < 100) lcd.print(" ");
                  lcd.print(timerHopfenMinuten + hopfenAlarm3);
                  lcd.print("m");
                  if (timerHopfenSekunden < 10) lcd.print(" ");
                  lcd.print(timerHopfenSekunden);
                  lcd.print("s");
                  lcd.write(byte(5));
                  break;
                }
                case 4:{
                  if (timerHopfenMinuten + hopfenAlarm4 < 10) lcd.print("  ");
                  else if (timerHopfenMinuten + hopfenAlarm4 < 100) lcd.print(" ");
                  lcd.print(timerHopfenMinuten + hopfenAlarm4);
                  lcd.print("m");
                  if (timerHopfenSekunden < 10) lcd.print(" ");
                  lcd.print(timerHopfenSekunden);
                  lcd.print("s");
                  lcd.write(byte(5));
                  break;
                }
              }
            }
            else if (hopfen == letzterHopfen) { //wenn das der letzte hopfen ist
                lcd.print("-)        ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.write(byte(5));
              }
          }
          else if (autoKochenStatus == 3) {  //zeigt verbleibende Kochdauer an
            lcd.home();
            lcd.print("Kochen fertig  ");lcd.write(byte(4));
            lcd.setCursor(0,1);
            lcd.print("     in ");
            if (timerMinuten < 10) lcd.print("  ");
            else if (timerMinuten < 100) lcd.print(" ");
            lcd.print(timerMinuten);lcd.print("m");
            if (timerSekunden < 10) lcd.print(" ");
            lcd.print(timerSekunden);
            lcd.print("s");
            lcd.write(byte(5));
          }
          else if (autoKochenStatus  == 4){  //zeigt welche rührerstufe an ist
            lcd.home();
            switch (ruehrerStufe){
              case 1: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer Stufe1  ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("Stufe2  ");
                lcd.write(byte(5));  
                 break;    
              }
            case 2: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer Stufe2  ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("OFF     ");
                lcd.write(byte(5));  
                 break;    
              }
            case 0: {
                lcd.print("R");lcd.write(byte(0));lcd.print("hrer OFF     ");
                lcd.write(byte(4));
                lcd.setCursor(0,1);
                lcd.print("      ");
                lcd.write(byte(2));lcd.print("Stufe1  ");
                lcd.write(byte(5)); 
                 break;    
              }
            }
          }
        }
      else if (!autoKochenStartDruecken) {//////////////////////////////////////////////////die Einstellungen für das Autokochen
        switch (autoKochenMenue) {
          case 1: {  //Rührerstufe einstellen
          lcd.home();
          lcd.print("AutoKochen      ");
          lcd.setCursor(0,1);
          lcd.print("  R");lcd.write(byte(0));lcd.print("hrer ");
          lcd.write(byte(2)); 
          if (ruehrerStufe == 2) lcd.print("Stufe2");
          else if (ruehrerStufe == 1) lcd.print("Stufe1");
          else lcd.print("OFF   ");
          break;
        }
          case 2:  { //Temperatureinstellung/////////////////////////////////////////kochtemperatur
            lcd.home();
            lcd.write(byte(2));
            lcd.print("KochTemp");
            if (tempKochen < 100)lcd.print("  ");
            else lcd.print(" ");
            lcd.print(tempKochen);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print(" KochDauer");
            if (timerKochen < 10) lcd.print("  ");
            else if (timerKochen < 100) lcd.print(" ");
            lcd.print(timerKochen);lcd.print("Min");
            break;
          }
          case 3:  { //Zeiteinstellung
            lcd.home();
            lcd.print(" KochTemp");
            if (tempKochen < 10)lcd.print("   ");
            else lcd.print("  ");
            lcd.print(tempKochen);
            lcd.print(" ");lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.write(byte(2));
            lcd.print("KochDauer");
            if (timerKochen < 10) lcd.print("  ");
            else if (timerKochen < 100) lcd.print(" ");
            lcd.print(timerKochen);lcd.print("Min");
            break;
          }
          case 4:  { //Zeiteinstellung erster hopfen
            lcd.home();
            lcd.print("1. Hopfen");
            if (timerHopfen1 < 10) lcd.print("   ");
            else if (timerHopfen1 < 100) lcd.print("  ");
            else lcd.print(" ");
            lcd.print(timerHopfen1);lcd.print("Min");
            lcd.setCursor(0,1);
            lcd.print("   vor Koch-Ende");
            break;
          }
          case 5:  { //nächster hopfen
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chster Hopfen");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 6:  { //Zeiteinstellung 2. hopfen
            lcd.home();
            lcd.print("2. Hopfen");
            if (timerHopfen2 < 10) lcd.print("   ");
            else if (timerHopfen2 < 100) lcd.print("  ");
            else lcd.print(" ");
            lcd.print(timerHopfen2);lcd.print("Min");
            lcd.setCursor(0,1);
            lcd.print("   vor Koch-Ende");
            break;
          }
          case 7:  { //nächster hopfen
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chster Hopfen");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 8:  { //Zeiteinstellung 3. hopfen
            lcd.home();
            lcd.print("3. Hopfen");
            if (timerHopfen3 < 10) lcd.print("   ");
            else if (timerHopfen3 < 100) lcd.print("  ");
            else lcd.print(" ");
            lcd.print(timerHopfen3);lcd.print("Min");
            lcd.setCursor(0,1);
            lcd.print("   vor Koch-Ende");
            break;
          }
          case 9:  { //nächster hopfen
            lcd.home();
            lcd.write(byte(2));
            lcd.print("n");lcd.write(byte(3));lcd.print("chster Hopfen");
            lcd.setCursor(0,1);
            lcd.print(" START          ");
            break;
          }
          case 10:  { //Zeiteinstellung 4. hopfen
            lcd.home();
            lcd.print("4. Hopfen");
            if (timerHopfen4 < 10) lcd.print("   ");
            else if (timerHopfen4 < 100) lcd.print("  ");
            else lcd.print(" ");
            lcd.print(timerHopfen4);lcd.print("Min");
            lcd.setCursor(0,1);
            lcd.print("   vor Koch-Ende");
            break;
          }
          case 11:  { //Start
            lcd.home();
            lcd.print("                ");
            lcd.setCursor(0,1);
            lcd.write(byte(2));
            lcd.print("START          ");
            break;
          }
        }
      }
    }
    if (hMenue == 4) { ////////////////////////////////////Statusmenü
      switch(statusMenue) {
        case 1:  {
          lcd.home();
            lcd.print("TempTopf ");
            if (tempT() < 10) lcd.print("  ");
            else if (tempT() < 100) lcd.print(" ");
            lcd.print(tempT(),1);lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("Version     ");
            lcd.print(software);
            break;
        }
        case 2: {
          lcd.home();
            lcd.print("TempElek");
            if (tempI() < 10) lcd.print("  ");
            else if (tempI() < 100) lcd.print(" ");
            lcd.print(tempI());lcd.write(byte(1));lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("                ");
            break;
        }
        case 3:  {
          lcd.home();
            lcd.print("Laufzeit/Run    ");
            lcd.setCursor(0,1);
            lcd.print("        microSec");
            lcd.setCursor(0,1);
            lcd.print(runtime);
            break;
        }
        case 4:  {
          lcd.home();
            lcd.print("LED+Buzzer Test ");
            lcd.setCursor(0,1);
            lcd.print("                ");
            break;
        }
      }
    }
  }
 }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////GoBetwino///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////Start von GoBetwino.exe resetet den Arduino////////////////////////////////////////////
////////////////////////////////////(http://www.mikmo.dk/gobetwino.html) ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gobetwino() {
    char buffer[20];
      if (!logGestartet) {
        Serial.print("#S|LOGTEST|["); //Command in Gobetwino muss LOGTEST heißen und ein LGFIL sein
        Serial.print("ProgrammZeit");
        Serial.print(";");
        Serial.print("IstTemp*10");
        Serial.print(";");
        Serial.print("SollTemp*10");
        Serial.print(";");
        Serial.print("SollTempErreicht");
        Serial.print(";");
        Serial.print("timerMinuten");
        Serial.print(";");
        /*Serial.print("HeizungON");
        Serial.print(";");
        Serial.print("Programm");
        Serial.print(";");
        if (autoMaischenStart) Serial.print("SchrittNr");
        else if (autoKochenStart) Serial.print("HopfenNr");
        else Serial.print(" ");
        Serial.print(";");
        Serial.print("Meldungen");
        Serial.print(";");
        Serial.print("RuehrerStufe");
        Serial.print(";");*/
        Serial.print("Laufzeit/Run(microsec)");
        Serial.println("]#");
        logGestartet = true;
      }
    if (millis() - logPreviousMillis >= gobetwinoIntervall) {
      logPreviousMillis = millis();
      logTime+=(gobetwinoIntervall/1000);
       Serial.print("#S|LOGTEST|[");
        Serial.print(itoa(logTime, buffer, 10));
        Serial.print(";");
        Serial.print(itoa((tempT()*10), buffer, 10)); //temperatur Topfboden
        Serial.print(";");
        Serial.print(itoa(tempSoll*10, buffer, 10));
        Serial.print(";");
        if (tempSollErreicht) Serial.print("1");
        else Serial.print("0");
        Serial.print(";");
        Serial.print(itoa(timerMinuten, buffer, 5));
        Serial.print(";");
        /*if (heizung() == true) Serial.print("1");
        else Serial.print("0");
        Serial.print(";");
        if (autoMaischenStart) Serial.print("autoM");
        else if (autoKochenStart) Serial.print("autoK");
        else if (manuellStart) Serial.print("Manuell");
        Serial.print(";");
        if (autoMaischenStart) Serial.print(itoa(currentStep, buffer, 10));
        else if (autoKochenStart) {
          if (!keinHopfenMehr) Serial.print(itoa(hopfen, buffer, 10));
          else Serial.print("0");
        }
        else Serial.print("0");
        Serial.print(";");
        if (autoMaischenStart) {
          if (einmaischenJetzt) Serial.print("jetztEinmaischen");
        }
        else if (autoKochenStart) {
          if (alarmON) {
           Serial.print(itoa(hopfen, buffer, 10));
           Serial.print(".Hopfengabe");
          }
        }
        else Serial.print(" ");
        Serial.print(";");
        if (ruehrerStufe1() == true) Serial.print("1");
        else if (ruehrerStufe2() == true) Serial.print("2");
        else Serial.print(" ");
        Serial.print(";");*/
        Serial.print(itoa(runtime, buffer, 10));
        Serial.println("]#");
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////Serielle Ausgaben///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*void serial() {
  Serial.print("  gradient   ");
  Serial.print(gradient);
  Serial.print("  tempT():   ");
  Serial.print(tempT());
  Serial.print("  h1:   ");
  //Serial.print(h1);
  Serial.print("  millis():   ");
  Serial.print(millis());
  Serial.print("  heizungVorherigeZeit:   ");
  Serial.print(heizungVorherigeZeit);
  Serial.print("   meldung: ");
  Serial.println(meldung);
  /*Serial.print("   timerManuell:  ");
  Serial.print(timerManuell);
  Serial.print("   timerSet:  ");
  Serial.print(timerSet);
  Serial.print("   sollTempErreicht:  ");
  Serial.print(tempSollErreicht);
  Serial.print("   timerSekunden: ");
  Serial.print(timerSekunden);
  Serial.print("   timerStart: ");
  Serial.print(timerStart);
  Serial.print("   timerEnde: ");
  Serial.print(timerEnde);
  /*Serial.print("   manuellMenue:  ");
  Serial.print(manuellMenue);
  
  Serial.print("   manuellStart:   ");
  Serial.print(manuellStart);
  Serial.print("   meldung:  ");
  Serial.print(meldung);
Serial.print("   tempSensorDaempfung: ");
  Serial.print(tempSensorDaempfung);
  Serial.print("  ruehrerStufe:   ");
  Serial.print(ruehrerStufe);
  Serial.print("  ruehrerPauseGestartet:   ");
  Serial.print(ruehrerPauseGestartet);*/
  /*Serial.print("   hMenue:   ");
  Serial.print(hMenue);
  Serial.print("   autoMaischenMenue:   ");
  Serial.print(autoMaischenMenue);
  Serial.print("   autoMaischenStart:   ");
  Serial.print(autoMaischenStart);
  Serial.print("   hMenueON:   ");
  Serial.print(hMenueON);
  Serial.print("   nMenueON:   ");
  Serial.print(nMenueON);
  Serial.print("   alarmON:   ");
  Serial.print(alarmON);
  
  Serial.print("   timerSet: ");
  Serial.print(timerSet);
  Serial.print("   timerEnde:   ");
  Serial.print(timerEnde);
  Serial.print("   autoKochenEnde:   ");
  Serial.print(autoKochenEnde);
  /*
  Serial.print("   manuellEnde:   ");
  Serial.print(manuellEnde);
  Serial.print("   alarmON:   ");
  Serial.print(alarmON);
  Serial.print("   timerEnde:   ");
  Serial.print(timerEnde);*/
  /*Serial.print("   hopfenAlarm1:   ");
  Serial.print(hopfenAlarm1);
  Serial.print("   hopfenAlarm2:   ");
  Serial.print(hopfenAlarm2);
  Serial.print("   hopfenAlarm3:   ");
  Serial.print(hopfenAlarm3);
  Serial.print("   hopfenAlarm4:   ");
  Serial.print(hopfenAlarm4);*/
  /*Serial.print("   autoMaischenStartDruecken:   ");
  Serial.print(autoMaischenStartDruecken);
  Serial.print("   autoMaischenMenue:   ");
  Serial.print(autoMaischenMenue);
  Serial.print("   letzterHopfen:   ");
  Serial.print(letzterHopfen);
  Serial.print("   timerHopfenMinuten:   ");
  Serial.print(timerHopfenMinuten);
  Serial.print("   timerHopfenStart:   ");
  Serial.print(timerHopfenStart);
  Serial.print("   timerMinuten:   ");
  Serial.print(timerMinuten);*/
  /*Serial.print("   einmaischenJetzt: ");
  Serial.print(einmaischenJetzt);  
  Serial.print("   timerStart:   ");
  Serial.print(timerStart);
  Serial.print("   currentStep:   ");
  Serial.print(currentStep);
  Serial.print("   timerSet:   ");
  Serial.print(timerSet);
  Serial.print("   timerMinuten:   ");
  Serial.print(timerMinuten);
  Serial.print("   datenUebergeben: ");
  Serial.print(datenUebergeben);
  Serial.print("   sollTempErreicht:  ");
  Serial.print(tempSollErreicht);
  Serial.print("   timerEnde:   ");
  Serial.print(timerEnde);
  Serial.print("   letzterStep:   ");
  Serial.print(letzterStep);*/
  /*Serial.print("   autoKochenEnde:   ");
  Serial.print(autoKochenEnde);
  Serial.print("   datenUebergeben: ");
  Serial.print(datenUebergeben);  
  Serial.print("   timerEnde:   ");
  Serial.print(timerEnde);
  
  Serial.print("   letzterHopfen:   ");
  Serial.print(letzterHopfen);
  Serial.print("   timerSet: ");
  Serial.print(timerSet);  */
  /*Serial.print("   hopfen:   ");
  Serial.print(hopfen);
  Serial.print("   letzterhopfen:   ");
  Serial.print(letzterHopfen);
  Serial.print("   automatikBestaetigung: ");
  Serial.print(automatikBestaetigung);
  Serial.print("   meldungsNummer: ");
  Serial.print(meldungsNummer);
  Serial.print("   zaehlerStart: ");
  Serial.print(zaehlerStart);
  Serial.print("   millis()-zaehlerStart: ");
  Serial.print(millis()-zaehlerStart);
  Serial.print("   buttonPressed: ");
  Serial.print(buttonPressed);
  Serial.print("   test: ");
  Serial.println(test);  */
  //Serial.print("   autoMaischenStart: ");
  //Serial.print(autoMaischenStart());  
  //Serial.print("   autoMaischenEnde: ");
  //Serial.print(autoMaischenEnde);  
    //Serial.print("   manuellStart: ");
  //Serial.print(manuellStart()); 
    //Serial.print("   hopfenreminderON: ");
  //Serial.print(hopfenreminderON);  
    //Serial.print("   letzterStep: ");
  //Serial.print(letzterStep);  
  //Serial.print("   currentStep: ");
  //Serial.print(currentStep); 
  //Serial.print("   tempT: ");
  //Serial.println(tempT());
  //Serial.print("   tempSoll: ");
  //Serial.print(tempSoll);
  //Serial.print("   tempSensorDaempfung: ");
  //Serial.print(tempSensorDaempfung);
  //Serial.print("   tempSollErreicht: ");
  //Serial.print(tempSollErreicht);
  
  //Serial.print("   timerSet: ");
  //Serial.println(timerSet);
  
  /*Serial.print("   pressedZurueck: ");
  Serial.print(pressedZurueck());
  Serial.print("   pressedOK:  ");
  Serial.print(pressedOK());
  Serial.print("   pressedHoch:   ");
  Serial.print(pressedHoch());
  Serial.print("   pressedRunter:  ");
  Serial.print(pressedRunter());
  Serial.print("   button:  ");
  Serial.print(button);
  Serial.print("   x:  ");
  Serial.println(x);
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////SETUP und LOOP/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600); //Serielle Ausgabe wird angeschaltet
  analogReference(EXTERNAL); //die Temperaturmessung ist dadurch genauer
  /////////////////
  //LCD Display Bibliothek wird gestartet (16 Spalten, 2 Zeilen)
  lcd.begin(16, 2); 
  lcd.createChar(0, ue);
  lcd.createChar(1, grad);
  lcd.createChar(2, pfeil);
  lcd.createChar(3, ae);
  lcd.createChar(4, up);
  lcd.createChar(5, down);
  lcd.createChar(6, soll); 
  ////////////////
  //Aktiviert Pins
  pinMode(tempTPin, INPUT);
  pinMode(tempIPin, INPUT);
  //pinMode(tempAPin, INPUT);
  pinMode(keypadPin, INPUT);
  pinMode(heizungPin, OUTPUT);
  pinMode(ruehrerPin1, OUTPUT);
  pinMode(ruehrerPin2, OUTPUT);
  pinMode(heizungLed, OUTPUT);
  pinMode(ruehrerLed1, OUTPUT);
  pinMode(ruehrerLed2, OUTPUT);
  pinMode(programmLed, OUTPUT);
  ////////////////
  //Gobetwino
  Serial.println("#S|CPTEST|[]#");  // Use the CPTEST copy file command to make a copy of a new empty logfile //Macht eine neue LogDatei aus einer bestehenden
}
void loop() {
  unsigned long start = micros();
  //
  //schaltet programme ein
  //serial();  //Aktiviert serielle Ausgaben für den Monitor
  timer();  //startet Timer für die gesamt Dauer
  eingabe();  //aktiviert die Eingabe durch buttons
  userInterface(); //Aktiviert das Userinterface
  autoMaischen(); //aktiviert Automaischen modus
  autoKochen();  //aktiviert Autokochen modus
  manuell(); //aktiviert den manuellen Modus
  //
  //schaltet die programmLed aus und ein
  if ((manuellStart || autoMaischenStart || autoKochenStart) && ledTakt() == true) digitalWrite(programmLed, HIGH); //schaltet automatik LED aus und ein
    else digitalWrite(programmLed, LOW);
  //
  //
  //Schaltet die Heizung aus und ein
  if (heizung() == true && !heizungOFF){
    digitalWrite(heizungPin, LOW);  //schaltet Heizung ein
    digitalWrite(heizungLed, HIGH); //schaltet heizungLed ein
  }
    else {
      digitalWrite(heizungPin, HIGH);   //schaltet Heizung aus
      digitalWrite(heizungLed, LOW); //schaltet heizungLed aus
    }
  //
  //Schaltet den Rührer aus und ein
  if (ruehrerStufe1() == true) { //schaltet rührer stufe 1 ein
    digitalWrite(ruehrerPin1, LOW);
    digitalWrite(ruehrerLed1, HIGH);
  }
    else {  //schaltet rührer stufe 1 aus
      digitalWrite(ruehrerPin1, HIGH);
      digitalWrite(ruehrerLed1, LOW);
    }
  if (ruehrerStufe2() == true) { //schalte rührer stufe 2 ein
    digitalWrite(ruehrerPin2, LOW);
    digitalWrite(ruehrerLed2, HIGH);
  }
    else {    //schaltet rührer stufe 2 aus
      digitalWrite(ruehrerPin2, HIGH);
      digitalWrite(ruehrerLed2, LOW);
    }
  //
  //Schaltet den Buzzer aus und ein
  if (buzzer() == true  && alarmON) tone(buzzerPin, buzzerFrequenz, buzzerDauer);
  //
  //Schaltet GoBetwino aus und ein
  if (autoMaischenStart || autoKochenStart || manuellStart) gobetwino();
  //
  //LED und Buzzer Test über statusmenü
  if (ledTest() == true) {
    if (buzzer() == true) tone(buzzerPin, buzzerFrequenz, buzzerDauer);
    digitalWrite(ruehrerLed1, HIGH);
    digitalWrite(ruehrerLed2, HIGH);
    digitalWrite(programmLed, HIGH);
    digitalWrite(heizungLed, HIGH);
  }
runtime = micros() - start;
}
