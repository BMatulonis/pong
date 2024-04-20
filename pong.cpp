#include <iostream>
#include <cstring>
#include <raylib.h>

using namespace std;

const string ColorThemes[4] = {"RED", "BLUE", "GREEN", "GRAY"};
Color theme[3] = {DARKGRAY, GRAY, LIGHTGRAY};

int ballSpeed = 9;
int maxScore = 3;
int playerScore = 0;
int cpuScore = 0;
int menuState = 0;      // 0:MAIN 1:GAME 2:OPTIONS 3:GAMEOVER
int difficulty = 1;     // 0:EASY 1:NORMAL 2:HARD
const string difficulties[3] = {"EASY", "NORMAL", "HARD"};
string winner;

class Ball{
    public:
    float x, y;
    int speedX, speedY;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, WHITE);
    }

    void Update() {
        x += speedX;
        y += speedY;

        if(y + radius >= GetScreenHeight() || y - radius <= 0)
            speedY *= -1;
        if(x + radius >= GetScreenWidth())
        {
            playerScore++;
            if(playerScore >= maxScore)
            {
                winner = "player";
                menuState = 3;
            }
            ResetBall();
        }
        if(x - radius <= 0)
        {
            cpuScore++;
            if(cpuScore >= maxScore)
            {
                winner = "cpu";
                menuState = 3;
            }
            ResetBall();
        }
    }

    void ResetBall()
    {
        x = GetScreenWidth()/2;
        y = GetScreenHeight()/2;

        int speedOption[2] = {-1,1};
        speedX *= speedOption[GetRandomValue(0,1)];
        speedY *= speedOption[GetRandomValue(0,1)];
    }
};

class Paddle{
    protected:
    void LimitMovement() {
        if(y <= 0)
            y = 0;
        if(y + height >= GetScreenHeight())
            y = GetScreenHeight() - height;
    }

    public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if(IsKeyDown(KEY_W))
            y = y - speed;
        if(IsKeyDown(KEY_S))
            y = y + speed;
        LimitMovement();
    }
};

class CpuPaddle: public Paddle{
    public:
    void Update(int ballX, int ballY, int sWidth, int sHeight) {
        if (difficulty == 2)    // HARD
        {
            if(ballX >= sWidth/2)    // cpu side
            {
                if(y + height/2 > ballY)
                    y = y - speed;
                if(y + height/2 <= ballY)
                    y = y + speed;
            }
            else    // move back to center
            {
                if(y + height/2 > sHeight/2 + height/2)
                    y = y - speed;
                else if(y + height/2 < sHeight/2 - height/2)
                    y = y + speed;
                else
                    y = sHeight/2 - height/2;
            }
            LimitMovement();
        }
        else if (difficulty == 0)   // EASY
        {
            if(ballX >= sWidth/2)    // cpu side
            {
                if(y + height/2 > ballY)
                    y = y - speed;
                if(y + height/2 <= ballY)
                    y = y + speed;
            }
            LimitMovement();
        }
        else        // NORMAL
        {
            if(y + height/2 > ballY)
                y = y - speed;
            if(y + height/2 <= ballY)
                y = y + speed;
            LimitMovement();
        }
    }
};

void Reset(int screenW, int screenH, int offset, int paddleW, int paddleH);
void DrawButton(int btnS, Rectangle btn, int x, int y, const char* phrase);
void CheckButtons(Vector2 mouse, Rectangle btn, int &btnS, bool &btnA);
void ChangeColorTheme(string &current, string newTheme);
void ChangeDifficulty(CpuPaddle &cp, int dif);

Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 800;
    const int paddleWidth = screenWidth / 64;
    const int paddleHeight = screenHeight / 8;
    const int offset = 10;

    InitWindow(screenWidth, screenHeight, "PONG");
    SetTargetFPS(60);

    // Initialize Ball and Paddles
    ball.radius = 20;
    ball.x = screenWidth/2;
    ball.y = screenHeight/2;
    ball.speedX = ballSpeed;
    ball.speedY = ballSpeed;

    player.width = paddleWidth;
    player.height = paddleHeight;
    player.x = offset;
    player.y = screenHeight/2 - player.height/2;
    player.speed = 7;

    cpu.width = paddleWidth;
    cpu.height = paddleHeight;
    cpu.x = screenWidth - paddleWidth - offset;
    cpu.y = screenHeight/2 - paddleHeight/2;
    cpu.speed = 7;

    // Buttons
    const int numB = 8;       // max number of buttons used
    Rectangle button[numB] = { screenWidth/2 - 125, screenHeight/2, 250, 100 };
    int btnState[numB] = {0};      // 0:NORMAL 1:MOUSE HOVER 2:PRESSED
    bool btnAction[numB] = {false};

    string CurrentTheme = ColorThemes[3];
    char difficultyStr[25];
    int timer = 0;

    Vector2 mousePoint = { 0.0f, 0.0f };
    bool exitWindow = false;

    // GAME LOOP
    while (WindowShouldClose() == false && !exitWindow)
    {
        mousePoint = GetMousePosition();

        for (int i = 0; i < numB; i++)      // initial buttons, check if pressed
        {
            btnAction[i] = false;
            CheckButtons(mousePoint, button[i], btnState[i], btnAction[i]);
        }

        BeginDrawing();

        switch (menuState)
        {
        case 0:     // MAIN MENU
            ClearBackground(theme[0]);
            DrawText("PONG", screenWidth/2 - MeasureText("PONG", 120)/2, screenHeight*0.2, 120, WHITE);
            button[0] = { screenWidth/2 - 125, screenHeight*0.4, 250, 100 };
            button[1] = { screenWidth/2 - 125, screenHeight*0.55, 250, 100 };
            button[2] = { screenWidth/2 - 125, screenHeight*0.7, 250, 100 };
            DrawButton(btnState[0], button[0], screenWidth/2, screenHeight*0.4, "START");
            DrawButton(btnState[1], button[1], screenWidth/2, screenHeight*0.55, "OPTIONS");
            DrawButton(btnState[2], button[2], screenWidth/2, screenHeight*0.70, "QUIT");
            if(btnAction[0])
                menuState = 1;
            if(btnAction[1])
                menuState = 2;
            if(btnAction[2])
                exitWindow = true;
            break;

        case 1:     // GAME LOOP
            // Update ball & player positions
            ball.Update();
            player.Update();
            cpu.Update(ball.x, ball.y, screenWidth, screenHeight);

            // Check for collisions
            if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
                ball.speedX *= -1;

            if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
                ball.speedX *= -1;

            // Draw Graphics
            ClearBackground(theme[0]);
            DrawRectangle(0, 0, screenWidth/2, screenHeight, theme[1]);
            DrawLine(screenWidth/2, 0, screenWidth/2, screenHeight, WHITE);
            ball.Draw();
            player.Draw();
            cpu.Draw();
            DrawText(TextFormat("%i", playerScore), screenWidth/4 - offset*2, offset*2, 80, WHITE);
            DrawText(TextFormat("%i", cpuScore), screenWidth - screenWidth/4 - offset*2, offset*2, 80, WHITE);
            break;

        case 2:     // OPTIONS
            ClearBackground(theme[0]);
            DrawText("OPTIONS", screenWidth/2 - MeasureText("OPTIONS", 80)/2, screenHeight*0.15, 80, WHITE);
            DrawText("THEME:", screenWidth*0.3 - MeasureText("THEME:", 50), screenHeight*0.36 + 15, 50, WHITE);
            DrawText("DIFFICULTY:", screenWidth*0.3 - MeasureText("DIFFICULTY:", 50), screenHeight*0.575 + 15, 50, WHITE);
            button[0] = { screenWidth*0.5 - 100, screenHeight*0.3, 200, 75 };
            button[1] = { screenWidth*0.7 - 100, screenHeight*0.3, 200, 75 };
            button[2] = { screenWidth*0.5 - 100, screenHeight*0.425, 200, 75 };
            button[3] = { screenWidth*0.7 - 100, screenHeight*0.425, 200, 75 };
            button[4] = { screenWidth*0.425 - 100, screenHeight*0.575, 200, 75 };
            button[5] = { screenWidth*0.6 - 100, screenHeight*0.575, 200, 75 };
            button[6] = { screenWidth*0.775 - 100, screenHeight*0.575, 200, 75 };
            button[7] = { screenWidth*0.5 - 125, screenHeight*0.75, 250, 100 };
            DrawButton(btnState[0], button[0], screenWidth*0.5, screenHeight*0.3, "RED");
            DrawButton(btnState[1], button[1], screenWidth*0.7, screenHeight*0.3, "BLUE");
            DrawButton(btnState[2], button[2], screenWidth*0.5, screenHeight*0.425, "GREEN");
            DrawButton(btnState[3], button[3], screenWidth*0.7, screenHeight*0.425, "GRAY");
            DrawButton(btnState[4], button[4], screenWidth*0.425, screenHeight*0.575, "EASY");
            DrawButton(btnState[5], button[5], screenWidth*0.6, screenHeight*0.575, "NORMAL");
            DrawButton(btnState[6], button[6], screenWidth*0.775, screenHeight*0.575, "HARD");
            DrawButton(btnState[7], button[7], screenWidth*0.5, screenHeight*0.75, "BACK");

            for (int i = 0; i < 4; i++)
            {
                if(btnAction[i])
                    ChangeColorTheme(CurrentTheme, ColorThemes[i]);
            }
            for (int i = 0; i < 3; i++)
            {
                if(btnAction[i+4])
                {
                    ChangeDifficulty(cpu, i);
                    strcpy(difficultyStr,"Difficulty set to ");
                    strcat(difficultyStr,difficulties[i].data());
                    timer = 180;
                }
            }

            if (timer > 0)
            {
                timer--;
                DrawText(difficultyStr, screenWidth*0.9 - MeasureText("DIFFICULTY:", 50), screenHeight*0.9 + 15, 30, WHITE);
            }
            
            if(btnAction[7])
                menuState = 0;
            break;

        case 3:     // GAME OVER
            ClearBackground(theme[0]);
            if(winner == "cpu")
                DrawText("GAME OVER", screenWidth/2 - MeasureText("GAME OVER", 100)/2, screenHeight/4, 100, WHITE);
            else
                DrawText("WINNER!", screenWidth/2 - MeasureText("WINNER!", 100)/2, screenHeight/4, 100, WHITE);

            button[0] = { screenWidth/2 - 125, screenHeight*0.45, 250, 100 };
            button[1] = { screenWidth/2 - 125, screenHeight*0.65, 250, 100 };
            DrawButton(btnState[0], button[0], screenWidth/2, screenHeight*0.45, "RESTART");
            DrawButton(btnState[1], button[1], screenWidth/2, screenHeight*0.65, "MAIN MENU");
            if(btnAction[0])
                Reset(screenWidth, screenHeight, offset, paddleWidth, paddleHeight);
            if(btnAction[1])
            {
                Reset(screenWidth, screenHeight, offset, paddleWidth, paddleHeight);
                menuState = 0;
            }
            break;
        }

        DrawText("Press Esc to exit", 0, 0, 20, { 255, 255, 255, 150});

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void Reset(int screenW, int screenH, int offset, int paddleW, int paddleH)
{
    menuState = 1;
    playerScore = 0;
    cpuScore = 0;
    player.x = offset;
    player.y = screenH/2 - player.height/2;
    cpu.x = screenW - paddleW - offset;
    cpu.y = screenH/2 - paddleH/2;
}

void DrawButton(int btnS, Rectangle btn, int x, int y, const char* phrase)
{
    if(btnS >= 1)
        DrawRectangleRounded(btn, 0.8, 0, theme[2]);
    else
        DrawRectangleRounded(btn, 0.8, 0, WHITE);
    Vector2 textVec = MeasureTextEx(GetFontDefault(), phrase, 40, 0);
    textVec.x = MeasureText(phrase, 40);
    DrawText(phrase, x - textVec.x/2, y + btn.height/2 - textVec.y/2, 40, BLACK);
}

void CheckButtons(Vector2 mouse, Rectangle btn, int &btnS, bool &btnA)
{
    if(CheckCollisionPointRec(mouse, btn))  // hovering over button
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            btnS = 2;
        else 
            btnS = 1;
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            btnA = true;
    }
    else btnS = 0;
}

void ChangeColorTheme(string &current, string newTheme)
{
    if (newTheme == "RED")
    {
        current = ColorThemes[0];
        theme[0] = MAROON;
        theme[1] = RED;
        theme[2] = LIGHTGRAY;
    }
    if (newTheme == "BLUE")
    {
        current = ColorThemes[1];
        theme[0] = DARKBLUE;
        theme[1] = BLUE;
        theme[2] = LIGHTGRAY;
    }
    if (newTheme == "GREEN")
    {
        current = ColorThemes[2];
        theme[0] = DARKGREEN;
        theme[1] = LIME;
        theme[2] = LIGHTGRAY;
    }
    if (newTheme == "GRAY")
    {
        current = ColorThemes[3];
        theme[0] = DARKGRAY;
        theme[1] = GRAY;
        theme[2] = LIGHTGRAY;
    }
}

void ChangeDifficulty(CpuPaddle &cp, int dif)
{
    if (dif == 0)
    {
        cp.speed = 6;
        ballSpeed = 8;
    }
    if (dif == 1)
    {
        cp.speed = 7;
        ballSpeed = 9;
    }
    if (dif == 2)
    {
        cp.speed = 9;
        ballSpeed = 10;
    }
    difficulty = dif;
}