#include <SFML/Graphics.h>
#include <stdio.h>

#define WIDTH 1920
#define HEIGHT 1080


void gameCycle(sfRenderWindow *window, sfEvent event) {
    while (sfRenderWindow_isOpen(window)) { // тот же цикл отслеживания действий
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        sfRenderWindow_clear(window, sfWhite);
        sfRenderWindow_display(window);
    }
}