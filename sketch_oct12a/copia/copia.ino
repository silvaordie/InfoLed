#define THRESHOLD 5
#define DIF_MAX 1000
#define MAX_TOQUES 10
#define SENSOR A0
#define ERRO_MAX 40
#define BOTAO_G 10
#define DELAY 350

int sensorValue;
unsigned long temp_time;
unsigned long diff=0;
int num=0;

boolean learned = false;
boolean rec=false;
boolean printed_mem = false;
boolean printed_cur = false;
boolean stored_cur = false;
boolean first_cur = false;
int prenum = 0;

int mem[] = {0,0,0,0,0,0,0,0,0,0}; //lock sequence
int cur[] = {0,0,0,0,0,0,0,0,0,0}; //current sequence

long time;
int threshold;
unsigned long zero=0;
int save;


void setup() {
  Serial.begin(9600);
  Serial.println(".....");
  pinMode(SENSOR, INPUT);
  pinMode(BOTAO_G, INPUT);
  threshold=THRESHOLD;
  Serial.println("Setup complete");
} 
 
void loop()
{
    sensorValue = analogRead(SENSOR);
    threshold=g(zero, save, millis() );

    if(!digitalRead(BOTAO_G))
      rec=true;

    
    if(millis()-temp_time > DIF_MAX && mem[0] != 0 && !learned) 
    { //at least one knock was done
      learned = true;
      rec=false;
      Serial.println("**********************");
    }

    if(learned && !printed_mem)
    {
      Serial.println("Printing mem");
      printArray(mem);
      printed_mem = true;
      Serial.println("Unlock pattern recorded. Please wait...");
      Serial.println("ready to be unlocked");
    }

    if(stored_cur && millis()-zero > DIF_MAX)
    {
      Serial.println("Printing cur:");
      printArray(cur);
      printed_cur = true;
      checkValid();
      
      for(int i=0; i<MAX_TOQUES; i++)
        cur[i] = 0;
      
      
      printed_cur = false;
      stored_cur = false;
      Serial.println("Please wait...");
      delay(1000);
      Serial.println("ready to be unlocked");
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
            if(mem[i] == 0) 
            { //if nothing is stored there
              mem[i] = diff; ///fix this for first time through???
              Serial.println(diff);
              break;
            }
          }
}

void printArray(int ar[]) 
{
  Serial.println("\nPRINTING ARRAY[]");
  
  for(int i=0; i<MAX_TOQUES; i++) 
    Serial.println(ar[i]);

  Serial.println("\n");
}

void checkValid() 
{
  double error;
  Serial.println("CHECKING UNLOCK PATTERN"); //knock lock
  for(int i=0; i<MAX_TOQUES; i++) 
  {
    error = (double) (cur[i]-mem[i])/mem[i];
    error = abs(error);
    error = 100 * error;
    Serial.print("error="); Serial.println(error);
    Serial.print(cur[i]); Serial.print("\t"); Serial.print(mem[i]); Serial.print("\t");
    Serial.print((double) error); Serial.println("%");
    
    if(error > ERRO_MAX || error < -ERRO_MAX ) 
    {///.4 for now
      Serial.print(cur[i]); Serial.print(" !~ "); Serial.println(mem[i]);
      Serial.println("INCORRECT");
      break;
    }
    if(i==MAX_TOQUES-1) 
    {
      Serial.println("UNLOCKED!!!!!!!!!!!!!!");
    }     
  }
}

int resetArray() 
{
  int ar[MAX_TOQUES];
  
  for(int i=0; i<MAX_TOQUES; i++) 
    ar[i] = 0;
}

boolean checkSize() 
{
  int mem_length=0;
  int cur_length=0;
  for(int i=0; i<10; i++) 
  {
    if(mem[i]==0)
      break;
    mem_length++;
  }

  for(int i=0; i<10; i++) 
  {
    if(cur[i]==0)
      break;
    cur_length++;
  }

  if(mem_length != cur_length || mem_length==0)
    return false;
    
  return true;
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
