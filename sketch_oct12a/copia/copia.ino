#define THRESHOLD 5
#define DIF_MAX 1000
#define MAX_TOQUES 10
#define SENSOR A0
#define ERRO_MAX 40
#define BOTAO_G 7
#define BOTAO_R 10
#define DELAY 350

#define LED_10 9
#define LED_11 8
#define LED_20 12
#define LED_21 11
#define LED_31 13

#define LED_ON 200
#define LED_OFF 3000

int LED1[3]={ 8, 11, 13};
int LED0[3]={ 9, 12, 4};

int sensorValue;
unsigned long temp_time;
unsigned long diff=0;
unsigned long led_timer_1=0;
unsigned long led_timer_2=0;
bool on=false;
int num=0;

boolean learned = false;
boolean rec=false;
boolean printed_mem = false;
boolean printed_cur = false;
boolean stored_cur = false;
boolean first_cur = false;
int prenum = 0;

int mems=0;
int mem[5][MAX_TOQUES] = {0}; //lock sequence
int cur[] = {0,0,0,0,0,0,0,0,0,0}; //current sequence
bool nots[5]={false};


long time;
int threshold;
unsigned long zero=0;
int save;


void setup() {
  Serial.begin(9600);
  Serial.println(".....");
  pinMode(SENSOR, INPUT);
  pinMode(BOTAO_G, INPUT);
  pinMode(LED_10, OUTPUT);
    pinMode(LED_11, OUTPUT);
      pinMode(LED_20, OUTPUT);
        pinMode(LED_21, OUTPUT);
          pinMode(LED_31, OUTPUT);
  digitalWrite(LED_10, LOW);
  digitalWrite(LED_11, LOW);
  digitalWrite(LED_20, LOW);
  digitalWrite(LED_21, LOW);
  digitalWrite(LED_31, LOW);
  
  
  threshold=THRESHOLD;
  Serial.println("Setup completo");
} 
 
void loop()
{
    sensorValue = analogRead(SENSOR);
    update_leds();
    threshold=g(zero, save, millis() );
  
    

    if(!digitalRead(BOTAO_G))
    {
      rec=true;
      learned=false;  
      prenum=0;    
    }

    if(!digitalRead(BOTAO_R))
    {
        for(int i=0; i<mems; i++)
          nots[i]=false; 
    }

    if(millis()-temp_time > DIF_MAX && mem[mems][0] != 0 && !learned) 
    { //at least one knock was done
      learned = true;
      mems++;
      rec=false;
      Serial.print("Combinação ");
      Serial.print(mems);
      Serial.println(" guardada");
      
    }
    if(stored_cur && millis()-zero > DIF_MAX)
    {
      checkValid();
      
      for(int i=0; i<MAX_TOQUES; i++)
        cur[i] = 0;

      printed_cur = false;
      stored_cur = false;
      Serial.print("Aguarde...");
      delay(1000);
      Serial.println("Pronto");
    }

    if(sensorValue > threshold && (learned || rec)) 
    { //if a knock occurred

      //Ajuste do threshold
      save = sensorValue;
      zero= millis();

      if(prenum > 0) 
      {
        diff = millis() - temp_time; //time since last knock
        if(!learned) 
        { //don't save first time of knock sequence
          learn();
        }
        //record cur
        if(learned && diff < DIF_MAX) 
        { //don't save first time of knock sequence
          record_curr();
        }
        temp_time = millis();
        delay(DELAY);
      }
      else 
      {
        prenum++;
        temp_time = millis();
        delay(DELAY);
      }
  } 
}

void update_leds()
{
  if(!on && (millis()-led_timer_1) > LED_OFF)
  {
    for(int i=0; i<mems && i<3; i++)
    {
      if (nots[i])
        digitalWrite(LED1[i], HIGH);
    }
    on=true; 
    led_timer_1=millis();
  }

  if(on && (millis()-led_timer_1) > LED_ON)
  {
    for(int i=0; i<mems && i<3; i++)
    {
        digitalWrite(LED1[i], LOW);
    }
    on=false; 
    led_timer_1=millis();
  }
}

void record_curr()
{
  for(int i=0; i<MAX_TOQUES; i++) 
  { //loop through
    if(cur[i] == 0) 
    { //if nothing is stored there
      cur[i] = diff; ///fix this for first time through???
       Serial.println(diff);
                   
        stored_cur = true;
                   
      break;
    }  
  }
}

void learn()
{
  for(int i=0; i<MAX_TOQUES; i++)
  { //loop through mem
    if(mem[mems][i] == 0) 
    { //if nothing is stored there
      mem[mems][i] = diff; ///fix this for first time through???
      Serial.println(diff);
      break;
    }
  }
}

void checkValid() 
{
  double error;
  bool certo=false;
  Serial.println("A comparar"); //knock lock
  for(int j=0; j<mems; j++)
  {
    for(int i=0; i<MAX_TOQUES; i++) 
    {
      error = (double) (cur[i]-mem[j][i])/mem[j][i];
      error = abs(error);
      error = 100 * error;
      
      if(error > ERRO_MAX || error < -ERRO_MAX ) 
      {///.4 for now
        Serial.println("Sem correspondência");
        break;
      }
      if(i==MAX_TOQUES-1) 
      {
        certo=true;
      }     
    }
    if (certo)
    {
      Serial.print("Reconhecido padrão ");
      Serial.println(j+1);
      nots[j]=true;
      break;       
    }
  }
}

int g(int zero, int thresh, int atual)
{
  int coord = atual-zero;

  if(coord <=DELAY)
  {
      return THRESHOLD + thresh;  
  }
  else  
    return THRESHOLD + thresh/(0.1*coord+DELAY );
}
