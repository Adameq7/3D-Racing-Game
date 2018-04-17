#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fifo.h"

#define M_PI 3.141592653589793

#define MAPSIZE 129
#define TILESIZE 1.0f
#define TERRAIN_FACTOR 15
#define TERRAIN_FACTOR_DECAY 1.5
#define WATER_LEVEL 1.5
#define MOUNTAIN_LEVEL 12.0
#define SNOW_LEVEL 36.0
#define MAX_WATER_LEVEL 1.0

#define MOVE_STEP 1.0
#define ROTATE_STEP 2.5f
#define ROTATE_STEP_F 0.05f

#define ModelCount 4
#define MAX_VERTEX_COUNT 4096
#define ModelScale 0.005f

#define ScreenEdge 50

#define TURN_ANGLE 15
#define ROAD_LENGTH 200

#define PLAYER_COUNT 2

#define LETTER_SIZE 0.1

#define CounterDelay 10

typedef struct _car
{
    int X, Y, VX, VY, CX, CY;

    float Angle;

    float R, G, B;

}car;

typedef struct _tile
{
    int R, G, B;
    float Height;
    bool HasRoad;
    bool Finish;
}tile;

typedef struct _color
{
    float R, G, B;
}color;

typedef struct _point
{
    float X, Y, Z;
}point;

typedef struct _option
{
    char Name[32];
}option;

void ChangeSize(int, int);
void ProcessNormalKeys(unsigned char, int, int);
void ProcessSpecialKeys(int, int, int);

void ProcessMouseMovement(int, int);
int RandomizeInt(int, int);
double Dist(float, float);
float max(float, float);
float min(float, float);

void LoadGame(char*);
void SaveGame(char*);
bool LoadModel(int, char*);
void DrawModel(int, int, int, float, float, float, float, float, bool);
float GenerateMap();
void GenerateRoad();
void GenMapRek(int, int, int, int, int);
void ProcessGame(void);
bool MovePlayer(car*);
void RotateCar(car*);
void BuildMapModel();
void BuildRoadModel();
void PrepareOptions();
void LoadFonts();

void RenderScene(void);
void DrawGround();
void DrawRoad();
void DrawWater();
void DrawLine(point, point, color);
void DrawMapInMenu();
void DrawTextPrep();
void DrawPlayers();
void DrawLetter(int);
void DrawText(float, float, char*, bool);
