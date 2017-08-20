// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 4

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{   
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 1;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        waitForClick();
                
        double velocity_y = 3.0;
        double velocity_x = drand48() * 3;
        
        while (true)
        {
                       
            move(ball,velocity_x,velocity_y);
            GObject object = detectCollision(window,ball);
            if (getY(ball) + getHeight(ball) >= getHeight(window))
            {
                lives--;
                setLocation(ball,200,300);
                setLocation(paddle,170,570);
                waitForClick();
            }
            
            GEvent move = getNextEvent(MOUSE_EVENT);
        
            if (move != NULL)
            {
                if (getEventType(move) == MOUSE_MOVED)
                {
                    double x = getX(move) - getWidth(paddle);
                    setLocation(paddle,x,570);
                    
                    if(getX(move) <= 70)
                    {
                        setLocation(paddle,0,570);
                    }
                                           
                }
                
            }
                                
            if (object != NULL)
            {           
            if (object == paddle)
            {
                velocity_y = -velocity_y;
            }
            
            else if(strcmp(getType(object), "GRect") == 0)
            {
                velocity_y = -velocity_y;
                removeGWindow(window, object);
                bricks--;
                char point[3];
                sprintf(point, "%i", points);
                points++;
                setLabel(label, point);
                
            }
            }
            
            
            else if (getY(ball) <= 0)
            {
                velocity_y = -velocity_y;
            }
            
            if (getX(ball) + getWidth(ball) >= getWidth(window))
            {
                velocity_x = -velocity_x;
            }
            else if (getX(ball) <= 0)
            {
                velocity_x = -velocity_x;
            }
            pause(10);
            
            if (lives == 0)
            {
                GLabel GAME_OVER = newGLabel("GAME OVER");
                double x = (getWidth(window) - getWidth(label)) / 4;
                double y = (getHeight(window) - getHeight(label)) / 2;
                
                setLocation(GAME_OVER, x, y);
                add(window, GAME_OVER);
                pause(100);
                closeGWindow(window);
            }
            
            if (bricks == 0)
            {
                GLabel YOU_WON = newGLabel("HBDS!! You won!! ");
                double x = (getWidth(window) - getWidth(label)) / 100;
                double y = (getHeight(window) - getHeight(label)) / 2;
                setLocation(YOU_WON, x, y);
                add(window, YOU_WON);
                pause(10000);
                closeGWindow(window);
            }
            }
            
        
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    int y=20;
    string colors[5] = {"RED","ORANGE","YELLOW","GREEN","BLUE"};
    for (int i = 0; i < ROWS; i++)
    {
        int x = 2;
        for (int j = 0; j < COLS; j++,x+=40)
        {
           
           GRect brick = newGRect(x,y,35, 10);
           setColor(brick, colors[i]);            
           setFilled(brick, true);
           add(window, brick);
        }
        y+=15;
    }
    
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(200,300,20,20);
    setColor(ball, "#c90020");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(165,570,70,15);
    setColor(paddle, "#20b2aa");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel score = newGLabel("0");
    setFont(score, "SansSerif-36");
    setColor(score, "PINK");
    setLocation(score,200,300);
    add(window,score);
    return score;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
