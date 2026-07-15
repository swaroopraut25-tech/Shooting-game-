#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ncurses.h>
#include<unistd.h>
#include<stdbool.h>

/* * BOTTLE SHOOTER GAME (C Language with ncurses)
 * * Requirements:
 * This program uses the ncurses library, which is part of the GNU project.
 * first run the program in terminal to recognize ncurses library.
 * To compile: gcc main.c -o main -lncurses
 * To run: ./main
 * * Controls:
 * - Left/Right Arrow Keys: Move launcher
 * - Spacebar: Shoot bullet
 * - 'q': Quit game
 */

#define MAX_BULLETS 10
#define MAX_BOTTLES 5

typedef struct {
    int x, y;
    bool active;
} Entity;

int main() {
    // Initialize ncurses
    initscr();
    noecho();           // Don't show typed characters
    curs_set(FALSE);    // Hide the cursor
    keypad(stdscr, TRUE); // Enable arrow keys
    nodelay(stdscr, TRUE); // Non-blocking input

    int screen_w, screen_h;
    getmaxyx(stdscr, screen_h, screen_w);

    // Game state variables
    int player_x = screen_w / 2;
    int score = 0;
    int level = 1;
    bool running = true;

    Entity bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;

    Entity bottles[MAX_BOTTLES];
    for (int i = 0; i < MAX_BOTTLES; i++) {
        bottles[i].x = rand() % (screen_w - 2);
        bottles[i].y = 1 + (rand() % 3);
        bottles[i].active = true;
    }

    // Main Game Loop
    while (running) {
        int ch = getch();

        // Handle Input
        switch (ch) {
            case KEY_LEFT:
                if (player_x > 0) player_x--;
                break;
            case KEY_RIGHT:
                if (player_x < screen_w - 1) player_x++;
                break;
            case ' ': // Spacebar to shoot
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = player_x;
                        bullets[i].y = screen_h - 2;
                        bullets[i].active = true;
                        break;
                    }
                }
                break;
            case 'q':
                running = false;
                break;
        }

        // Logic: Move Bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].y--;
                if (bullets[i].y < 0) bullets[i].active = false;
            }
        }

        // Logic: Move and Respawn Bottles
        static int frame_count = 0;
        frame_count++;
        if (frame_count % (6 - (level > 4 ? 4 : level)) == 0) {
            for (int i = 0; i < MAX_BOTTLES; i++) {
                if (bottles[i].active) {
                    bottles[i].x += (i % 2 == 0 ? 1 : -1); // Move back and forth
                    if (bottles[i].x >= screen_w) bottles[i].x = 0;
                    if (bottles[i].x < 0) bottles[i].x = screen_w - 1;
                }
            }
        }

        // Logic: Collision Detection
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) continue;
            for (int j = 0; j < MAX_BOTTLES; j++) {
                if (bottles[j].active && bullets[i].x == bottles[j].x && bullets[i].y == bottles[j].y) {
                    bottles[j].active = false;
                    bullets[i].active = false;
                    score += 10;
                    
                    // Level up every 100 points
                    if (score % 100 == 0) level++;

                    // Respawn bottle
                    bottles[j].x = rand() % (screen_w - 2);
                    bottles[j].y = 1 + (rand() % 3);
                    bottles[j].active = true;
                }
            }
        }

        // Rendering
        erase(); // Clear screen buffer

        // Draw Score and HUD
        mvprintw(0, 2, "Score: %d | Level: %d | 'q' to Quit", score, level);
        mvhline(4, 0, '-', screen_w); // Boundary line

        // Draw Bottles (Representation: [B])
        for (int i = 0; i < MAX_BOTTLES; i++) {
            if (bottles[i].active) {
                attron(A_BOLD);
                mvaddch(bottles[i].y, bottles[i].x, 'B');
                attroff(A_BOLD);
            }
        }

        // Draw Bullets (Representation: |)
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                mvaddch(bullets[i].y, bullets[i].x, '|');
            }
        }

        // Draw Player (Representation: ^)
        mvaddch(screen_h - 1, player_x, '^');

        refresh(); // Update physical screen
        usleep(30000); // Control game speed (30ms delay)
    }

    // Cleanup
    endwin();
    printf("Game Over! Your Final Score: %d\n", score);

    return 0;
}   
