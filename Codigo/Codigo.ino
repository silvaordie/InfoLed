#define THRESHOLD 25 //Valor do threshold default
#define DIF_MAX 1000 //Tempo de espera máximo entre toques
#define MAX_TOQUES 10 //Número máximo de toques
#define SENSOR A0 //Porta do sensor 
#define ERRO_MAX 40 //Desvio máximo admitido entre combinação guardada e ouvida
#define BOTAO_G 7 // Porta do botao de gravar
#define BOTAO_R 4 // Porta do botao de presença
#define DELAY 350 //Delay minimo entre roques

#define LED_1 10 //Azul
#define LED_2 11 //Amarelo
#define LED_3 12 //Verde
#define LED_4 13 //Vermelho

#define LED_ON 200 //ms de LED ligado
#define LED_OFF 3000 //ms de LED desligado

int LED1[4]={ 10 , 11, 12, 13}; //Portas dos LEDs


int sensorValue; //Leitura do sensor

unsigned long temp_time;
unsigned long diff=0;

unsigned long led_timer_1=0; //ms do estado dos LEDs (on/off)
bool on=false; //Estado dos LEDs


boolean predef = true; //Existem combinções pre-definidas
boolean learned = false; //Existem combinações aprendidas
boolean rec=false; //Se é necessário gravar uma combinação
boolean printed_cur = false; //Imprimiu a combinação ouvida
boolean stored_cur = false; //Guardou a combinação ouvida

int prenum = 0; //Magia Negra, NÃO MEXER NISTO SE NAO EXPLODE

int mems=3; //Número de comboinações guardadas
int mem[5][MAX_TOQUES] = {0}; //Combinações
int cur[] = {0,0,0,0,0,0,0,0,0,0}; //Combinação guardada

bool nots[5]={false}; //Notificações recebidas

int threshold; //Valor do threshold atual
unsigned long zero=0; //Valor do deslocamento da função de filtragem
int save; //Valor do pico da função de filtragem


void setup() {
  Serial.begin(9600);
  Serial.println(".....");
  pinMode(SENSOR, INPUT);
  pinMode(BOTAO_G, INPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  digitalWrite(LED_4, LOW);

  //Combinações predefinidas
  mem[0][0]=500 ;
  mem[1][0]=500 ;  
  mem[1][1]=500 ;
  mem[2][0]=500 ;
  mem[2][1]=500 ;
  mem[2][2]=500 ;
  
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
      predef = false;
      mems=0;
      mem[1][1]=0 ;
      mem[0][0]=0 ;
      mem[1][0]=0 ;  
      mem[1][1]=0 ;
      mem[2][0]=0 ;
      mem[2][1]=0 ;
      mem[2][2]=0 ;
      
      for(int i=0; i<mems; i++)
          nots[i]=false; 
      
      on=false;
    }

    if(digitalRead(BOTAO_R))
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

    if(sensorValue > threshold && (learned || rec || predef)) 
    { //if a knock occurred

      //Ajuste do threshold
      save = sensorValue;
      zero= millis();

      if(prenum > 0) 
      {
        diff = millis() - temp_time; //time since last knock
        if(!learned && rec) 
        { //don't save first time of knock sequence
          learn();
        }
        //record cur
        if( (learned || predef) && diff < DIF_MAX) 
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

//Rotina de LEDs que temporiza o ligar/desligar dos LEDs sem interromper o flow do código
void update_leds()
{
  if(!on && (millis()-led_timer_1) > LED_OFF)
  {
    for(int i=0; i<mems && i<4; i++)
    {
      if (nots[i])
      {
        digitalWrite(LED1[i], HIGH);
      }
    }
    on=true; 
    led_timer_1=millis();

  }

  if(on && (millis()-led_timer_1) > LED_ON)
  {
    for(int i=0; i<mems && i<4; i++)
    {
        digitalWrite(LED1[i], LOW);
    }
    on=false; 
    led_timer_1=millis();
  }
}

//Grava a diferença entre batimentos ouvidos(Tentativa)
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

//Grava a diferença entre batimentos ouvidos(Gravação)
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

//Compara a combinação ouvida com a gravada
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

//Função que realiza a filtragem temporal
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
