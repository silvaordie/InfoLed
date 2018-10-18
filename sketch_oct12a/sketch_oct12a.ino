#define BOTAO 10 // Porta do sensor
#define SIZE 500 //tamanho da memoria estática (número de amostras)
#define DELAY_A 10 //tempo entre cada bloco do vetor (tempo de amostragem em ms)
#define TEMPO_MAX 5000 // tempo de maximo de gravação 
#define THRESHOLD 10
#define SENSOR A0
#define MARGEM 10

int grava_vetor[SIZE]; //  Vetor com a combinação sonora
bool combinacao;


int com=0;
void setup() 
{
     Serial.begin(9600);
     pinMode(BOTAO, INPUT);
}

void loop() 
{
  
    //Serial.println(analogRead(A0)); saida do buzzer
    if(!digitalRead(BOTAO)) //Se o bOtão for premido
    {
        ouve();
    }

    if(!digitalRead(7))
      playback();



 /*if(analogRead(A0)>4)
 {
   digitalWrite(2, HIGH);
   delay(100);
   digitalWrite(2, LOW);
 }*/
}

void playback()
{
  
  for(int j=0; j<SIZE; j++)
  {
    if(grava_vetor[j] == 1)
      digitalWrite(12, HIGH);
      
    delay(DELAY_A);
    digitalWrite(12, LOW);   
  }

}

void ouve()
{      
      while( !digitalRead(BOTAO) ); //verifica se o botao ja foi largado ou não
      Serial.println("Botao");   
         
      while( analogRead(SENSOR) < THRESHOLD ); //comaça ao primeiro barulho
      Serial.println("teste");  
            
      for(int i=0 ; i<SIZE && digitalRead(BOTAO) ; i++)
      {
         if( analogRead(SENSOR) > THRESHOLD )
             grava_vetor[i]=1;
         else
            grava_vetor[i]=0;
         delay(DELAY_A);
      }
      int j=0;
      
      for(int j=0; j<SIZE; j++)
       Serial.print(grava_vetor[j]);
       
}
