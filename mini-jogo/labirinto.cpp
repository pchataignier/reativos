#include <SDL.h>
#include <assert.h>
#include <stdio.h>

#define UNIT 60
#define TICK 2000
#define WALL UNIT/10
#define OFFSET UNIT/2
#define WINDOW_W 15*UNIT
#define WINDOW_H 11*UNIT
#define MAX_WALLS 310
#define MAX_GATES 30
#define MAX_STALKERS 5
#define N_BOUNDS 7
#define PL_SPACE UNIT/6
#define PL_SIDE 2*UNIT/3
#define X_START PL_SPACE
#define Y_START 5*UNIT+OFFSET+PL_SPACE
#define MAX_LEVELS 2

bool win = false;
SDL_Rect player = {0,0,0,0};
SDL_Rect walls[MAX_WALLS];
bool wall_hit[MAX_WALLS];
SDL_Rect gates[MAX_GATES];
bool gate_hit[MAX_GATES];
bool active[MAX_GATES];
int gate_time[MAX_GATES];
unsigned long int last_change[MAX_GATES];
SDL_Rect bounds[] = {{0, 0, WINDOW_W, OFFSET}, {0, OFFSET, UNIT, 5*UNIT}, {0, 6*UNIT + OFFSET, UNIT, 5*UNIT}, {0, WINDOW_H - OFFSET, WINDOW_W, OFFSET}, {WINDOW_W - UNIT, OFFSET, UNIT, 5*UNIT}, {WINDOW_W - UNIT, 6*UNIT + OFFSET, UNIT, 5*UNIT}, {0, 5 * UNIT + OFFSET, PL_SPACE/5, UNIT}};
int n_walls, n_gates, n_stalkers, stalker_break;
SDL_Rect goal = {WINDOW_W - PL_SPACE, 5 * UNIT + OFFSET, PL_SPACE, UNIT};
SDL_Rect stalker[MAX_STALKERS];
bool stalker_hit[MAX_STALKERS];
bool stalker_going[MAX_STALKERS];
int stalker_min[MAX_STALKERS];
int stalker_max[MAX_STALKERS];
int stalker_pos[MAX_STALKERS];
unsigned long int stalker_change;
int deaths;


void check_collision(){
	int i;

	//Walls
	for(i = 0; i < n_walls; i++){
		if(SDL_HasIntersection(&player, &walls[i])){
			wall_hit[i] = true;
			player.x = X_START;
			player.y = Y_START;
			deaths++;
		}
	}

	//Gates
	for(i = 0; i < n_gates; i++){
		if(active[i] && SDL_HasIntersection(&player, &gates[i])){
			gate_hit[i] = true;
			player.x = X_START;
			player.y = Y_START;
			deaths++;
		}
	}

	//Stalkers
	for(i = 0; i < n_stalkers; i++){
		if(SDL_HasIntersection(&player, &stalker[i])){
			stalker_hit[i] = true;
			player.x = X_START;
			player.y = Y_START;
			deaths++;
		}
	}

	//Bounds
	for(i = 0; i < N_BOUNDS; i++){
		if(SDL_HasIntersection(&player, &bounds[i])){
			player.x = X_START;
			player.y = Y_START;
			deaths++;
		}
	}

	//Goal
	if(SDL_HasIntersection(&player, &goal)){
		win = true;
	}
}

void update(SDL_Renderer* renderer){
	int i;

	if(win){SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0x00);}
	else{SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0x00);}

    SDL_RenderFillRect(renderer, NULL);

	//Bounds
	SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x00,0x00);
	for(i = 0; i < N_BOUNDS; i++){
		SDL_RenderFillRect(renderer, &bounds[i]);
	}

	//Walls
	for(i = 0; i < n_walls; i++){
		if(wall_hit[i]){
			SDL_SetRenderDrawColor(renderer, 0x99,0x00,0x00,0x00);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0x00);
		}
		SDL_RenderFillRect(renderer, &walls[i]);
	}

	//Gates
	for(i = 0; i < n_gates; i++){
		unsigned long int aux = SDL_GetTicks();
		if(aux - last_change[i] > gate_time[i]){
			active[i] = !active[i];
			last_change[i] = aux;
		}
		if(gate_hit[i] && active[i]){
			SDL_SetRenderDrawColor(renderer, 0x99,0x00,0x00,0x00);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0x00);
		}
		SDL_RenderFillRect(renderer, &gates[i]);
	}
	
	//Stalkers
	unsigned long int aux = SDL_GetTicks();
	if(aux - stalker_change > TICK){
		stalker_change = aux;
		for(i = 0; i < n_stalkers; i++){
			if(stalker_going[i]){
				stalker_pos[i]++;
			}
			else{
				stalker_pos[i]--;
			}
			if(stalker_pos[i] < stalker_min[i] || stalker_pos[i] > stalker_max[i]){
				stalker_going[i] = !stalker_going[i];
				if(stalker_going[i]){
					stalker_pos[i]+=2;
				}
				else{
					stalker_pos[i]-=2;
				}
			}
			if(i < stalker_break){
				stalker[i].x = stalker_pos[i] * UNIT + PL_SPACE;
			}
			else{
				stalker[i].y = stalker_pos[i] * UNIT + PL_SPACE;
			}
		}
	}
	for(i = 0; i < n_stalkers; i++){
		if(stalker_hit[i]){
			SDL_SetRenderDrawColor(renderer, 0x99,0x00,0x00,0x00);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 0xC0,0xC0,0xC0,0x00);
		}
		SDL_RenderFillRect(renderer, &stalker[i]);
	}
	

	//Player

	SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0x00,0x00);
	SDL_RenderFillRect(renderer, &player);

	//Fim
	SDL_RenderPresent(renderer);
}

void load_level(char num_level_1, char num_level_2){

	int i;
	int w_hor, w_ver, g_hor, g_ver, s_hor, s_ver;
	SDL_Rect r;
	char level[] = "level__.txt";
	level[5] = num_level_1;
	level[6] = num_level_2;
	FILE* arq = fopen(level, "r");
	assert(level != NULL);

	fscanf(arq, "%d %d %d %d %d %d", &w_hor, &w_ver, &g_hor, &g_ver, &s_hor, &s_ver);
	n_walls = w_hor + w_ver;
	n_gates = g_hor + g_ver;
	n_stalkers = s_hor + s_ver;
	stalker_break = s_hor;

	//Player

	player.x = X_START;
	player.y = Y_START;
	player.w = PL_SIDE;
	player.h = PL_SIDE;

	//Paredes
	//Paredes Horizontais
	for(i = 0; i < w_hor; i++){
		int x, y;
		fscanf(arq, "%d %d", &x, &y);

		walls[i].x = x * UNIT;
		walls[i].y = y * UNIT + OFFSET;
		walls[i].w = UNIT;
		walls[i].h = WALL;
		wall_hit[i] = false;
	}
	//Paredes Verticais
	for(i = w_hor; i < n_walls; i++){
		int x, y;
		fscanf(arq, "%d %d", &x, &y);

		walls[i].x = x * UNIT;
		walls[i].y = y * UNIT + OFFSET;
		walls[i].w = WALL;
		walls[i].h = UNIT;
		wall_hit[i] = false;
	}

	//Gates
	unsigned long int start = SDL_GetTicks();
	//Gates Horizontais
	for(i = 0; i < g_hor; i++){
		int x, y, t;
		fscanf(arq, "%d %d %d", &x, &y, &t);

		gates[i].x = x * UNIT;
		gates[i].y = y * UNIT + OFFSET;
		gates[i].w = UNIT;
		gates[i].h = WALL;
		gate_time[i] = t * TICK;
		active[i] = true;
		last_change[i] = start;
		gate_hit[i] = false;
	}
	//Gates Verticais
	for(i = g_hor; i < n_gates; i++){
		int x, y, t;
		fscanf(arq, "%d %d %d", &x, &y, &t);

		gates[i].x = x * UNIT;
		gates[i].y = y * UNIT + OFFSET;
		gates[i].w = WALL;
		gates[i].h = UNIT;
		gate_time[i] = t * TICK;
		active[i] = SDL_TRUE;
		last_change[i] = start;
		gate_hit[i] = false;
	}

	//Stalkers
	//Stalkers Horizontais
	for(i = 0; i < s_hor; i++){
		int y, min, max;
		fscanf(arq, "%d %d %d", &y, &min, &max);

		stalker[i].x = (min * UNIT) + PL_SPACE;
		stalker[i].y = (y * UNIT) + OFFSET + PL_SPACE;
		stalker[i].w = PL_SIDE;
		stalker[i].h = PL_SIDE;
		stalker_hit[i] = false;
		stalker_going[i] = true;
		stalker_change = start;
		stalker_min[i] = min;
		stalker_max[i] = max;
		stalker_pos[i] = min;
	}
	//Stalkers Verticais
	for(i = s_hor; i < n_stalkers; i++){
		int x, min, max;
		fscanf(arq, "%d %d %d", &x, &min, &max);

		stalker[i].x = (x * UNIT) + PL_SPACE;
		stalker[i].y = (min * UNIT) + OFFSET + PL_SPACE;
		stalker[i].w = PL_SIDE;
		stalker[i].h = PL_SIDE;
		stalker_hit[i] = false;
		stalker_going[i] = true;
		stalker_change = start;
		stalker_min[i] = min;
		stalker_max[i] = max;
		stalker_pos[i] = min;
	}


	//Fim
	fclose(arq);
}

int main (int argc, char* args[])
{
    /* INITIALIZATION */

    int err = SDL_Init(SDL_INIT_EVERYTHING);
    assert(err == 0);

	SDL_Window* window = SDL_CreateWindow("Labirinto Invisivel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    assert(window != NULL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    assert(renderer != NULL);

	char num_level_1 = '0';
	char num_level_2 = '0';
	int i = MAX_LEVELS;

    /* EXECUTION */
	
	SDL_Event e;

	while(i--){
		load_level(num_level_1, num_level_2);
		win = false;
		deaths = 0;
		while (!win)
		{
			if(SDL_PollEvent(&e) != 0){
				if (e.type == SDL_QUIT) {
				    break;
				}
				else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_UP:
							player.y -= 10;
							break;
						case SDLK_DOWN:
							player.y += 10;
							break;
						case SDLK_LEFT:
							player.x -= 10;
							break;
						case SDLK_RIGHT:
							player.x += 10;
							break;
					}	
				}
			}

			check_collision();
			update(renderer);
			if(win){
				num_level_2++;
				if(num_level_2 > '9'){
					num_level_1++;
					num_level_2 = '0';
					if(num_level_1 > '9'){num_level_1 = '0'; num_level_2 = '0';}
				}
				SDL_Delay(5000);
			}
		}
	}

    /* FINALIZATION */

    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}