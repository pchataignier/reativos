#define MAX_TIMER 2
#define MAX_PINS 5

boolean listento[MAX_PINS];
int state[MAX_PINS];
long timer_start[MAX_TIMER];
long timer_dur[MAX_TIMER];
int contador_but = 0;
int contador_timer = 0;
int current_val;

/* Funções de registro: */

void button_listen (int pin) {  // “pin” (0 a 13) passado deve gerar notificações
   listento[pin] = true;
   pinMode(pin, INPUT);
   state[pin] = digitalRead(pin);
}

void timer_set (int timer_num, long ms) {  // timer "timer_num" (0 ou 1) deve expirar após “ms” milisegundos
   timer_start[timer_num] = millis();
   timer_dur[timer_num] = ms;
}

/* Callbacks */

void button_changed (int pin, int v);  // notifica que “pin” mudou para “v”
void timer_expired (int timer_num);    // notifica que o timer expirou

/* Programa principal: */

void setup () {
	// inicialização da API
	int i;
    for(i = 0; i < MAX_TIMER - 1 ; i++){
		timer_start[i] = -1;
		timer_dur[i] = -1;
	}
	for(i = 0; i < MAX_PINS - 1; i++){
		listento[i] = false;
		state[i] = -1;
	}
	
    user_init(); // inicialização do usuário
}

void loop () {
	
	// detecta eventos de butao
	if(listento[contador_but] == true){
	
		current_val = digitalRead(contador_but);
		delayMicroseconds(20);
		
		if(current_val != state[contador_but]){
			state[contador_but] = current_val;
			button_changed(contador_but, current_val);	// notifica o usuário
		}
	}
    contador_but++;
	if(contador_but > MAX_PINS - 1){contador_but = 0;}
	
	// detecta eventos de timer
	if(timer_start[contador_timer] != -1 && timer_dur[contador_timer] != -1 && millis() - timer_start[contador_timer] >= timer_dur[contador_timer]){
		timer_start[contador_timer] = -1;
		timer_dur[contador_timer] = -1;
		timer_expired(contador_timer);		// notifica o usuário
	}
	contador_timer++;
	if(contador_timer > MAX_TIMER - 1){contador_timer = 0;}

}