// jogo_pi.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include <math.h>
#include <stdbool.h>

#define NUM_QUADS 3
#define QUAD_SIZE 80

typedef struct {
    float x, y;
    bool arrastando;
    float offset_x, offset_y;
} Quadrado;

int main(void) {
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_install_mouse()) return -1;
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* disp = al_create_display(800, 600);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(fila, al_get_display_event_source(disp));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));

    bool sair = false;
    bool up = false, down = false, left = false, right = false;

    float x = 400, y = 300;
    const float PLAYER_RADIUS = 20;

    bool movendo_para_click = false;
    float destino_x = x, destino_y = y;

    Quadrado quads[NUM_QUADS] = {
        {100, 100, false, 0, 0},
        {300, 200, false, 0, 0},
        {600, 400, false, 0, 0}
    };

    al_start_timer(timer);

    while (!sair) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_KEY_UP) {
            bool press = (ev.type == ALLEGRO_EVENT_KEY_DOWN);
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_W: case ALLEGRO_KEY_UP:    up = press;    break;
            case ALLEGRO_KEY_S: case ALLEGRO_KEY_DOWN:  down = press;  break;
            case ALLEGRO_KEY_A: case ALLEGRO_KEY_LEFT:  left = press;  break;
            case ALLEGRO_KEY_D: case ALLEGRO_KEY_RIGHT: right = press; break;
            case ALLEGRO_KEY_ESCAPE: if (press) sair = true; break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;

            if (ev.mouse.button & 2) {
                destino_x = mx;
                destino_y = my;
                movendo_para_click = true;
            }

            if (ev.mouse.button & 1) {
                for (int i = 0; i < NUM_QUADS; i++) {
                    if (mx >= quads[i].x && mx <= quads[i].x + QUAD_SIZE &&
                        my >= quads[i].y && my <= quads[i].y + QUAD_SIZE) {
                        quads[i].arrastando = true;
                        quads[i].offset_x = mx - quads[i].x;
                        quads[i].offset_y = my - quads[i].y;
                    }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            for (int i = 0; i < NUM_QUADS; i++) {
                quads[i].arrastando = false;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            for (int i = 0; i < NUM_QUADS; i++) {
                if (quads[i].arrastando) {
                    quads[i].x = ev.mouse.x - quads[i].offset_x;
                    quads[i].y = ev.mouse.y - quads[i].offset_y;
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            const float SPEED = 4.0f;

            if (up) { y -= SPEED; movendo_para_click = false; }
            if (down) { y += SPEED; movendo_para_click = false; }
            if (left) { x -= SPEED; movendo_para_click = false; }
            if (right) { x += SPEED; movendo_para_click = false; }

            if (movendo_para_click) {
                float dx = destino_x - x;
                float dy = destino_y - y;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist > 2.0f) {
                    x += SPEED * (dx / dist);
                    y += SPEED * (dy / dist);
                }
                else {
                    movendo_para_click = false;
                }
            }

            al_clear_to_color(al_map_rgb(0, 0, 0));

            for (int i = 0; i < NUM_QUADS; i++) {
                al_draw_filled_rectangle(quads[i].x, quads[i].y,
                    quads[i].x + QUAD_SIZE,
                    quads[i].y + QUAD_SIZE,
                    al_map_rgb(255, 255, 255));
            }

            al_draw_filled_circle(x, y, PLAYER_RADIUS, al_map_rgb(255, 0, 0));

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_display(disp);
    return 0;
}