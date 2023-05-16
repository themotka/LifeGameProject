#include <SFML/Graphics.h>
#include <stdio.h>
#include  <stdbool.h>
#include <time.h>
#include <malloc.h>
#include<unistd.h>
#define WIDTH 1900
#define HEIGHT 970

#define LIFE_EXPECTANCY_PLANKTON 3
#define LIFE_EXPECTANCY_FISH 5
#define LIFE_EXPECTANCY_HUNTERS 6
enum CreatureType{
    Empty,
    Plankton,
    Fish,
    Hunter
};
const int cellSize = 10; //размер каждой клетки
const  int n = HEIGHT / cellSize;
const int m = WIDTH / cellSize;
const int FISH_IN_BEGINNING = n * m * 0.1;
typedef struct // структура существа
{
    int lifeExpactancy; // количество доступных итераций
    bool isAlive;
    enum CreatureType type; // планктон/рыба1/рыба2
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
    cell->type = Empty;
    cell->lifeExpactancy = 0;
    cell->alreadyWalked = true;
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
            freeCell(world[i][j]);
        }
    }
    return world;
}
void generateWorld(creature*** world){
    srand(time(NULL));
    int maxCreatures = WIDTH * HEIGHT * 0.3 / cellSize / cellSize;
    int planktonNum = maxCreatures / 3, fishNum = maxCreatures / 3, hunterNum = maxCreatures / 10;
    for (int i = 0; i < planktonNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = Plankton;
        world[y][x]->lifeExpactancy = LIFE_EXPECTANCY_PLANKTON;
    }
    for (int i = 0; i < fishNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = Fish;
        world[y][x]->lifeExpactancy = LIFE_EXPECTANCY_FISH;
    }
    for (int i = 0; i < hunterNum; i++){
        int x = rand() % m;
        int y = rand() % n;
        while (world[y][x]->isAlive){
            x = rand() % m;
            y = rand() % n;
        }
        world[y][x]->isAlive = true;
        world[y][x]->type = Hunter;
        world[y][x]->lifeExpactancy = LIFE_EXPECTANCY_HUNTERS;

    }
}


int countCreatures(creature*** world, enum CreatureType type){
    int count = 0;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++)
            if (world[i][j]->isAlive && world[i][j]->type == type) count++;
    }
    return count;
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
            if(world[newI][newJ]->isAlive == true && world[newI][newJ]->type == Hunter){
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
    if (!world[midI][midJ]->isAlive || world[midI][midJ]->type == Plankton)
        return midI * m + midJ;
    else if (!world[per(midI - deltaI, n)][midJ]->isAlive || world[per(midI - deltaI, n)][midJ]->type == Plankton)
        return per(midI - deltaI, n) * m + midJ;
    else if (!world[midI][per(midJ - deltaJ, m)]->isAlive || world[midI][per(midJ - deltaJ, m)]->type == Plankton)
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
            if(world[newI][newJ]->type == Plankton){
                allPlankton[numP++] = newI*m + newJ;
            }
        }
    }
    return numP == 0 ? -1 : (allPlankton[rand() % numP]);
}
//ищем рыбу для хищников, если ее нет, то возвращаем -1
int findFish(creature*** world, int i, int j){
    //srand(time(NULL));
    int allFish[25];
    int numFish = 0;
    for (int k = i - 2; k <= i + 2; k++){
        for (int g = j - 2; g <= j + 2; g++){
            int newI = per(k, n), newJ = per(g, m);
            if(world[newI][newJ]->type == Fish){
                allFish[numFish++] = newI*m + newJ;
            }
        }
    }
    return numFish == 0 ? -1 : (allFish[rand() % numFish]);
}

int getRandomPosition(int i, int j, int range){
    //srand(time(NULL));
    int vertical = rand() % (range * 2 + 1) - range, horizontal = rand() % (range * 2 + 1) - range;
    int newI = per(i - vertical, n), newJ = per(j - horizontal, m);
    return newI * m + newJ;
}
int min(int a, int b){
    return a < b ? a : b;
}
//обнуляем alreadyWalked
void freeToWalk(creature*** world){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++)
            world[i][j]->alreadyWalked = false;
    }
}
//обновление мира
void update(creature*** world, int countPlankton, int countFishes, int countHunters) {
    freeToWalk(world);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            creature* currentCreature = world[i][j];
            if (currentCreature->lifeExpactancy <= 0) {
                freeCell(currentCreature);
                continue;
            }
            if (currentCreature->alreadyWalked)
                continue;
            enum CreatureType type = currentCreature->type;
            currentCreature->alreadyWalked = true;
            switch (type) {
                case Plankton: {
                    int vertical = rand() % 3 - 1, horizontal = rand() % 3 - 1;
                    //движение мха
                    if (!world[per(i - vertical, n)][per(j - horizontal, m)]->isAlive && currentCreature->lifeExpactancy > 1){
                        currentCreature->lifeExpactancy--;
                        replaceCell(currentCreature, world[per(i - vertical, n)][per(j - horizontal, m)]);
                    }
                        //размножение
                    else if (!world[per(i - vertical, n)][per(j - horizontal, m)]->isAlive && currentCreature->lifeExpactancy == 1) {
                        copyCell(currentCreature, world[per(i - vertical, n)][per(j - horizontal, m)]);
                        world[per(i - vertical, n)][per(j - horizontal, m)]->lifeExpactancy = LIFE_EXPECTANCY_PLANKTON;
                        currentCreature->lifeExpactancy = LIFE_EXPECTANCY_PLANKTON;
                    }
                    break;
                }
                case Fish: {
                    int whatAboutPlankton = findPlankton(world, i, j), whatAboutHunter = findHunter(world, i, j);
                    int newI, newJ;
                    //размножение рыбы
                    bool shouldSpawn = (rand() % countFishes) < FISH_IN_BEGINNING * 0.4;
                    if (shouldSpawn){
                        int pos = getRandomPosition(i, j, 1);
                        pos = world[pos / m][pos % m]->isAlive ? whatAboutPlankton : pos;
                        if (pos >= 0) {
                            currentCreature->lifeExpactancy += world[pos / m][pos % m]->type == Plankton ? 1 : -1;
                            copyCell(currentCreature, world[pos / m][pos % m]);
                            //continue;
                        }
                    }
                    //если жить еще долго, то сначала убегаем от хищинков и при возможности едим
                    if (currentCreature->lifeExpactancy > 2) {
                        if (whatAboutHunter == -1) {//рядом нет хищников
                            if (whatAboutPlankton == -1) {//еды рядом нет, двигаемся в случайную сторону
                                int newRandomPos = getRandomPosition(i, j, 1);
                                newI = newRandomPos / m;
                                newJ = newRandomPos % m;
                                if (!world[newI][newJ]->isAlive) {
                                    currentCreature->lifeExpactancy--;
                                    replaceCell(currentCreature, world[newI][newJ]);
                                }

                            } else {//занимаем место планктона и едим его
                                newI = whatAboutPlankton / m, newJ = whatAboutPlankton % m;
                                currentCreature->lifeExpactancy--;
                                currentCreature->lifeExpactancy = min(currentCreature->lifeExpactancy + 2, LIFE_EXPECTANCY_FISH);
                                replaceCell(currentCreature, world[newI][newJ]);
                                freeCell(currentCreature);
                            }
                        } else if (whatAboutHunter > -2) {//рядом есть хищники, убегаем (а лучше едим в это время планктона)
                            newI = whatAboutHunter / m, newJ = whatAboutHunter % m;
                            currentCreature->lifeExpactancy--;
                            if (world[newI][newJ]->isAlive) {
                                currentCreature->lifeExpactancy = min(currentCreature->lifeExpactancy + 2, LIFE_EXPECTANCY_FISH);
                                replaceCell(currentCreature, world[newI][newJ]);
                                freeCell(currentCreature);
                            } else replaceCell(currentCreature, world[newI][newJ]);
                        }
                    } else {//скоро умрем, надо есть
                        newI = whatAboutHunter / m, newJ = whatAboutHunter % m;
                        if (whatAboutHunter >= 0 && world[newI][newJ]->type == Plankton) {//если есть хищники, и можно убегая поесть, just do it
                            currentCreature->lifeExpactancy--;
                            currentCreature->lifeExpactancy = min(currentCreature->lifeExpactancy + 2, LIFE_EXPECTANCY_FISH);
                            replaceCell(currentCreature, world[newI][newJ]);
                            freeCell(currentCreature);
                        } else if (whatAboutHunter == -1 && whatAboutPlankton != -1) {//нет хищников, еда есть, едим
                            currentCreature->lifeExpactancy--;
                            newI = whatAboutPlankton / m, newJ = whatAboutPlankton % m;
                            currentCreature->lifeExpactancy = min(currentCreature->lifeExpactancy + 2, LIFE_EXPECTANCY_FISH);
                            replaceCell(currentCreature, world[newI][newJ]);
                            freeCell(currentCreature);
                        } else if (whatAboutHunter == -1) {//нет нихщников, нет еды, рандомно ходим
                            currentCreature->lifeExpactancy--;
                            int newRandomPos = getRandomPosition(i, j, 1);
                            newI = newRandomPos / m;
                            newJ = newRandomPos % m;
                            if (!world[newI][newJ]->isAlive) {
                                replaceCell(currentCreature, world[newI][newJ]);
                            }

                        }//иначе стоим на месте
                        else currentCreature->lifeExpactancy--;// мб забалансит рыб

                    }
                    break;
                }
                case Hunter: {
                    //размножение
                    bool shouldSpawn = (rand() % countHunters) < FISH_IN_BEGINNING * 0.07;
                    if (shouldSpawn){
                        int pos = getRandomPosition(i, j, 1);
                        if (world[pos / m][pos % m]->type != Hunter) {
                            currentCreature->lifeExpactancy += world[pos / m][pos % m]->type == Fish ? 1 : 0;
                            copyCell(currentCreature, world[pos / m][pos % m]);
                            //continue;
                        }
                    }
                    //охота
                    //либо съедает рыбу, либо меняется местами с рандомной незанятой ячейкой в радиусе 2 клеток
                    int whatAboutFish = findFish(world, i, j);
                    if (whatAboutFish >= 0 && currentCreature->lifeExpactancy < 4){
                        currentCreature->lifeExpactancy = min(LIFE_EXPECTANCY_HUNTERS, currentCreature->lifeExpactancy + 2);
                        replaceCell(currentCreature, world[whatAboutFish / m][whatAboutFish % m]);
                        freeCell(currentCreature);
                    }
                    else{
                        int pos = getRandomPosition(i, j, 2);
                        currentCreature->lifeExpactancy--;
                        replaceCell(currentCreature, world[pos / m][pos % m]);
                    }
                    break;
                }
            }


        }
    }

}


void initWorld(sfRenderWindow *window, sfEvent event, creature*** world, int steps, sfFont *font){
    sfColor colorPlanqton = sfColor_fromRGB(15, 255, 23);
    sfColor colorFish1 = sfColor_fromRGB(15, 10, 255);
    sfColor colorFish2 = sfColor_fromRGB(97, 0, 0);
    sfRectangleShape* cell = sfRectangleShape_create();
    sfVector2f size = {cellSize-2, cellSize-2};
    sfRectangleShape_setSize(cell, size);
    sfRectangleShape_setFillColor(cell,sfBlack);

    char adcString[100];  // буфер, в которую запишем число
    sprintf(adcString,"%d", steps);
    char sSt[100]= "Steps: ";
    strcat(sSt, adcString);
    sfText *textStep = sfText_create();
    sfText_setColor(textStep, sfBlack);
    sfText_setString(textStep, sSt);
    sfText_setFont(textStep, font);
    sfVector2f pos1 = {100, HEIGHT-8};
    sfText_setPosition(textStep, pos1);
    sfText_setCharacterSize(textStep, 40);


    sfText *textPlankton = sfText_create();
    sfText_setColor(textPlankton, sfBlack);
    char sPl[100]= "Plankton: ";
    sprintf(adcString,"%d", countCreatures(world, Plankton));
    strcat(sPl, adcString);
    sfText_setString(textPlankton, sPl);
    sfText_setFont(textPlankton, font);
    sfVector2f pos2 = {570, HEIGHT-8};
    sfText_setPosition(textPlankton, pos2);
    sfText_setCharacterSize(textPlankton, 40);


    sfText *textFish = sfText_create();
    sfText_setColor(textFish, sfBlack);
    char sFi[100]= "Fish: ";
    sprintf(adcString,"%d", countCreatures(world, Fish));
    strcat(sFi, adcString);
    sfText_setString(textFish, sFi);
    sfText_setFont(textFish, font);
    sfVector2f pos3 = {1150, HEIGHT-8};
    sfText_setPosition(textFish, pos3);
    sfText_setCharacterSize(textFish, 40);


    sfText *textHunter= sfText_create();
    sfText_setColor(textHunter, sfBlack);
    char sHu[100]= "Hunter: ";
    sprintf(adcString,"%d", countCreatures(world, Hunter));
    strcat(sHu, adcString);
    sfText_setString(textHunter, sHu);
    sfText_setFont(textHunter, font);
    sfVector2f pos4 = {1600, HEIGHT-8};
    sfText_setPosition(textHunter, pos4);
    sfText_setCharacterSize(textHunter, 40);


    for(int i = 0; i < WIDTH/cellSize; i++){
        for(int j = 0; j < HEIGHT/cellSize; j++) {
            enum CreatureType type;
            sfVector2f posCell = {i*cellSize, j* cellSize};
            sfRectangleShape_setPosition(cell, posCell);
            if (world[j][i]->isAlive == false) type = Empty;
            else type = world[j][i]->type;
            //printf("%d ", world[j][i]->isAlive);
            switch (type){
                case Empty:
                    sfRectangleShape_setFillColor(cell,sfWhite);
                    break;
                case Plankton:
                    sfRectangleShape_setFillColor(cell,colorPlanqton);
                    break;
                case Fish:
                    sfRectangleShape_setFillColor(cell,colorFish1);
                    break;
                case Hunter:
                    sfRectangleShape_setFillColor(cell,colorFish2);
                    break;
            }
            sfRenderWindow_drawRectangleShape(window, cell, NULL);
        }
    }
    sfRenderWindow_drawText(window, textStep, NULL);
    sfRenderWindow_drawText(window, textPlankton, NULL);
    sfRenderWindow_drawText(window, textFish, NULL);
    sfRenderWindow_drawText(window, textHunter, NULL);

}

void gameCycle(sfRenderWindow *window, sfEvent event) {
    creature ***world = createWorld();
    generateWorld(world);
    int steps = 1, countPlankton, countFishes, countHunters;
    while (sfRenderWindow_isOpen(window)) { // тот же цикл отслеживания действий
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        sfRenderWindow_clear(window, sfWhite);
        sfFont *font = sfFont_createFromFile("fonts/ARCADE.TTF");
        initWorld(window, event, world, steps, font);
        sfRenderWindow_display(window);
        countPlankton = countCreatures(world, Plankton);
        countFishes = countCreatures(world, Fish);
        countHunters = countCreatures(world, Hunter);
//        if (countFishes == 0 || countHunters ==  0 || countPlankton == 0)  sfRenderWindow_close(window);
        printf("Step: %d, Plankton: %d, Fish: %d, Hunters: %d\n", steps++, countPlankton, countFishes, countHunters);
        usleep(20000);
        update(world, countPlankton, countFishes, countHunters);
    }
}