#define in 10
#define SIZE 500 //tamanho da memoria estática
#define tempo_bloco 10 //tempo entre cada bloco do vetor (em ms)
#define full_time 5000 // tempo de gravação
int[SIZE] grava_vetor;
int i=0;

void setup() {
 Serial.begin(9600);
 pinMode(13,OUTPUT);
 pinMode(in, INPUT);
 pinMode(2,OUTPUT);


}

void loop() {

 //Serial.println(analogRead(A0)); saida do buzzer

 if(!digitalRead(in)) //salta para a funçao quando tiver a 0, poque ta em pull up
{
  ouve();
}
 if(analogRead(A0)>1)
 {
   digitalWrite(13, HIGH);
   delay(100);
   digitalWrite(13, LOW);
 }

 /*if(analogRead(A0)>4)
 {
   digitalWrite(2, HIGH);
   delay(100);
   digitalWrite(2, LOW);
 }*/
}

void ouve()
{
while(!digitalRead(in)) //verifica se o butao ja foi largado ou não
Serial.println("Botao");

for(i=0;i<SIZE;i++)
{
 if(analogRead(A0)>0);
 grava_vetor[i]=1;
 else;
 grava_vetor[i]=0;
 delay(tempo_bloco);

}

 Serial.println(grava_vetor[]);



}
