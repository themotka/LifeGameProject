#include <SFML/Graphics.h>
#include <stdio.h>
#include  <stdbool.h>
#include <time.h>

#define WIDTH 1900
#define HEIGHT 1000

typedef struct //примерная структура существа
{
    bool isAlive;
    unsigned char type; // планктон/рыба1/рыба2
    bool isHungry;
    long long time; // время чего-то, придумаете

}creature;


const int cellSize = 20; //размер каждой клетки
//creature curStep[WIDTH/cellSize][HEIGHT/cellSize] = {0}; //клетки в этой итерации
//creature nextStep[WIDTH / cellSize][HEIGHT/cellSize] = {0}; //клетки в следующей итерации
//static sfColor colorPlanqton = sfColor_fromRGB(15, 255, 23);
//static sfColor colorFish1 = sfColor_fromRGB(15, 10, 255);
//static sfColor colorFish2 = sfColor_fromRGB(97, 0, 0);

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