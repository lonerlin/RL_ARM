#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

float distance;
float Q_table[16][4];
float epsilon = 0.1;
float alpha = 1;
float gamma = 0.9;
int episode = 10;
int goal = 15;
int s1_step = 20;
int s2_step = 45;
int s1_angle = 40;
int s2_angle = 0;
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
  float reward;
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
          s2_angle += 45;
          break;
        case 3:
          next_state = state -1;
          s2_angle -= 45;
          break;
        default:
          break;
      } 

      servo_move(s1,pre_s1_angle,s1_angle);
      servo_move(s2,pre_s2_angle,s2_angle);

      //求动作后距离的变化，以变化值作为回报，向前正回报，向后负回报。
      dist = get_distance();
      diff = dist - pre_dist;
      if(abs(diff)<0.5){diff=0;}             //减少误差到底有没有必有呢？
      pre_dist = dist;
      reward = map(diff,-4,4,-10,10);

      return reward;
}

void train(){
  
  float reward;
  pre_dist = get_distance();
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
    while(state != 15 && !over){
      pre_s1_angle = s1_angle;
      pre_s2_angle = s2_angle;

      action = epsilon_greedy_policy(state);//根据epsilon随机策略选择一个动作

      reward = step(action); //执行选定的动作,改变状态,获取回报

      Serial.print(" episode=");Serial.print(i);
      Serial.print(" state=");Serial.print(state);
      Serial.print(" action=");Serial.print(action);
      Serial.print(" s1_angle=");Serial.print(s1_angle);
      Serial.print(" s2_angle=");Serial.println(s2_angle);
      Serial.print(" reward=");Serial.println(reward);

      //根据Q_learning公式，更新Q_table
      Q_table[state][action] += alpha * (reward + gamma * calc_max_Q(next_state) - Q_table[state][action] );
      
      if(reward != 0)over = true; //如果前进或者后退了，其实这一幕探索可以结束了，不用等到最后一个state。
    }

    show_table(16,4);
  }
}


void show_table(int row,int col){
  Serial.println("Q_Table:");
  for(int i=0;i<row;i++){
    for(int j=0;j<col;j++){
      Serial.print(Q_table[i][j]);
      Serial.print(" ");
    }
    Serial.println(" ");
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
    for(int i=start_angle;i<=end_angle;i++){
      servo.write(i);
      delay(10);
    }
  }else{
    for(int i=start_angle;i>=end_angle;i--){
      servo.write(i);
      delay(10);
    }
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
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
