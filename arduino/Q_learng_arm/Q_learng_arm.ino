#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <WiFiSerial.h>

WiFiSerial wSerial(9600);
VL53L0X sensor;

float distance;
float Q_table[16][4];
float epsilon = 0.1;
float alpha = 1;
float gamma = 0.9;
int episode = 5;
int goal = 15;
int s1_angle_ini=0;
int s2_angle_ini=0;
int s1_step = 10;
int s2_step = 40;
int s1_angle = 15;
int s2_angle = 20;
int pre_s1_angle;
int pre_s2_angle; 
int state,next_state;
float pre_dist,dist,diff;

Servo s1,s2;

int state_action[16][3]={ 
//总共有16（32会不会更好？）中状态，每种状态下的可能动作
//0表示马达1角度加大（向下），1表示马达1角度减小（向上），2表示马达2向下，3表示马达2向上
//因为有些state下可能只有两种动作，所以-1是一个占位符，数组的原因。
  {0,2,-1},
  {2,3,-1}, 
  {2,3,-1}, 
  {3,-1,-1}, 
  {0,1,2},
  {2,3,-1}, 
  {2,3,-1}, 
  {3,3,3}, 
  {0,1,2}, 
  {2,3,3}, 
  {2,3,-1}, 
  {3,3,3}, 
  {1,2,-1}, 
  {2,3,-1}, 
  {2,3,-1}, 
  {1,3,-1}, 
  };


float calc_max_Q(int state){
  int value=-100;
  for(int i=0;i<4;i++){
    if(value < Q_table[state][i]){
      value = Q_table[state][i];
    }
  }
  return value;
}

//epsilon贪婪策略选择当前状态下的下一个action
int epsilon_greedy_policy(int state){
  int num;
  int action = -1;
  if (random(1)<epsilon){
    while(action == -1){
      num = random(3);
      action = state_action[state][num];
    }
  }else{
    action = calc_max_Q(state);
  }
  return action;
}

float step(int action){
  
  switch (action)
      {
        case 0:
          next_state = state + 4;
          s1_angle += s1_step;
          s2_angle = 0;
          break;
        case 1:
          next_state = state -4;
          s1_angle -= s1_step;
          s2_angle = 0;
          break;
        case 2:
          next_state = state + 1;
          s2_angle += s2_step;
          break;
        case 3:
          next_state = state -1;
          s2_angle -= s2_step;
          break;
        default:
          break;
      } 

      servo_move(s1,pre_s1_angle,s1_angle);
      servo_move(s2,pre_s2_angle,s2_angle);
      delay(150);
}
float get_reward(){
  //求动作后距离的变化，以变化值作为回报，向前正回报，向后负回报。
    float reward;
    dist = get_distance();
    diff = dist - pre_dist;
    if(abs(diff)<19){diff=0;}             //减少误差到底有没有必有呢？
    pre_dist = dist;
    reward = map(diff/10,-8,8,-10,10);
    return reward;
}

void train(){
  
  float reward;
  pre_dist = get_distance();
  s1_angle=s1_angle_ini;
  s2_angle=s2_angle_ini;
  pre_s1_angle = s1.read();
  pre_s2_angle = s2.read();
  
  for(int i=0;i<episode;i++){
    state = 0;
    bool over = false;
    s1_angle = 40;
    s2_angle =0;
    servo_move(s1,pre_s1_angle,s1_angle);
    servo_move(s2,pre_s2_angle,s2_angle);
    int action;
    while(state != goal && !over){
      pre_s1_angle = s1_angle;
      pre_s2_angle = s2_angle;

      action = epsilon_greedy_policy(state);//根据epsilon随机策略选择一个动作

      step(action); //执行选定的动作,改变状态,获取回报
      reward = get_reward();

      Serial.print(" episode=");Serial.print(i);
      Serial.print(" state=");Serial.print(state);
      Serial.print(" action=");Serial.print(action);
      Serial.print(" s1_angle=");Serial.print(s1_angle);
      Serial.print(" s2_angle=");Serial.println(s2_angle);
      Serial.print(" reward=");Serial.println(reward);

      //根据Q_learning公式，更新Q_table并转移到下一个转台
      Q_table[state][action] += alpha * (reward + gamma * calc_max_Q(next_state) - Q_table[state][action] );
      state =next_state;
       
      //if(reward != 0)over = true; //如果前进或者后退了，其实这一幕探索可以结束了，不用等到最后一个state。
    }

    show_table(16,4);
  }
  servo_move(s1, pre_s1_angle, 20);
  servo_move(s2, pre_s2_angle, 0);
}


void show_table(int row,int col){
  Serial.println("Q_Table:");
  for(int i=0;i<row;i++){
    for(int j=0;j<col;j++){
      Serial.print(Q_table[i][j]);
      Serial.print("  ");
    }
    Serial.println(" ");
    if(i%4 == 0)delay(300);
    
  }
}


float get_distance(){
  int average = 0;
  int count = 5;
  for(int i=0;i<count;i++){
    distance = sensor.readRangeContinuousMillimeters();
    average = average + distance;
    delay(50);
  }
  average = average/count;
  return average;
}


void servo_move(Servo servo,int start_angle,int end_angle){
  if(start_angle<end_angle){
    for(int i=start_angle;i<=end_angle;i=i+2){
      servo.write(i);
      delay(20);
    }
  }else{
    for(int i=start_angle;i>=end_angle;i=i-2){
      servo.write(i);
      delay(15);
    }
  }
}

void predict(int count)
{
  float tmp_value;
  int tmp_a;
  for(int i=0;i<count;i++){
    state=0;
    s1_angle = 40;
    s2_angle = 0;
    servo_move(s1,pre_s1_angle,s1_angle);
    servo_move(s2,pre_s2_angle,s2_angle);
    while(state != goal){
      pre_s1_angle = s1_angle;
      pre_s2_angle = s2_angle;
      
      tmp_value = -100;
      for(int j=0;j<4;j++)
      {
        if(tmp_value < Q_table[state][j]){
          tmp_value = Q_table[state][j];
          tmp_a = j;
        }
      }
      step(tmp_a);
      state=next_state;
    }

  }
}

void MessageEvent(String order,int paraOne,int paraTwo)
{
    Serial.println("get even");
    Serial.print("order:");
    Serial.println(order);
    if(order=="train")
    {
      if (paraOne>0)
      {
        episode=paraOne;
      }else
      {
        episode=1;
      }
      train();
      Serial.println("train finish!");
    }
    if(order=="predict")
    {
      if(paraOne>0){
        predict(paraOne);
      }else
      {
        predict(1);
      }
    }
    if(order=="step")
    {
      s1_step=paraOne;
      s2_step=paraTwo;
      Serial.print("step:");
      Serial.print(s1_step);
      Serial.print(" ");
      Serial.println(s2_step);
    }
    if(order=="init")
    {
      s1_angle_ini=paraOne;
      s2_angle_ini=paraTwo;
      Serial.print("init:");
      Serial.print(s1_angle_ini);
      Serial.print(" ");
      Serial.println(s2_angle_ini);
    }
    if(order=="clear")
    {
       memset(Q_table,0,sizeof(Q_table));
       Serial.println("clear Q_Table!");
    }
}

void setup() {
  memset(Q_table,0,sizeof(Q_table));
  Serial.begin(9600);
  s1.attach(9);
  s2.attach(6);
  s1.write(0);
  s2.write(0);
  randomSeed(analogRead(0));//随机种子
  delay(4000);//等待WIFI连接
  show_table(16,4);

  //vl350setup
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init()){
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
  //end vl350setup
  wSerial.Begin();
  wSerial.setMessageEvent(MessageEvent);

  //train();
}

void loop() {
  // put your main code here, to run repeatedly:
  wSerial.SerialEvent();
}
