#include <Pixy2.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>



#define PIN   6
#define NUMPIXELS 4
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Pixy2 pixy;
SoftwareSerial BT(4, 2);


double a = 0, d = 0;
String tosend = "", info = "", xf, yf, receivedMessage;
int i;
int x_r, y_r, x_h, y_h, x_b1, y_b1, x_b2, y_b2;
bool robot = 0, head = 0, b1 = 0, b2 = 0;
double v, u, u1, v1, u2, v2, uv;
bool startcount = 1;
unsigned long times, time_follow;


//***********************************************
void setup()
{

  Serial.begin(9600);
  Serial.print("Starting...\n");
  //***************bt********************************
  BT.begin(9600);
  //******************Pixy*****************************
  pixy.init();

  //******************Neopixel*****************************
  pixels.begin();
  pixels.clear();
  //***************************
  check();
  times = millis();
}
//**************************************************************************************************
int distance(int x_r, int y_r, int x_b, int y_b) {

  return round(sqrt(pow((x_r - x_b), 2) + pow((y_r - y_b), 2)));

}
//****************************************************************************************************
int angle(int x_h, int y_h, int x_r, int y_r, int x_b, int y_b)
{
  v = sqrt(pow((x_r - x_b), 2) + pow((y_r - y_b), 2));
  u = sqrt(pow((x_r - x_h), 2) + pow((y_r - y_h), 2));
  u1 = x_b - x_r;
  v1 = x_h - x_r;
  u2 = y_b - y_r;
  v2 = y_h - y_r;
  uv = u1 * v1 + u2 * v2;
  Serial.println(uv / (v * u));
  return round(acos(uv / (v * u)) * 180 / 3.14);
}

//**************************************************************************************************

String calcinfo(String obj)
{
  robot = 0; head = 0; b1 = 0; b2 = 0; tosend = ""; info = "";
  for (i = 0; i < pixy.ccc.numBlocks; i++)
  {
    if (pixy.ccc.blocks[i].m_signature == 1)
    {
      x_h = pixy.ccc.blocks[i].m_x;
      y_h = pixy.ccc.blocks[i].m_y;
      //h_h = pixy.ccc.blocks[i].m_height;
      head = 1;
    }
    else if (pixy.ccc.blocks[i].m_signature == 2)
    {
      x_r = pixy.ccc.blocks[i].m_x;
      y_r = pixy.ccc.blocks[i].m_y;
      robot = 1;
    }
    else if (pixy.ccc.blocks[i].m_signature == 3)
    {
      x_b1 = pixy.ccc.blocks[i].m_x;
      y_b1 = pixy.ccc.blocks[i].m_y;
      b1 = 1;
    }
    else if (pixy.ccc.blocks[i].m_signature == 4)
    {
      x_b2 = pixy.ccc.blocks[i].m_x;
      y_b2 = pixy.ccc.blocks[i].m_y;
      b2 = 1;
    }

  }


  if (obj == "db1") {
    if (b1 && head && robot)
    {
      d = distance(x_h, y_h, x_b1, y_b1);
      return (String)d;
    }
    else
      return "p";
  }
  else if (obj == "ab1")
  {
    if (b1 && head && robot)
    {
      a = angle(x_h, y_h, x_r, y_r, x_b1, y_b1);
      return (String)a;
    }
    else
      return "p";
  }
  else if (obj == "db2") {
    if (b2 && head && robot)
    {
      d = distance(x_h, y_h, x_b2, y_b2);

      return (String)d;
    }
    else
      return "p";
  }
  else if (obj == "ab2")
  {
    if (b2 && head && robot)
    {
      a = angle(x_h, y_h, x_r, y_r, x_b2, y_b2);
      Serial.print("a= ");
      Serial.println(a);
      return (String)a;
    }
    else
      return "p";
  }
  else if (obj == "c")
  {
    if (robot && head)
    {
      info = (String)x_r + " " + y_r + " ";
    }
    else
      info = "n n ";
    if (b1)
    {
      info += (String)x_b1 + " " + y_b1 + " ";
    }
    else
      info += "n n ";
    if (b2)
    {
      info += (String)x_b2 + " " + y_b2;
    }
    else
      info += "n n";

    return info;
  }

}
//*************************************************************************

void check() //check what the camera sees
{
  pixels.clear();
  pixy.ccc.getBlocks();
  for (i = 0; i < pixy.ccc.numBlocks; i++)
  {
    if (pixy.ccc.blocks[i].m_signature == 1)
    {
      pixels.setPixelColor(0, pixels.Color(0, 100, 0));


    }
    else if (pixy.ccc.blocks[i].m_signature == 2)
    {
      pixels.setPixelColor(1, pixels.Color(100, 100, 0));


    }
    else if (pixy.ccc.blocks[i].m_signature == 3)
    {
      pixels.setPixelColor(2, pixels.Color(0, 0, 100));


    }
    else if (pixy.ccc.blocks[i].m_signature == 4)
    {
      pixels.setPixelColor(3, pixels.Color(100, 0, 0));


    }

  }
  pixels.show();

}

//********************************************************

void  recive_send()
{
  if (BT.available() > 0)
  {
    receivedMessage = BT.readString();
    Serial.println(receivedMessage);
    pixy.ccc.getBlocks();

    if (pixy.ccc.numBlocks && (receivedMessage == "db1" || receivedMessage == "db2" || receivedMessage == "ab1" || receivedMessage == "ab2" || receivedMessage == "c" ))
    {
      tosend = calcinfo((String)receivedMessage);
      Serial.println(tosend);
      BT.print(tosend);
    }
    else if (!pixy.ccc.numBlocks)
    {
      BT.print('p');
    }

  }

}



//***************************************************************************
void loop()
{
  recive_send(); //recive message, send information

  if (millis() - times > 3000)//every 3sec check what the camera sees
  {
    times = millis();
    check();
  }
}
