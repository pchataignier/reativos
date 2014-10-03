#define LED 13
#define GATE 2
#define TIMEOUT 10000

int state = 0;
unsigned int t;
unsigned int last_t = 0;

void setup(){
  pinMode(LED, OUTPUT);
  pinMode(GATE, INPUT);
  Serial.begin(9600);
  last_t = millis();
  state = digitalRead(GATE);
  digitalWrite(LED,state);
}

void loop(){
  int aux = digitalRead(GATE);
  t = millis();
  if(aux != state){
    state = aux;
    digitalWrite(LED,aux);
    if(aux == HIGH){
      float dt = (t - last_t)/1000.0;
      float vel = 0.471 / dt;
      vel = vel * 3.6;
      last_t = t;
      Serial.print(vel);
      Serial.println(" km/h");
    }
  }
  if(t - last_t > TIMEOUT){
    Serial.println("0.0 km/h");
  }
  
  //delay(100);
}
