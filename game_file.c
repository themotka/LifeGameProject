#include <SFML/Graphics.h>
#include <stdio.h>
#include  <stdbool.h>
#include <time.h>
#include <malloc.h>
#include <math.h>
#define WIDTH 1900
#define HEIGHT 1000
const int cellSize = 10; //размер каждой клетки
const  int n = HEIGHT / cellSize;
const int m = WIDTH / cellSize;
typedef struct //примерная структура существа
{
    int lifeExpactancy; // количество доступных итераций
    bool isAlive;
    unsigned char type; // планктон/рыба1/рыба2
    //bool isHungry;
    int speed;
    bool alreadyWalked;

}creature;

void copyCell(creature* src, creature* dist){
    dist->isAlive = src->isAlive;
    dist->lifeExpactancy = src->lifeExpactancy;
    dist->type = src->type;
    dist->speed = src->speed;
}

void freeCell(creature* cell){
    cell->isAlive = false;
    cell->type = 0;
    cell->lifeExpactancy = 0;
    cell->alreadyWalked = 0;
}

void replaceCell(creature* cell1, creature* cell2){
    creature* tmp = (creature*)malloc(sizeof(creature) * 2);
    copyCell(cell1,tmp);
    copyCell(cell2, cell1);
    copyCell(tmp,cell2);
    free(tmp);
}

creature*** createWorld(){
    //srand(time(NULL));
    int width = WIDTH / cellSize, height = HEIGHT / cellSize;
    creature*** world = (creature***) malloc(sizeof(creature**) * height);
    for (int i = 0;i < height; i++){
        world[i] = (creature**) malloc(sizeof(creature*) * width);
        for (int j = 0; j < width; j++){
            world[i][j] = (creature*) malloc(sizeof(creature));
            world[i][j]->type = 0;
            world[i][j]->isAlive = 0;
            world[i][j]->alreadyWalked = false;
        }
    }
    return world;
}
void generateWorld(creature*** world){
    srand(time(NULL));
    int maxCreatures = WIDTH * HEIGHT * 0.3 / cellSize / cellSize;
    int planktonNum = maxCreatures / 3, fishNum = maxCreatures / 3, hunterNum = maxCreatures / 3;
    for (int i = 0; i < planktonNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive == true){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 1;
        world[y][x]->lifeExpactancy = 2;
    }
    for (int i = 0; i < fishNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive  == true){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 2;
        world[y][x]->lifeExpactancy = 5;
    }
    for (int i = 0; i < hunterNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive  == true){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = 3;
        world[y][x]->lifeExpactancy = 0;

    }
}

void printWorld(creature*** world){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++) {
            printf("%d", world[i][j]->type);
        }
        printf("\n");
    }
}



//правильное взятие остатка
int per(int a, int b){
    while(a < 0){
        a += b;
    }
    return a % b;
}
//поиск оптимального побега от хищиников, если они есть
int findHunter(creature*** world, int i, int j){
    int allHunters[8];
    int numHunters = 0;
    for (int k = i - 2; k <= i + 2; k++){
        for (int g = j - 2; g <= j + 2; g++){
            int newI = per(k, n), newJ = per(g, m);
            if(world[newI][newJ]->isAlive == true && world[newI][newJ]->type == 3){
                allHunters[numHunters++] = newI*m + newJ;
            }
        }
    }
    if (numHunters == 0) return -1;
    int midI, midJ, sum;
    for (int k = 0; k < numHunters; k++){
        sum += allHunters[k];
    }
    sum /= numHunters;
    midI = i - sum / m;
    int deltaI = (i - sum / m) / abs( (i - sum / m));
    int deltaJ =  ( j - sum % m) / abs((j - sum % m));
    midI = per(i + deltaI, n);
    midJ = per(j + deltaJ, m);
    if (!world[midI][midJ]->isAlive || world[midI][midJ]->type == 1)
        return midI * m + midJ;
    else if (!world[per(midI - deltaI, n)][midJ]->isAlive || world[per(midI - deltaI, n)][midJ]->type == 1)
        return per(midI - deltaI, n) * m + midJ;
    else if (!world[midI][per(midJ - deltaJ, m)]->isAlive || world[midI][per(midJ - deltaJ, m)]->type == 1)
        return midI * m + per(midJ - deltaJ, m);
    return -2; // некуда идти, рыба окружена хочу фикса
}
//поиск еды для рыбы, возвращает индекс планктона, если она доступнаб иначе -1
int findPlankton(creature*** world, int i, int j){
    //srand(time(NULL));
    int allPlankton[8];
    int numP = 0;
    for (int k = i - 1; k <= i + 1; k++){
        for (int g = j - 1; g <= j + 1; g++){
            int newI = per(k, n), newJ = per(g, m);
            if(world[newI][newJ]->isAlive == true && world[newI][newJ]->type == 1){
                allPlankton[numP++] = newI*m + newJ;
            }
        }
    }
    return numP == 0 ? -1 : (allPlankton[rand() % numP]);
}

int getRandomPosition(creature *** world, int i, int j){
    //srand(time(NULL));
    int vertical = rand() % 3 - 1, horizontal = rand() % 3 - 1;
    int newI = per(i - vertical, n), newJ = per(j - horizontal, m);
    if (!world[newI][newJ]->isAlive)
        return newI * m + newJ;
    return -1;
}
int min(int a, int b){
    return a > b ? a : b;
}
//обнуляем alreadyWalked
void freeToWalk(creature*** world){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++)
            world[i][j]->alreadyWalked = false;
    }
}
//обновление мира
void update(creature*** world) {
    freeToWalk(world);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (world[i][j]->lifeExpactancy <= 0 || world[i][j]->alreadyWalked) {
                freeCell(world[i][j]);
                continue;
            }
            int type = world[i][j]->type;

            //srand(time(NULL));
            //printf("%d ", rand() % 100);
            switch (type) {
                case 1: {//для мха
                    int vertical = rand() % 3 - 1, horizontal = rand() % 3 - 1;
                    //движение мха
                    if (!world[per(i - vertical, n)][per(j - horizontal, m)]->isAlive && world[i][j]->lifeExpactancy > 1){
                        world[i][j]->lifeExpactancy--;
                        replaceCell(world[i][j], world[per(i - vertical, n)][per(j - horizontal, m)]);
                    }
                    //размножение
                    else if (!world[per(i - vertical, n)][per(j - horizontal, m)]->isAlive && world[i][j]->lifeExpactancy == 1) {
                        copyCell(world[i][j], world[per(i - vertical, n)][per(j - horizontal, m)]);
                        world[per(i - vertical, n)][per(j - horizontal, m)]->lifeExpactancy = 2;
                        world[i][j]->lifeExpactancy = 2;
                    }
                    break;
                }
               case 2: {//для рыб
                    int whatAboutPlankton = findPlankton(world, i, j), whatAboutHunter = findHunter(world, i, j);
                    int newI, newJ;
                    //если жить еще долго, то сначала убегаем от хищинков и при возможности едим
                    if (world[i][j]->lifeExpactancy > 2) {
                        if (whatAboutHunter == -1) {//рядом нет хищников
                            if (whatAboutPlankton == -1) {//еды рядом нет, двигаемся в случайную сторону
                                int newRandomPos = getRandomPosition(world, i, j);
                                if (newRandomPos != -1) {
                                    newI = newRandomPos / m;
                                    newJ = newRandomPos % m;
                                    world[i][j]->lifeExpactancy--;
                                    replaceCell(world[i][j], world[newI][newJ]);
                                    freeCell(world[i][j]);
                                }

                            } else {//занимаем место планктона и едим его
                                newI = whatAboutPlankton / m, newJ = whatAboutPlankton % m;
                                world[i][j]->lifeExpactancy--;
                                world[i][j]->lifeExpactancy = min(world[i][j]->lifeExpactancy + 2, 4);
                                replaceCell(world[i][j], world[newI][newJ]);
                                freeCell(world[i][j]);
                            }
                        } else if (whatAboutHunter > -2) {//рядом есть хищники, убегаем (а лучше едим в это время планктона)
                            newI = whatAboutHunter / m, newJ = whatAboutHunter % m;
                            world[i][j]->lifeExpactancy--;
                            if (world[newI][newJ]->isAlive) {
                                world[i][j]->lifeExpactancy = min(world[i][j]->lifeExpactancy + 2, 4);
                                replaceCell(world[i][j], world[newI][newJ]);
                                freeCell(world[i][j]);
                            } else replaceCell(world[i][j], world[newI][newJ]);
                        }
                    } else {//скоро умрем, надо есть
                        newI = whatAboutHunter / m, newJ = whatAboutHunter % m;
                        if (whatAboutHunter >= 0 && world[newI][newJ]->type == 1) {//если есть хищники, и можно убегая поесть, just do it
                            world[i][j]->lifeExpactancy--;
                            world[i][j]->lifeExpactancy = min(world[i][j]->lifeExpactancy + 2, 4);
                            replaceCell(world[i][j], world[newI][newJ]);
                            freeCell(world[i][j]);
                        } else if (whatAboutHunter == -1 && whatAboutPlankton != -1) {//нет хищников, еда есть, едим
                            world[i][j]->lifeExpactancy--;
                            newI = whatAboutPlankton / m, newJ = whatAboutPlankton % m;
                            world[i][j]->lifeExpactancy = min(world[i][j]->lifeExpactancy + 2, 4);
                            replaceCell(world[i][j], world[newI][newJ]);
                            freeCell(world[i][j]);
                        } else if (whatAboutHunter == -1) {//нет нихщников, нет еды, рандомно ходим
                            world[i][j]->lifeExpactancy--;
                            int newRandomPos = getRandomPosition(world, i, j);
                            if (newRandomPos != -1) {
                                newI = newRandomPos / m;
                                newJ = newRandomPos % m;
                                replaceCell(world[i][j], world[newI][newJ]);
                            }

                        }//иначе стоим на месте
                        else world[i][j]->lifeExpactancy--;// мб забалансит рыб

                    }
                    break;
                }
                case 3: {//для хищников
                    break;
                }
            }

            //старение
            world[i][j]->alreadyWalked = true;

        }
    }

}

//void initWorld(sfRenderWindow *window, sfEvent event){
//    sfRectangleShape* cell = sfRectangleShape_create();
//    sfVector2f size = {cellSize-2, cellSize-2};
//    sfRectangleShape_setSize(cell, size);
//    sfRectangleShape_setFillColor(cell,sfBlack);
//    for(int i = 0; i < WIDTH/cellSize; i++){
//        for(int j = 0; j < HEIGHT/cellSize; j++) {
//            sfVector2f posCell = {i*cellSize, j* cellSize};
//            sfRectangleShape_setPosition(cell, posCell);
//            sfRenderWindow_drawRectangleShape(window, cell, NULL);
//        }
//    }
//}


void initWorld(sfRenderWindow *window, sfEvent event, creature*** world){
    sfColor colorPlanqton = sfColor_fromRGB(15, 255, 23);
    sfColor colorFish1 = sfColor_fromRGB(15, 10, 255);
    sfColor colorFish2 = sfColor_fromRGB(97, 0, 0);
    sfRectangleShape* cell = sfRectangleShape_create();
    sfVector2f size = {cellSize-2, cellSize-2};
    sfRectangleShape_setSize(cell, size);
    sfRectangleShape_setFillColor(cell,sfBlack);
    for(int i = 0; i < WIDTH/cellSize; i++){
        for(int j = 0; j < HEIGHT/cellSize; j++) {
            int type;
            sfVector2f posCell = {i*cellSize, j* cellSize};
            sfRectangleShape_setPosition(cell, posCell);
            if (world[j][i]->isAlive == false) type = 0;
            else type = world[j][i]->type;
            //printf("%d ", world[j][i]->isAlive);
            switch (type){
                case 0:
                    sfRectangleShape_setFillColor(cell,sfWhite);
                    break;
                case 1:
                    sfRectangleShape_setFillColor(cell,colorPlanqton);
                    break;
                case 2:
                    sfRectangleShape_setFillColor(cell,colorFish1);
                    break;
                case 3:
                    sfRectangleShape_setFillColor(cell,colorFish2);
                    break;
            }
            sfRenderWindow_drawRectangleShape(window, cell, NULL);
        }
    }
}

void gameCycle(sfRenderWindow *window, sfEvent event){
    creature*** world = createWorld();
    generateWorld(world);
    while (sfRenderWindow_isOpen(window)) { // тот же цикл отслеживания действий
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        sfRenderWindow_clear(window, sfWhite);
        initWorld(window, event, world);
        sfRenderWindow_display(window);
        update(world);
    }
}