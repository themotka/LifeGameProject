#include <SFML/Graphics.h>
#include <stdio.h>
#include "game_file.c"
#define WIDTH 1900
#define HEIGHT 1000

void if_clicked(sfRenderWindow* window, sfEvent event){
    //pos: x 705; y 703
    //pos: x 1210; y 860
    sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
    if (mousePos.x > 705 && mousePos.x < 1210){
        if (mousePos.y > 705 && mousePos.y < 860){
            gameCycle(window, event);
        }
    }
}
int main()
{
    creature*** world = createWorld();
    generateWorld(world);
    printWorld(world);
    // Режим окна (ширина, высота, глубина цвета)
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    // События
    sfEvent event;
    sfRenderWindow* window;
    sfTexture *texture = sfTexture_createFromFile("images/menu.png", NULL);
    sfTexture *texture1 = sfTexture_createFromFile("images/start.png", NULL);
    sfTexture *texture2 = sfTexture_createFromFile("images/tyagi1.png", NULL);

    sfSprite *sprite = sfSprite_create();
    sfSprite *startButton = sfSprite_create();
    sfSprite *name = sfSprite_create();

    sfVector2f scale = {1.65, 1.7};
    sfVector2f scaleButton = {0.4, 0.4};
    sfVector2f posButton = {(WIDTH-1310*0.4) /2 , 700};
    sfVector2f posName = {300 , 100};

    sfSprite_setTexture(sprite, texture, sfTrue);
    sfSprite_setTexture(startButton, texture1, sfTrue);
    sfSprite_setTexture(name, texture2, sfTrue);

    sfSprite_setScale(sprite, scale);
    sfSprite_setScale(startButton, scaleButton);

    sfSprite_setPosition(startButton, posButton);
    sfSprite_setPosition(name, posName);
    // Создать окно
    window = sfRenderWindow_create(mode, "UnderwaterLife", sfResize | sfClose, NULL);

    // Ограничить частоту кадров в секунду до 60
    sfRenderWindow_setFramerateLimit(window, 60);

    // Основной цикл
    while (sfRenderWindow_isOpen(window))
    {
        // Обработка событий (нажатие кнопок, закрытие окна и т.д.)
        while (sfRenderWindow_pollEvent(window, &event))
        {
            // Закрыть окно если нажата кнопка "Закрыть"
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if(event.type == sfEvtMouseButtonPressed) {
                if_clicked(window, event);
            }
        }

        // Очистить окно и залить его черным цветом

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_drawSprite(window, startButton, NULL);
        sfRenderWindow_drawSprite(window, name, NULL);
        // Отобразить
        sfRenderWindow_display(window);
    }

    // Уничтожить окно
    sfRenderWindow_destroy(window);

    return 0;
}