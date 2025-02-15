/* 该例程展示调用简洁初始化使用8个串口
* 每次构造SoftwareSerial时，程序会执行分配pio状态块和tx,rx的索引,
* 最大可以申请4个软件串口。
*注:
* rp2040 有2个状态机块(pio0 or pio1)，每个状态机块有4个状态机索引(0~3)
* 二个状态机索引对应一个软串口,一个用于接收，一个用于发送，所以一共可以
* 申请4个软件串口。
*/

#include <ICMakerSoftwareSerial.h>


#define TX_PIN 7
#define RX_PIN 8

//*下面两句共同占用一个状态块 0
//占用2个pio状态机 
SoftwareSerial SoftSerial= SoftwareSerial(TX_PIN, RX_PIN);
//占用2个pio状态机 
SoftwareSerial SoftSerial1 = SoftwareSerial(9, 10);

//*下面两句共同占用一个状态块 1
//占用2个pio状态机 
SoftwareSerial SoftSerial2 = SoftwareSerial(11, 12);
//占用2个pio状态机 
SoftwareSerial SoftSerial3 = SoftwareSerial(14, 15);

void setup()
{
    //如果使用USB CDC则默认串口映射到USB
  Serial.begin(115000); //默认串口映射到USB

  Serial1.begin(115200); //串口1 
  
  //Serial1.setPinout(4,5); 修改pin

  //软件串口
  SoftSerial.begin(115200);
  SoftSerial1.begin(115200);
  SoftSerial2.begin(115200);
  SoftSerial3.begin(115200);
}

void loop()
{
  Serial.printf("Serial\n");
  Serial1.print("Serial1\n"); 
  SoftSerial.print("SoftSerial\n"); 
  SoftSerial1.print("SoftSerial1\n"); 
  SoftSerial2.print("SoftSerial2\n"); 
 SoftSerial3.print("SoftSerial3\n"); 
  delay(400);
}