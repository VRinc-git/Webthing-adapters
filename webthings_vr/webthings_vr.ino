#include <Thing.h>
#include <WebThingAdapter.h>

#define BUILT_IN_LED   2
#define SWITCH_PIN     34
#define LED_PIN        33

typedef struct
{
  bool flag1;
  bool flag2;
} switchFlag_t;

switchFlag_t SW1 = {0, 1};


/*
   SSID and Password
*/
const char *ssid = "SSID";
const char *password = "PSWD";


/*
   Host name used by mDNS
*/
const String mDNSHostname = "RealTimeFeedback";



/*
   Handle connection between things and gateway
*/

WebThingAdapter *adapter;



/*
   @type members: Capabilities supported by your Thing
   See schemas: https://iot.mozilla.org/schemas#capabilities
*/
const char *ledTypes[] = {"onoffswitch",
                          "Light",
                          nullptr
                         };


/*
   Description of your Thing

   ThingDevice device(id, title, types)

      @id: unique identifier for Thing (part of URL: http://<IP>/things/<id>)
      @description: string that shows up in Gateway for your Thing
      @types: array of @types
*/
ThingDevice led("led", "Lamp", ledTypes);



/*
   Define one or more properties supported by your Thing

   ThingProperty property(id, description, type, atType)

      @id: unique identifier for property
      @description: user-readable description of property
      @type: NO_STATE, BOOLEAN, NUMBER, or STRING
      @atType: property @type (https://iot.mozilla.org/schemas#properties)
*/

ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");


bool lastOn = false;       // setting the value as 0



void setup() {


  pinMode(BUILT_IN_LED, OUTPUT);
  //  digitalWrite(BUILT_IN_LED, HIGH);

  //user defined pins
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);


  //Setting up serial
  Serial.begin(115200);

  Serial.print("Connecting to");
  Serial.print(ssid);

#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");


  /*
     Waiting for connection
  */
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    //    digitalWrite(BUILT_IN_LED, blink ? LOW : HIGH);
    blink = !blink;
  }
  //  digitalWrite(BUILT_IN_LED, HIGH);

  Serial.println("");
  Serial.print("Connected!");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  Serial.println("");
  Serial.print("Connected!");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  /*
    Create new WebThing connection handle (default port: 80)
  */
  adapter = new WebThingAdapter(mDNSHostname, WiFi.localIP());



  /*
     Associate properties with devices
  */
  led.addProperty(&ledOn);               //@params : passing address of the object




  /*
     Associate device with connection
  */
  adapter->addDevice(&led);




  /*
     start mDNS and HTTP server
  */

  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(led.id);



}

void loop() {
  ThingPropertyValue tpVal;
  bool pin;

  adapter->update();

  pin = digitalRead(SWITCH_PIN);
  tpVal.boolean = pin;

  if (pin == LOW)
  {
    if (SW1.flag1 == LOW)
    {
      ledOn.setValue(tpVal);
      SW1.flag1 = 1;
      SW1.flag2 = 1;
    }
  }
  if (pin == HIGH)
  {
    if (SW1.flag2 == HIGH)
    {
      ledOn.setValue(tpVal);
      SW1.flag1 = 0;
      SW1.flag2 = 0;
    }
  }

  bool isOn = ledOn.getValue().boolean;
  digitalWrite(BUILT_IN_LED, isOn ? HIGH : LOW);
}
