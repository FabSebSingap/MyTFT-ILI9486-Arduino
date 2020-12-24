/***********************************************************************************
*This program is a demo of clearing screen to display red,green,blue.
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program don't need to rely on any libraries and Can run directly.

* File                : _9341Mega2560.ino
* Hardware Environment: Arduino Mega2560
* Build Environment   : Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
*pin usage as follow:
*                  LCD_CS  LCD_CD  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK                          
*Arduino Mega2560    A3      A2      A1      A0      A4      10     11     12      13                           

*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*Arduino Mega2560    8       9       2       3       4       5       6       7 

*Remember to set the pins to suit your display module!
*
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <TouchScreen.h> //touch library
#include <Servo.h>
#include <SoftwareSerial.h>  //Software Serial Port  
#define RxD 25    //Pin 2 pour arduino RX (pin0=serial) 
#define TxD 27    //Pin 3 pour arduino TX 
SoftwareSerial mySerial(RxD,TxD);  

/* After including the corresponding libraries,
   we can use the "class" like "Servo" created by the developer for us.
   We can use the functions and variables created in the libraries by creating 
   objects like the following "myservo" to refer to the members in ".".*/

Servo myservo;

LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BLUELED 41
#define GREENLED 43
#define REDLED 39

int redValue = 0; // choose a value between 1 and 255 to change the color.
int greenValue = 0;
int blueValue = 255;

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

int BlueLed = 47; //Led blue digital pin

//param calibration from kbv
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92 
#define TS_MAXY 952

String CurrentMenu;
int i;
int Yaxis=500;
int16_t old_pen,current_pen,flag_pen;
int led = 0;


char BluetoothData [20]; // the data given from Bluetooth
char PCData[25]; // the data given from Computer

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//display main surface
unsigned long show_Title(uint8_t *str)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(0,0,255);
    my_lcd.Fill_Rectangle(0, 0, my_lcd.Get_Display_Width()-1, 14);// x,y,width, height
    show_string(str,CENTER,3,1,0x07E0, 0,1);
    return micros() - time_start;
}


unsigned long show_Text(uint8_t *str, uint16_t y)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(0,0,255);
    my_lcd.Fill_Rectangle(0, 0, my_lcd.Get_Display_Width()-1, 14);
    show_string(str,CENTER,y,1,0x07E0, 0,1);
    return micros() - time_start;
}

unsigned long show_Footer(uint8_t *str)
{
    unsigned long time_start = micros();
    
    my_lcd.Set_Draw_color(0,0,255);
    my_lcd.Fill_Rectangle(0, my_lcd.Get_Display_Height()-15, my_lcd.Get_Display_Width()-1, my_lcd.Get_Display_Height()-1);
    show_string(str,CENTER,my_lcd.Get_Display_Height()-11,1,0xFFFF, 0,1);

    return micros() - time_start;
}

unsigned long show_Button_haut(uint8_t *str)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(0, 255,0); 
    
    Serial.println(*str);
    Serial.println("X=150, Y=60");
    Serial.print("High Button Width: ");
    Serial.println(my_lcd.Get_Display_Width()-150);
    Serial.print("High Button Height: ");
    Serial.println(my_lcd.Get_Display_Height()-200);

    my_lcd.Draw_Rectangle(150, 60, my_lcd.Get_Display_Width()-150, my_lcd.Get_Display_Height()-200);
    my_lcd.Fill_Rect(151, 61, 179, 59,GREEN); //Fill the button in RED
    
    //display TEXT in the button
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(4);
    my_lcd.Print_String(str, CENTER,75); 
    
    return micros() - time_start;
}

unsigned long show_Button_bas(uint8_t *str)
{
    unsigned long time_start = micros();
    Serial.print("Micros value: ");
    Serial.println(time_start);

    my_lcd.Set_Draw_color(255, 255, 0);  
    my_lcd.Draw_Rectangle(150, 200, my_lcd.Get_Display_Width()-150, my_lcd.Get_Display_Height()-60);

    my_lcd.Fill_Rect(151, 201, 179, 59, MAGENTA); //Fill the button in MAGENTA
    
    my_lcd.Set_Text_colour(YELLOW);
    my_lcd.Set_Text_Size(4);
    my_lcd.Print_String(str, CENTER,215);
    return micros() - time_start;
}

unsigned long show_Button_Back()
{
    unsigned long time_start = micros();

    my_lcd.Set_Draw_color(255, 255, 0);  
    my_lcd.Draw_Rectangle(430,250,470,290); //x1,y1,x2,y2
    my_lcd.Fill_Rect(431,251,39,39,WHITE); //Fill the button in white
    
    my_lcd.Set_Text_colour(BLACK);
    my_lcd.Set_Text_Size(4);
    my_lcd.Print_String("<",438,256);
     return micros() - time_start;
}
unsigned long show_Line()
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(255, 0, 0); //RED Line

    my_lcd.Draw_Rectangle(100, 100, my_lcd.Get_Display_Width()-100, 105); // x,y,width, height
    
    //display TEXT begining of line
    my_lcd.Set_Text_colour(YELLOW);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String("0%             100%",90,70); // (Text, x, y)

    return micros() - time_start;
}

unsigned long show_VLine()
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(255, 0, 0); //RED Line

    my_lcd.Draw_Rectangle(10,40, 30, my_lcd.Get_Display_Height()-40); // x,y,width, height
    
    //display TEXT begining of line
    my_lcd.Set_Text_colour(RED);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String("RED",35,40); // (Text, x, y)

    my_lcd.Set_Text_colour(GREEN);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String("GREEN",35,my_lcd.Get_Display_Height()/2); // (Text, x, y)

    my_lcd.Set_Text_colour(BLUE);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String("BLUE",35,my_lcd.Get_Display_Height()-60); // (Text, x, y)
    
    return micros() - time_start;
}
unsigned long show_Button(uint8_t *str,int x, int y, int w, int h)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(255, 255, 0);  
    my_lcd.Draw_Rectangle(x, y, w, h);
    
    Serial.print("Valeur de y: ");
    Serial.println(y);
 
    my_lcd.Set_Text_colour(YELLOW);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String(str,CENTER,y+10);
    
    return micros() - time_start;
}

unsigned long Clear_Button(int x, int y, int w, int h, uint16_t Backcolor)
{
    unsigned long time_start = micros();
   
    my_lcd.Fill_Rect(x, y, w, h,Backcolor); //Fill the button in back color passed
    
    return micros() - time_start;
}

void MainMenu()
{
  my_lcd.Fill_Screen(0x0);  //Black
  show_Title("/*  MyTFT ILI9486  \*\\");
  show_Footer("/*  By Fabrice SEBILLE  \*\\");
  show_Button_haut("LIGHT");
  show_Button_bas("ENGINE");
  
  
   
  current_pen = 0;
  pinMode(13, OUTPUT);
  CurrentMenu="MAIN";
}

void EngineMenu()
{
  my_lcd.Fill_Screen(0x0);  //Black
  show_Title("/*  MyTFT ILI9486  \*\\");
  show_Footer("/*  By Fabrice SEBILLE  \*\\");
  show_Line();
  show_Button_bas("INIT");
    
  show_Button("",200,45,280,90); //(text, x debut, y debut, x fin, y fin) my_lcd.Get_Display_Width()-150, my_lcd.Get_Display_Height()
  show_Button_Back();
  current_pen = 0;
  pinMode(13, OUTPUT);
  i=0;
  CurrentMenu="ENGINE";
}

void LIGHTMenu()
{
  my_lcd.Fill_Screen(0x0);  //Black
  show_Title("/*  MyTFT ILI9486  \*\\");
  show_Footer("/*  By Fabrice SEBILLE  \*\\");
  show_Button_haut("ON");
  show_Button_bas("OFF");
  show_Button_Back();
  show_VLine();
  current_pen = 0;
  pinMode(13, OUTPUT);
  CurrentMenu="LIGHT";
}

void InitLcd()
{
  my_lcd.Init_LCD();
  Serial.println(my_lcd.Read_ID(), HEX);
  Serial.print("Width: ");
  Serial.println(my_lcd.Get_Display_Width());
  Serial.print("Height: ");
  Serial.println(my_lcd.Get_Display_Height());
  my_lcd.Fill_Screen(0x0);  //Black
  my_lcd.Set_Rotation(1); //Landscape
}

void InitBluetooth(){

  Serial.println("Type AT commands!"); // put your setup code here, to run once:
  mySerial.begin(9600);
  delay(500);// prepare for next data …

  mySerial.write("AT+NAMESeed");
  delay(100);// prepare for next data …
  Serial.print("Reponse HC-06 :");
  for(i;i<19;i++){
    BluetoothData[i]=mySerial.read();
  }
  Serial.println(BluetoothData);
  
  delay(100);// prepare for next data …
  mySerial.write("AT+PIN1234");
  Serial.print("Reponse HC-06 apres PIN :");
  for(int i=0;i<19;i++){
   BluetoothData[i]=char(mySerial.read());
  }
  
  for(int i=0;i<19;i++){
   BluetoothData[i]=' ';
  }
  Serial.println("BluetoothData reset ");
  Serial.println(BluetoothData);
  i=0;
  Serial.println(Serial.available());
}

void setup()
{
  Serial.begin(9600);
  //myservo.attach(27);//connect pin 27 with the control line(the middle line of Servo) 
  //myservo.write(90);// move servos to center position -> 90°
  /*pinMode(BlueLed, OUTPUT);  
  digitalWrite(BlueLed, HIGH);*/
  InitLcd();
  MainMenu();
  InitBluetooth();
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  digitalWrite(BLUELED, LOW);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{  
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (CurrentMenu=="MAIN")
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
      if ((p.y>360 && p.y<690) && (p.x>610 && p.x<750)) //Touch Button LIGHT
      {
        LIGHTMenu();
        led=1;
      }
      
      if ((p.y>360 && p.y<690) && (p.x>260 && p.x<400)) //Touch Button Engine
      {
        EngineMenu();
      }
    }
  }
  if (CurrentMenu=="LIGHT")
  {
       
        
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    {  
       /*Serial.println(CurrentMenu);
       Serial.print("Pointer y: ");
       Serial.println(p.y);
       Serial.print("Pointer x: ");
       Serial.println(p.x);*/
      
      if ((p.y>360 && p.y<690) && (p.x>610 && p.x<750)&& led==0) //Touch Button ON
      {
        delay(100);
        Serial.println("Blue ON");
        analogWrite(BlueLed, 255);

        led=1;
      }
      
      if ((p.y>360 && p.y<690) && (p.x>260 && p.x<400)) //Touch Button OFF
      {
        delay(100);
        Serial.println("Blue OFF");
        digitalWrite(BlueLed, LOW);
        redValue = 0;
        greenValue = 0;
        blueValue = 0;
        
        analogWrite(REDLED, redValue);
        analogWrite(GREENLED, greenValue);
        analogWrite(BLUELED, blueValue);
        
        Serial.print("Je suis OFF: ");
        Serial.print("\n");
        led=0;
      }
      
      if ((p.y>110 && p.y<180) && (p.x>170 && p.x<270)) //BACK Button
      {
        digitalWrite(BlueLed, LOW);
        redValue = 0;
        greenValue = 0;
        blueValue = 0;
        
        analogWrite(REDLED, redValue);
        analogWrite(GREENLED, greenValue);
        analogWrite(BLUELED, blueValue);
        MainMenu();
      }
      if ((p.y>890 && p.y<920) && (p.x>200 && p.x<800)) //Touch lINE
      {
        Serial.println("VLine");
        Serial.println(p.x);

        if(p.x>200 && p.x<300)
        {
          redValue = 0;
          greenValue = 0;
          blueValue = 255;
          
          analogWrite(REDLED, redValue);
          analogWrite(GREENLED, greenValue);
          analogWrite(BLUELED, blueValue);
          
          Serial.print("Je suis Bleu: ");
          Serial.print("\n");
      
        }
        if(p.x>300 && p.x<400)
        {
          redValue = 255;
          greenValue = 255;
          blueValue = 0;
          
          analogWrite(REDLED, redValue);
          analogWrite(GREENLED, greenValue);
          analogWrite(BLUELED, blueValue);
          
          Serial.print("Je suis Jaune: ");
          Serial.print("\n");
      
        }
        if(p.x>400 && p.x<600)
        {
          redValue = 0; // choose a value between 1 and 255 to change the color.
          greenValue = 255;
          blueValue = 0;
          
          analogWrite(REDLED, redValue);
          analogWrite(GREENLED, greenValue);
          analogWrite(BLUELED, blueValue);
          
          Serial.print("Je suis Vert: ");
          Serial.print("\n"); 
        }
        if(p.x>600 && p.x<700)
        {
          redValue = 255; // choose a value between 1 and 255 to change the color.
          greenValue = 0;
          blueValue = 255;
          
          analogWrite(REDLED, redValue);
          analogWrite(GREENLED, greenValue);
          analogWrite(BLUELED, blueValue);
          
          Serial.print("Je suis Rose: ");
          Serial.print("\n"); 
        }
        if(p.x>700 && p.x<800)
        {
          redValue = 255; // choose a value between 1 and 255 to change the color.
          greenValue = 0;
          blueValue = 0;
          
          analogWrite(REDLED, redValue);
          analogWrite(GREENLED, greenValue);
          analogWrite(BLUELED, blueValue);
          
          Serial.print("Je suis Rouge: ");
          Serial.print("\n"); 
        }
        
      }
      else
      {
        led=0;
      }
    }
  }

  if (CurrentMenu=="ENGINE")
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
      /*Serial.print("p.x :");
      Serial.println(p.x);
      Serial.print("p.y :");
      Serial.println(p.y);*/
      
      if ((p.y>270 && p.y<760) && (p.x>610 && p.x<680)) //Touch Button lINE
      {
        Clear_Button(201,46,79,44, BLACK); //(x debut, y debut, x fin, y fin, Back color of the button) my_lcd.Get_Display_Width()-150, my_lcd.Get_Display_Height()
        
        i=0;
        i=p.y;
        Serial.print("i :");
        Serial.println(i);
        if(i>270 && i<500)
        {
          myservo.write(0);
          Serial.println("Servo 0");
        }
        else if(i>550)
        {
           myservo.write(180);
           Serial.println("Servo 180");
        }
        else
        {
          myservo.write(90);
          Serial.println("Servo 90");
        }
        
        if (p.y>270 && p.y<450) {
            my_lcd.Set_Text_colour(YELLOW);
            my_lcd.Set_Text_Size(3);
            my_lcd.Print_String("/", CENTER,55);
        }
        if (p.y>450 && p.y<600) {
            my_lcd.Set_Text_colour(YELLOW);
            my_lcd.Set_Text_Size(3);
            my_lcd.Print_String("|", CENTER,55);
         }
        if (p.y>600 && p.y<760) {
            my_lcd.Set_Text_colour(YELLOW);
            my_lcd.Set_Text_Size(3);
            my_lcd.Print_String("\\", CENTER,55);
         }
      }
      if ((p.y>360 && p.y<690) && (p.x>260 && p.x<400)) //Touch Button INIT
      {
         myservo.write(90);// move servos to center position -> 90°
         Clear_Button(201,46,79,44, BLACK); //(x debut, y debut, x fin, y fin, Back color of the button) my_lcd.Get_Display_Width()-150, my_lcd.Get_Display_Height()

         my_lcd.Set_Text_colour(YELLOW);
         my_lcd.Set_Text_Size(2);
         my_lcd.Print_String("Init", CENTER,60);

      }
      if ((p.y>110 && p.y<180) && (p.x>170 && p.x<270)) //BACK Button
      {
        MainMenu();
      }
     }
    }
}
