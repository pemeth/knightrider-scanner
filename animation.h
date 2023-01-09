#ifndef ANIMATION_H
#define ANIMATION_H

#define DIR_CHANGE      1   // To be XORed if direction is changed
#define DIR_LR          0   // Left-to-Right
#define DIR_RL          1   // Right-to-Left

#define SPEED_STEP      2500
#define MAX_SPEED       SPEED_STEP
#define MIN_SPEED       SPEED_STEP * 40
#define DEFAULT_SPEED   SPEED_STEP * 10

#define MODES_N         2
#define MODE_BASIC      0
#define MODE_SMOOTH     1

typedef struct animation_settings
{
    int const snake_y;
    int const snake_len;
    int snake_head;
    int direction;
    int direction_change_flag;
    int max_y;
    int max_x;
} animation_settings_t;

/**
 * @brief Basic animation mode just flips the snake after reaching the edge.
 *
 * @param s pointer to struct containing info about current animation frame.
 */
void mode_basic(animation_settings_t const *s);

/**
 * @brief The snake flip at the edges is smoothed out by having the snake's
 * tail chase the head even after flipping.
 *
 * @param s pointer to struct containing info about current animation frame.
 */
void mode_smooth(animation_settings_t const *s);

#endif /* ANIMATION_H */
