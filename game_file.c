#include <SFML/Graphics.h>
#include <stdio.h>
#include  <stdbool.h>
#include <time.h>
#include <malloc.h>

#define WIDTH 1900
#define HEIGHT 1000

typedef struct //примерная структура существа
{
    int lifeExpactancy; // количество доступных итераций
    bool isAlive;
    unsigned char type; // планктон/рыба1/рыба2
    //bool isHungry;
    int speed;

}creature;

void copyCell(creature* src, creature* dist){
        dist->isAlive = src->isAlive;
        dist->lifeExpactancy = src->lifeExpactancy;
        dist->type = src->type;
        dist->speed = src->speed;
};

void freeCell(creature* cell){
    cell->isAlive = 0;
}

void replaceCell(creature* cell1, creature* cell2){
    creature* tmp = (creature*)malloc(sizeof(creature));
    copyCell(cell1,tmp);
    copyCell(cell2, cell1);
    copyCell(tmp,cell2);
    free(tmp);
}
const int cellSize = 20; //размер каждой клетки

creature*** createWorld(){
    time_t t;
    srand((unsigned) time(&t));
    int width = WIDTH / cellSize, height = HEIGHT / cellSize;
    creature*** world = (creature***) malloc(sizeof(creature**) * height);
    for (int i = 0;i < height; i++){
        world[i] = (creature**) malloc(sizeof(creature*) * width);
        for (int j = 0; j < width; j++){
            world[i][j] = (creature*) malloc(sizeof(creature));
            world[i][j]->type = 0;
            world[i][j]->isAlive = 0;
        }
    }
    return world;
}
void generateWorld(creature*** world){
    time_t t;
    srand((unsigned) time(&t));
    int maxCreatures = WIDTH * HEIGHT * 0.3 / cellSize / cellSize;
    int planktonNum = maxCreatures / 3, fishNum = maxCreatures / 3, hunterNum = maxCreatures / 3;
    for (int i = 0; i < planktonNum; i++){
        int x = rand() % (WIDTH / cellSize);
        int y = rand() % (HEIGHT / cellSize);
        while (world[y][x]->isAlive == true){
            x = rand() % (WIDTH / cellSize);
            y = rand() % (HEIGHT / cellSize);
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 1;
    }
    for (int i = 0; i < fishNum; i++){
        int x = rand() % (WIDTH / cellSize);
        int y = rand() % (HEIGHT / cellSize);
        while (world[y][x]->isAlive  == true){
            x = rand() % (WIDTH / cellSize);
            y = rand() % (HEIGHT / cellSize);
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 2;
    }
    for (int i = 0; i < hunterNum; i++){
        int x = rand() % (WIDTH / cellSize);
        int y = rand() % (HEIGHT / cellSize);
        while (world[y][x]->isAlive  == true){
            x = rand() % (WIDTH / cellSize);
            y = rand() % (HEIGHT / cellSize);
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 3;
    }
}

void printWorld(creature*** world){
    int n = HEIGHT / cellSize, m = WIDTH / cellSize;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++) {
            printf("%d", world[i][j]->type);
        }
        printf("\n");
    }
}
//creature curStep[WIDTH/cellSize][HEIGHT/cellSize] = {0}; //клетки в этой итерации
//creature nextStep[WIDTH / cellSize][HEIGHT/cellSize] = {0}; //клетки в следующей итерации
//static sfColor colorPlanqton = sfColor_fromRGB(15, 255, 23);
//static sfColor colorFish1 = sfColor_fromRGB(15, 10, 255);
//static sfColor colorFish2 = sfColor_fromRGB(97, 0, 0);
void movePlankton(creature*** world, int i, int j){
    time_t t;
    srand((unsigned) time(&t));

}
void initWorld(sfRenderWindow *window, sfEvent event){
    sfRectangleShape* cell = sfRectangleShape_create();
    sfVector2f size = {cellSize-2, cellSize-2};
    sfRectangleShape_setSize(cell, size);
    sfRectangleShape_setFillColor(cell,sfBlack);
    for(int i = 0; i < WIDTH/cellSize; i++){
        for(int j = 0; j < HEIGHT/cellSize; j++) {
            sfVector2f posCell = {i*cellSize, j* cellSize};
            sfRectangleShape_setPosition(cell, posCell);
            sfRenderWindow_drawRectangleShape(window, cell, NULL);
        }
    }
}


void gameCycle(sfRenderWindow *window, sfEvent event) {
    while (sfRenderWindow_isOpen(window)) { // тот же цикл отслеживания действий
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        sfRenderWindow_clear(window, sfWhite);
        initWorld(window, event);
        sfRenderWindow_display(window);
    }
}