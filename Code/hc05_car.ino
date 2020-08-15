//#include <SoftwareSerial.h>  
#include <Servo.h>  // 舵机库

// D3、D5、D6、D9、D10、D11 可以做PWM输出用
// nano的D5或D6口，输出pwm频率960Hz，而3,9,10,11口输出的pwm频率为480Hz
#define IN1  2 // 定义uno的pin 5 向 input1 输出 
#define IN2  4 // 定义uno的pin 6 向 input2 输出
#define IN3  7 // 定义uno的pin 9 向 input3 输出
#define IN4  8 // 定义uno的pin 10 向 input4 输出
#define ENA  3 // pwm电机调速
#define ENB  10 // pwm电机调速
/*
  Arduino Nano 使用pwm调节4线风扇转速
  D5接风扇的PWM，D2接风扇的测速端口，a0口接一个可调电阻，通过改变电阻来调节风扇转速
*/
#define fan_pin 5 // 定义风扇pwm输出io
#define fan_speed_pin  12   //测速端口
//用电位器调节转速
#define Pot A0         //电位器输入引脚
int PotBuffer = 0;     //电位器读取数据缓存
int fan_PWM = 0;
unsigned long duration = 0;

//SoftwareSerial BT(10, 11); // 接收, 传送，程序中10为RX需要接hc-05的TXD,11为TX，需要接hc-05的RX。
Servo myservo;
int pos = 0;//角度存储变量
int rate = 127;//占空比50%
/*函数声明
void Forward();
void Back();
void Turn_left();
void Turn_right();
void Speed_up();
void Slow_down();
*/
void setup() { 

  //BT.begin(9600);
  Serial.begin(9600);  // HC-05
  Serial.println(" BT is ready! ");

  myservo.attach(9);// 舵机控制线连接数字9
  // 数字引脚 D3、D5、D6、D9、D10、D11 可以做PWM输出用 D10到D13，可以做SPI总线用
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT); // PWM
  pinMode(4, OUTPUT); 
  pinMode(5, OUTPUT); // PWM
  pinMode(6, OUTPUT);// PWM
  pinMode(7, OUTPUT); 
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);// PWM
  pinMode(10, OUTPUT);// PWM
  pinMode(11, OUTPUT);// PWM
  pinMode(12, INPUT);  
  pinMode(13, OUTPUT);// led灯
}

void loop() {
  char BTval;// 蓝牙接收的数据 一个字节 字符 
  
  if(Serial.available()) {
    BTval = Serial.read();
    Serial.print(BTval);

    switch(BTval) {
      case '1':
        // 返回到手机调试程序上
        Serial.write(" Serial--13--high\n");
        digitalWrite(13, HIGH);
        break;
      case '0':
        Serial.write(" Serial--13--low\n");
        digitalWrite(13, LOW);
        break;
      case 'W':// 前进
        Serial.println(" FORWARD "); //输出状态
        Forward();
        break;  
      case 'S': // 后退
        Serial.println(" BACKWARD "); // 输出状态
        Back();
        break;
      case 'A': // 左转
        Serial.println(" TURN  LEFT "); // 输出状态
        Turn_left();
        break;    
      case 'D': // 右转
        Serial.println(" TURN  RIGHT "); // 输出状态
        Turn_right();
        break;
      case 'u':// 加舵机角度  加速
        Speed_up();
        pos++;
        break;
      case 'd':// 减舵机角度  减速
        Speed_down();
        pos--;
        break;  
  /*    case 'P':// stop停下 /制动
        ENA=0；
        ENB=0;   */
      default:
        Serial.println(" STOP "); // 输出状态
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);

        myservo.write(90);
        delay(5);
        break;
      }
  }         
}

void Forward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // PWM信号频率大约490Hz,pin写入端口，value占空比0-255
  // analogWrite(255)表示100%占空比（常开），
  // analogWrite(127)占空比大约为50%（一半的时间）。
  analogWrite(ENA,rate);

  myservo.write(90);
  delay(5);
  }

void Back(){ 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  myservo.write(90);
  delay(5);
  }

void Turn_left(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  myservo.write(150);
  delay(5);
  }

void Turn_right(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  myservo.write(45);
  delay(5);
  }

void Speed_up(){
  if(rate >= 0 && rate <= 255){
    rate += 2;
    }
  }

void Speed_down(){
  if(rate >= 0 && rate <= 255){
    rate -= 2;
    }  
  }

void fan(){
  PotBuffer = analogRead(Pot);                 //读取AD值
  fan_PWM = map(PotBuffer, 0, 1023, 0, 255); //将AD值映射为PWM数值
  Serial.print("fan_PWM=");
  Serial.print(fan_PWM);
  //pwm输出，用可调电阻调节占空比0~255。Speed_PWM为0时风扇转速最高，大于235时风扇停止转动
  analogWrite(fan_pin, fan_PWM );

  duration = 0;
  if (fan_PWM < 230) {                    // 当风扇开始转动时，计算转速
    for (int i = 0; i < 5; i++) {           //高低电平变化5个周期（如果要精确的话，可以多取几次），取总计时间（微秒）
      duration += pulseIn(fan_speed_pin, HIGH);
      duration += pulseIn(fan_speed_pin, LOW);
    }
  } else {
    duration = 4294967295;          //如果风扇未转动，设置为unsigned long类型的最大值，让计算后风扇转速接近为0
  }
  duration = duration / 5;          //高低电平变化1次用时
  duration = 1000000 / duration;    // 1秒内可变化几次（即转化为风扇转速频率Hz）
  Serial.print("   f is ");
  Serial.print(duration);
  Serial.print(" HZ");
  duration = duration * 60;         //1分钟内可变化几次（即转换为风扇转速 转/分钟）
  Serial.print("  v is ");
  Serial.print(duration);
  Serial.println(" r/min");

  delay(500);
  }

  
