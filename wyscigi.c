#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/SOIL.h>
#include <math.h>
#include <stdbool.h>
#include "wyscigi.h"
#include "fifo.h"

GLfloat TerrainVertices[MAPSIZE * MAPSIZE * 12];                //Współrzędne wierzchołków mapy
GLubyte TerrainColors[MAPSIZE * MAPSIZE * 12];                  //Kolory wierzchołków mapy
unsigned int TerrainIndices[MAPSIZE * MAPSIZE * 4];             //Numery wierzchołków w wielokątach modelu mapy
GLfloat RoadVertices[MAPSIZE * MAPSIZE * 12];                   //Współrzędne wierzchołków drogi
GLubyte RoadColors[MAPSIZE * MAPSIZE * 12];                     //Kolory wierzchołków drogi
unsigned int RoadIndices[MAPSIZE * MAPSIZE * 4];                //Numery wierzchołków w wielokątach modelu drogi
GLfloat ModelVertices[ModelCount][MAX_VERTEX_COUNT];            //Wierzchołki wczytanych modeli
unsigned int ModelIndices[ModelCount][MAX_VERTEX_COUNT * 3];    //Numery wierzchołków w wielokątach wczytanych modeli

tile MapData[MAPSIZE][MAPSIZE];                                 //Informacje o aktualnie wczytanej mapie

int RoadIndicesCount = 0;                                       //Liczba wierzchołków w wielokątach modelu drogi
int TerrainIndicesCount = 0;                                    //Liczba wierzchołków w wielokątach modelu mapy

int MousePosX, MousePosY;                                       //Współrzędne kursora myszy

unsigned char* Font;                                            //Tekstura czcionki

bool EnterPressed = false;
bool EscapePressed = false;
bool MoveMade[2] = {false, false};                              //Czy gracz wykonał ruch
bool MoveSent = false;                                          //Czy wykonany ruch został przesłany drugiemu graczowi

extern float CamPosX, CamPosY, CamPosZ, CamAngle, CamAngleY, CamTargetX, CamTargetY, CamTargetZ, CamDistance;
extern int PlayerID;
extern int CurrentOption;
extern int WinnerID, CurrentTurn;
extern int MenuMode;
extern bool InGame;
extern bool RestartMap, OverwriteSave, RestartMapPrompt, OverwriteSavePrompt, CannotLoadPrompt, RaceOver, MoveMade[2], PipesSet;
extern bool EnterPressed, EscapePressed;
extern option MenuOptions[16];
extern float WaterLevel;
extern car Player[PLAYER_COUNT];
extern PipesPtr potoki;

int counter = 60;

double Dist(float a, float b)                                   //Funkcja zwracająca odległosć pomiędzy dwoma punktami na płaszczyźnie
{
    return sqrt(a * a + b * b);
}

void BuildMapModel()                    //Funkcja budująca model mapy na podstawie jej aktualnego stanu
{
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;

    for(int i = 0; i < MAPSIZE - 1; i++)
    {
        for(int j = 0; j < MAPSIZE - 1; j++)
        {
                        TerrainVertices[p1] = TILESIZE * (float)i;
                        TerrainVertices[p1 + 1] = MapData[i][j].Height;
                        TerrainVertices[p1 + 2] = TILESIZE * (float)j;
                        TerrainVertices[p1 + 3] = TILESIZE * (float)(i + 1);
                        TerrainVertices[p1 + 4] = MapData[i + 1][j].Height;
                        TerrainVertices[p1 + 5] = TILESIZE * (float)j;
                        TerrainVertices[p1 + 6] = TILESIZE * (float)(i + 1);
                        TerrainVertices[p1 + 7] = MapData[i + 1][j + 1].Height;
                        TerrainVertices[p1 + 8] = TILESIZE * (float)(j + 1);
                        TerrainVertices[p1 + 9] = TILESIZE * (float)i;
                        TerrainVertices[p1 + 10] = MapData[i][j + 1].Height;
                        TerrainVertices[p1 + 11] = TILESIZE * (float)(j + 1);

                        TerrainIndices[p2] = p2;
                        TerrainIndices[p2+1] = p2 + 1;
                        TerrainIndices[p2+2] = p2 + 2;
                        TerrainIndices[p2+3] = p2 + 3;

                        TerrainColors[p3] = MapData[i][j].R;
                        TerrainColors[p3 + 1] = MapData[i][j].G;
                        TerrainColors[p3 + 2] = MapData[i][j].B;
                        TerrainColors[p3 + 3] = MapData[i + 1][j].R;
                        TerrainColors[p3 + 4] = MapData[i + 1][j].G;
                        TerrainColors[p3 + 5] = MapData[i + 1][j].B;
                        TerrainColors[p3 + 6] = MapData[i + 1][j + 1].R;
                        TerrainColors[p3 + 7] = MapData[i + 1][j + 1].G;
                        TerrainColors[p3 + 8] = MapData[i + 1][j + 1].B;
                        TerrainColors[p3 + 9] = MapData[i][j + 1].R;
                        TerrainColors[p3 + 10] = MapData[i][j + 1].G;
                        TerrainColors[p3 + 11] = MapData[i][j + 1].B;

                        p1 += 12;
                        p2 += 4;
                        p3 += 12;
        }
    }

    TerrainIndicesCount = p2 - 4;
}

void BuildRoadModel()                   //Funkcja budująca model drogi na podstawie jej aktualnego stanu
{
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;

    for(int i = 0; i < MAPSIZE; i++)
    {
        for(int j = 0; j< MAPSIZE; j++)
        {
            if(MapData[i][j].HasRoad)
            {
                        RoadVertices[p1] = TILESIZE * (float)i;
                        RoadVertices[p1 + 1] = max(MapData[i][j].Height + 0.1f, WaterLevel + 0.1f);
                        RoadVertices[p1 + 2] = TILESIZE * (float)j;
                        RoadVertices[p1 + 3] = TILESIZE * (float)(i + 1);
                        RoadVertices[p1 + 4] = max(MapData[i + 1][j].Height + 0.1f, WaterLevel + 0.1f);
                        RoadVertices[p1 + 5] = TILESIZE * (float)j;
                        RoadVertices[p1 + 6] = TILESIZE * (float)(i + 1);
                        RoadVertices[p1 + 7] = max(MapData[i + 1][j + 1].Height + 0.1f, WaterLevel + 0.1f);
                        RoadVertices[p1 + 8] = TILESIZE * (float)(j + 1);
                        RoadVertices[p1 + 9] = TILESIZE * (float)i;
                        RoadVertices[p1 + 10] = max(MapData[i][j + 1].Height + 0.1f, WaterLevel + 0.1f);
                        RoadVertices[p1 + 11] = TILESIZE * (float)(j + 1);

                        RoadIndices[p2] = p2;
                        RoadIndices[p2+1] = p2 + 1;
                        RoadIndices[p2+2] = p2 + 2;
                        RoadIndices[p2+3] = p2 + 3;

                        if(MapData[i][j].Finish)
                        {
                            RoadColors[p3] = 255;
                            RoadColors[p3 + 1] = 0;
                            RoadColors[p3 + 2] = 0;
                            RoadColors[p3 + 3] = 255;
                            RoadColors[p3 + 4] = 0;
                            RoadColors[p3 + 5] = 0;
                            RoadColors[p3 + 6] = 255;
                            RoadColors[p3 + 7] = 0;
                            RoadColors[p3 + 8] = 0;
                            RoadColors[p3 + 9] = 255;
                            RoadColors[p3 + 10] = 0;
                            RoadColors[p3 + 11] = 0;
                        }
                        else
                        {
                            RoadColors[p3] = 0;
                            RoadColors[p3 + 1] = 0;
                            RoadColors[p3 + 2] = 0;
                            RoadColors[p3 + 3] = 0;
                            RoadColors[p3 + 4] = 0;
                            RoadColors[p3 + 5] = 0;
                            RoadColors[p3 + 6] = 0;
                            RoadColors[p3 + 7] = 0;
                            RoadColors[p3 + 8] = 0;
                            RoadColors[p3 + 9] = 0;
                            RoadColors[p3 + 10] = 0;
                            RoadColors[p3 + 11] = 0;
                        }

                        p1 += 12;
                        p2 += 4;
                        p3 += 12;
            }
        }
    }

    RoadIndicesCount = p2 - 4;
}

void SaveGame(char* Filename)                   //Funkcja zapisująca aktualny stan gry w pliku o podanej nazwie
{
    FILE *fp = fopen(Filename, "w");

    fprintf(fp, "%f\n%d %d %d %d\n%d %d %d %d\n", WaterLevel, Player[0].X, Player[0].Y, Player[0].VX, Player[0].VY, Player[1].X, Player[1].Y, Player[1].VX, Player[1].VY);

    for(int i = 0; i < MAPSIZE; i++)
    {
        for(int j = 0; j < MAPSIZE; j++)
        {
            fprintf(fp, "%f %d %d %d %d %d", MapData[j][i].Height, MapData[j][i].R, MapData[j][i].G, MapData[j][i].B, (MapData[j][i].HasRoad) ? 1 : 0, (MapData[j][i].Finish) ? 1 : 0);

            if(j < MAPSIZE - 1)
                fprintf(fp, " ");
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

void LoadGame(char* Filename)                   //Funkcja wczytująca podany plik jako zapis gry
{
    FILE *fp = fopen(Filename, "r");

    fscanf(fp, "%f\n%d %d %d %d\n%d %d %d %d\n", &WaterLevel, &Player[0].X, &Player[0].Y, &Player[0].VX, &Player[0].VY, &Player[1].X, &Player[1].Y, &Player[1].VX, &Player[1].VY);

    for(int i = 0; i < MAPSIZE; i++)
    {
        for(int j = 0; j < MAPSIZE; j++)
        {
            fscanf(fp, "%f %d %d %d %d %d", &MapData[j][i].Height, &MapData[j][i].R, &MapData[j][i].G, &MapData[j][i].B, &MapData[j][i].HasRoad, &MapData[j][i].Finish);
        }
    }

    fclose(fp);
}

float max(float a, float b)                     //Funkcja zwracają maksimum dwóch liczb
{
    return (a > b) ? a : b;
}

float min(float a, float b)                     //Funkcja zwracająca minimum dwóch liczb
{
    return (a < b) ? a : b;
}

int RandomizeInt(int number, int percent)       //Funkcja zwracająca wartość różniącą się od pierwotnej o maksymalnie podany procent
{
    if(percent > 100)
        percent = 100;
    if(percent < 0)
        percent = 0;

    percent = rand()%percent;

    return (int)((float)number * (float)(1 - (float)percent / 100));
}

void LoadFonts()                                //Funkcja wczytująca czcionkę/i
{
    int width = 512, height = 512;
    Font = SOIL_load_image("Fonts\\Font1.png", &width, &height, 0, SOIL_LOAD_RGBA);
}

void PrepareOptions()                           //Funkcja przygotowująca nazwy opcji w menu
{
    option temp;

    strcpy(temp.Name,"Utworz gre");
    MenuOptions[0] = temp;

    strcpy(temp.Name,"Dolacz do gry");
    MenuOptions[1] = temp;

    strcpy(temp.Name,"Wyjdz z gry");
    MenuOptions[2] = temp;

    strcpy(temp.Name,"Graj na aktualnej mapie");
    MenuOptions[3] = temp;

    strcpy(temp.Name,"Wygeneruj nowa mape");
    MenuOptions[4] = temp;

    strcpy(temp.Name,"Zapisz aktualna mape");
    MenuOptions[5] = temp;

    strcpy(temp.Name,"Wczytaj zapisana mape");
    MenuOptions[6] = temp;

    strcpy(temp.Name,"Wstecz");
    MenuOptions[7] = temp;

    strcpy(temp.Name,"Kontynuuj gre");
    MenuOptions[8] = temp;

    strcpy(temp.Name,"Zapisz stan gry");
    MenuOptions[9] = temp;

    strcpy(temp.Name,"Zakoncz gre");
    MenuOptions[10] = temp;
}

void ProcessNormalKeys(unsigned char key, int x, int y)     //Funkcja glutowska odpowiedzialna za obsługę klawiatury
{
    switch(key)
    {
        //Klawisza W-S-A-D służą do obracania kamery, E i Q do jej przybliżania i oddalania, klawisz T przywraca ją do pierwotnego stanu, a F potwierdza wykonany ruch
/*        case 'w':
            if(InGame)
                CamAngleY += ROTATE_STEP;
            break;
        case 's':
            if(InGame)
                CamAngleY -= ROTATE_STEP;
        break;
        case 'd':
            if(InGame)
                CamAngle += ROTATE_STEP;
        break;
        case 'a':
            if(InGame)
                CamAngle -= ROTATE_STEP;
        break;*/
        case 'w':
            if(InGame)
                if(Player[1].CY < 1)
                    Player[1].CY++;
            if((MenuMode != -1))
                CurrentOption--;
            CannotLoadPrompt = false;
        break;
        case 's':
            if(InGame)
                if(Player[1].CY > -1)
                    Player[1].CY--;
            if((MenuMode != -1))
                CurrentOption++;
            CannotLoadPrompt = false;
        break;
        case 'd':
            if(InGame)
                if(Player[1].CX > -1)
                    Player[1].CX--;
            if(MenuMode == 2)
                OverwriteSave = false;
            if(MenuMode == 3)
                RestartMap = false;
            CannotLoadPrompt = false;
        break;
        case 'a':
            if(InGame)
                if(Player[1].CX < 1)
                    Player[1].CX++;
            if(MenuMode == 2)
                OverwriteSave = true;
            if(MenuMode == 3)
                RestartMap = true;
            CannotLoadPrompt = false;
        break;
        case 'f':
            MoveMade[PlayerID] = true;
        break;
        case 'e':
            if(InGame)
                CamDistance += MOVE_STEP;
        break;
        case 'q':
            if(InGame)
                CamDistance -= MOVE_STEP;
        break;
        case 't':
            CamAngle = 180;
            CamAngleY = 45;
        break;
        case 13:
            EnterPressed = true;
            CannotLoadPrompt = false;
        break;
        case 27:
            EscapePressed = true;
            CannotLoadPrompt = false;
        break;
    }
}

void ProcessSpecialKeys(int key, int x, int y)     //Funkcja glutowska odpowiedzialna za obsługę klawiatury
{
    switch(key)
    {
        //Klawisze strzałek modyfikują wektor przemieszczenia gracza i służą do obsługi menu
        case GLUT_KEY_UP:
            if(InGame)
                if(Player[0].CY < 1)
                    Player[0].CY++;
            if((MenuMode != -1))
                CurrentOption--;
            CannotLoadPrompt = false;
        break;
        case GLUT_KEY_DOWN:
            if(InGame)
                if(Player[0].CY > -1)
                    Player[0].CY--;
            if((MenuMode != -1))
                CurrentOption++;
            CannotLoadPrompt = false;
        break;
        case GLUT_KEY_RIGHT:
            if(InGame)
                if(Player[0].CX > -1)
                    Player[0].CX--;
            if(MenuMode == 2)
                OverwriteSave = false;
            if(MenuMode == 3)
                RestartMap = false;
            CannotLoadPrompt = false;
        break;
        case GLUT_KEY_LEFT:
            if(InGame)
                if(Player[0].CX < 1)
                    Player[0].CX++;
            if(MenuMode == 2)
                OverwriteSave = true;
            if(MenuMode == 3)
                RestartMap = true;
            CannotLoadPrompt = false;
        break;
    }
}

void ProcessMouseMovement(int x, int y)     //Funkcja glutowska odpowiedzialna za obsługę myszy
{
    MousePosX = x;
    MousePosY = y;
}

void GenMapRek(int px, int kx, int py, int ky, int factor)      //Funkcja rekurencyjnie generująca mapy przy pomocy algorytmu diamond-square
{
    if(factor == 0)
        factor = 1;

    if((kx - px) < 2 || (ky - py) < 2)
        return;

    MapData[(px + kx) / 2][(py + ky) / 2].Height = (MapData[px][py].Height + MapData[px][ky].Height + MapData[kx][py].Height + MapData[kx][ky].Height) / 4 + (-rand()%factor + 2 * (rand()%factor));

    if(MapData[(px + kx) / 2][py].Height == 0)
        MapData[(px + kx) / 2][py].Height = (MapData[px][py].Height + MapData[kx][py].Height + MapData[(px + kx) / 2][(py + ky) / 2].Height) / 3 + (-rand()%factor + 2 * (rand()%factor));
    if(MapData[kx][(py + ky) / 2].Height == 0)
        MapData[kx][(py + ky) / 2].Height = (MapData[kx][py].Height + MapData[kx][ky].Height + MapData[(px + kx) / 2][(py + ky) / 2].Height) / 3 + (-rand()%factor + 2 * (rand()%factor));
    if(MapData[(px + kx) / 2][ky].Height == 0)
        MapData[(px + kx) / 2][ky].Height = (MapData[px][ky].Height + MapData[kx][ky].Height + MapData[(px + kx) / 2][(py + ky) / 2].Height) / 3 + (-rand()%factor + 2 * (rand()%factor));
    if(MapData[px][(py + ky) / 2].Height == 0)
        MapData[px][(py + ky) / 2].Height = (MapData[px][py].Height + MapData[px][ky].Height + MapData[(px + kx) / 2][(py + ky) / 2].Height) / 3 + (-rand()%factor + 2 * (rand()%factor));

    GenMapRek(px, (px + kx) / 2, py, (py + ky) / 2, (float)factor / TERRAIN_FACTOR_DECAY);
    GenMapRek((px + kx) / 2, kx,  py, (py + ky) / 2, (float)factor / TERRAIN_FACTOR_DECAY);
    GenMapRek(px, (px + kx) / 2, (py + ky) / 2, ky, (float)factor / TERRAIN_FACTOR_DECAY);
    GenMapRek((px + kx) / 2, kx,  (py + ky) / 2, ky, (float)factor / TERRAIN_FACTOR_DECAY);
}

float GenerateMap()         //Główna funkcją generująca mapę
{
    for(int i = 0; i < MAPSIZE; i++)
        for(int j = 0; j < MAPSIZE; j++)
        {
            MapData[i][j].Height = 0;
            MapData[i][j].Finish = false;
        }

    MapData[0][0].Height = (float)(rand()%TERRAIN_FACTOR);
    MapData[0][MAPSIZE - 1].Height = (float)(rand()%TERRAIN_FACTOR);
    MapData[MAPSIZE - 1][0].Height = (float)(rand()%TERRAIN_FACTOR);
    MapData[MAPSIZE - 1][MAPSIZE - 1].Height = (float)(rand()%TERRAIN_FACTOR);

    WaterLevel = rand()%TERRAIN_FACTOR + 10;

    GenMapRek(0, MAPSIZE - 1, 0, MAPSIZE - 1, TERRAIN_FACTOR);

    for(int i = 0; i < MAPSIZE; i++)
        for(int j = 0; j < MAPSIZE; j++)
        {
            if(MapData[i][j].Height <= WaterLevel + WATER_LEVEL)
            {
                MapData[i][j].R = RandomizeInt(255, 10);
                MapData[i][j].G = RandomizeInt(255, 10);
                MapData[i][j].B = 0;
            }
            else if((WaterLevel + WATER_LEVEL < MapData[i][j].Height) && (MapData[i][j].Height <= WaterLevel + MOUNTAIN_LEVEL))
            {
                MapData[i][j].R = 0;
                MapData[i][j].G = RandomizeInt(160, 10);
                MapData[i][j].B = 0;
            }
            else if((WaterLevel + MOUNTAIN_LEVEL < MapData[i][j].Height) && (MapData[i][j].Height <= WaterLevel + SNOW_LEVEL))
            {
                MapData[i][j].R = RandomizeInt(128, 20);
                MapData[i][j].G = RandomizeInt(128, 20);
                MapData[i][j].B = RandomizeInt(128, 20);
            }
            else if(MapData[i][j].Height > WaterLevel + SNOW_LEVEL)
            {
                MapData[i][j].R = RandomizeInt(255, 10);
                MapData[i][j].G = RandomizeInt(255, 10);
                MapData[i][j].B = RandomizeInt(255, 10);
            }
        }

    MapData[MAPSIZE / 2][0].Height = (MapData[MAPSIZE / 2 - 1][0].Height + MapData[MAPSIZE / 2 + 1][0].Height) / 2;

    BuildMapModel();

    float w = 0;

    for(int i = 0; i < MAPSIZE - 1; i++)
    {
        for(int j = 0; j < MAPSIZE - 1; j++)
        {
            if(MapData[i][j].Height < WATER_LEVEL)
                w++;
        }
    }

    w /= (MAPSIZE * MAPSIZE);

    return w;
}

bool LoadModel(int i, char filename[])      //Funkcja wczytująca wskazany model w formacie *.obj do pamięci
{
    FILE *fp;
    int facecount = 0, vc = 3, fc = 0;

    float a, b, c;
    int x, y, z;
    char d[64];

    if((fp = fopen(filename, "r")) == NULL)
        return false;

    while((fgets(d, 64, fp)) != NULL)
    {
        if(d[0] == 'v')
        {
            sscanf(d, "v %f %f %f\n", &a, &b, &c);

            ModelVertices[i][vc] = a * ModelScale;
            ModelVertices[i][vc + 1] = c * ModelScale;
            ModelVertices[i][vc + 2] = b * ModelScale;

            vc += 3;
        }

        if(d[0] == 'f')
        {
            sscanf(d, "f %d %d %d\n", &x, &y, &z);

            ModelIndices[i][fc] = x;
            ModelIndices[i][fc + 1] = y;
            ModelIndices[i][fc + 2] = z;

            fc += 3;
        }
    }

    facecount = fc / 3;

    ModelIndices[i][MAX_VERTEX_COUNT * 3 - 1] = facecount;

    rewind(fp);

    fclose(fp);

    return true;
}

void GenerateRoad()         //Główna funkcją generująca drogę
{
    int x, y, z, l;

    float angle = 0;
    float distance = 1;

    while(1)
    {
        for(int i = 0; i < MAPSIZE; i++)
            for(int j = 0; j < MAPSIZE; j++)
            {
                MapData[i][j].Finish = false;
                MapData[i][j].HasRoad = false;
            }
        l = 0;

        x = MAPSIZE / 2;
        y = 0;

        angle = 0;

        while(1)
        {
            int tempx = x;
            int tempy = y;

            for(int i = 0; i <= distance; i++)
            {
                x = tempx;
                y = tempy;

                x += sin( (M_PI/180) * angle ) * i;
                y += cos( (M_PI/180) * angle ) * i;

                if(x < 0)
                    x = 0;
                if(x > MAPSIZE - 1)
                    x = MAPSIZE - 1;
                if(y < 0)
                    y = 0;
                if(y > MAPSIZE - 1)
                    y = MAPSIZE - 1;

                int szer = 4;

                for(int i = -szer + 1; i < szer; i++)
                {
                    for(int j = -szer + 1; j < szer; j++)
                    {
                        if( (x + i >= 0) && (x + i < MAPSIZE - 1) && (y + j >= 0) && (y + j < MAPSIZE - 1))
                            MapData[x + i][y + j].HasRoad = true;
                    }
                }
            }

            z = rand()%2;

            if(z > 0)
                angle += rand()%TURN_ANGLE;
            else
                angle -= rand()%TURN_ANGLE;

            distance = 1 + rand()%3;

            l++;

            if((x == 0) || (x == MAPSIZE - 1) || (y == 0) || (y == MAPSIZE - 1))
            {
                for(int i = -2; i < 3; i++)
                {
                    for(int j = -2; j < 3; j++)
                    {
                        if( (x + i >= 0) && (x + i < MAPSIZE - 1) && (y + j >= 0) && (y + j < MAPSIZE - 1))
                            MapData[x + i][y + j].Finish = true;
                    }
                }
                break;
            }
        }

        if(l > ROAD_LENGTH)
            break;
    }

    Player[0].X = MAPSIZE / 2 + 1;
    Player[0].Y = 0;
    Player[0].VX = 0;
    Player[0].VY = 0;
    Player[1].X = MAPSIZE / 2 - 1;
    Player[1].Y = 0;
    Player[1].VX = 0;
    Player[1].VY = 0;

    BuildRoadModel();

    for(int i = 0; i < MAPSIZE; i++)
    {
        for(int j = 0; j< MAPSIZE; j++)
        {
            if(MapData[i][j].HasRoad)
            {
                        MapData[i][j].Height = max(MapData[i][j].Height, WaterLevel - 0.01f);
                        MapData[i + 1][j].Height = max(MapData[i + 1][j].Height, WaterLevel - 0.01f);
                        MapData[i][j + 1].Height = max(MapData[i][j + 1].Height, WaterLevel - 0.01f);
                        MapData[i + 1][j + 1].Height = max(MapData[i + 1][j + 1].Height, WaterLevel - 0.01f);
            }
        }
    }

    BuildMapModel();
}

void RotateCar(car* Car)            //Funkcja obracają samochód wg jego wektora
{
    if((Car -> VY) != 0)
        Car -> Angle = atan((float)(Car -> VX) / (float)(Car -> VY)) * (180 / M_PI) - 90;
    else if((Car -> VX) != 0)
        Car -> Angle = min(0, (abs(Car -> VX)/(Car -> VX)) * 180);

    if(Car -> VY < 0)
        Car -> Angle += 180;
}

bool MovePlayer(car* Car)           //Funkcja przemieszczająca graczy wedle ich wektora i sprawdzająca poprawność ruchu
{
    int stepcount = 50;             //Dokładność sprawdzania poprawności ruchu

    float i = 0, j = 0;

    while(Dist(i,j) < Dist(Car -> VX, Car -> VY))           //Sprawdzanie poprawności ruchu
    {
        i += ((float)Car -> VX) / stepcount;
        j += ((float)Car -> VY) / stepcount;

        if(((Car -> X + (int)i) >= MAPSIZE) || ((Car -> X + (int)i) < 0) || ((Car -> Y + (int)j) >= MAPSIZE) || ((Car -> Y + (int)j) < 0))
            return false;

        if(!MapData[Car -> X + (int)i][Car -> Y + (int)j].HasRoad)
        {
            return false;
        }
    }

    if(((Car -> X + Car -> VX) >= 0) && ((Car -> X + Car -> VX) < MAPSIZE))
        Car -> X += Car -> VX;
    if(((Car -> Y + Car -> VY) >= 0) && ((Car -> Y + Car -> VY) < MAPSIZE))
        Car -> Y += Car -> VY;

    Car -> VX += Car -> CX;
    Car -> VY += Car -> CY;

    Car -> CX = 0;
    Car -> CY = 0;

    return true;
}

void ProcessGame()          //Główna pętla gry
{
    char str[64], str2[4][3], str3[64];

    for(int i = 0; i < PLAYER_COUNT; i++)
        RotateCar(&Player[i]);

    if(InGame)
    {
        if(MoveMade[PlayerID])
        {
            if(!MoveSent)           //Prześlij ruch drugiemu graczowi
            {
                itoa(Player[PlayerID].VX, str2[0], 10);
                itoa(Player[PlayerID].VY, str2[1], 10);
                itoa(Player[PlayerID].CX, str2[2], 10);
                itoa(Player[PlayerID].CY, str2[3], 10);

                strcpy(str, str2[0]);
                strcat(str, " ");
                strcat(str, str2[1]);
                strcat(str, " ");
                strcat(str, str2[2]);
                strcat(str, " ");
                strcat(str, str2[3]);

//                printf("\n  %s\n", str);

//                sendStringToPipe(potoki, str);

                MoveSent = true;
            }
        }

//        int result = getStringFromPipe(potoki, str3, 63);

//        if(result > 0)          //Jeśli otrzymaliśmy ruch od przeciwnika
//        {
//            printf("    %d\n",result);
/*
            switch(PlayerID)
            {
                case 0:
//                    sscanf(str3, "%d %d %d %d", &Player[1].VX, &Player[1].VY, &Player[1].CX, &Player[1].CY);
                    MoveMade[1] = true;
                break;
                case 1:
//                    sscanf(str3, "%d %d %d %d", &Player[0].VX, &Player[0].VY, &Player[0].CX, &Player[0].CY);
                    MoveMade[0] = true;
                break;
            }
//        }
*/
//        printf("%d\n",PlayerID);

        if(counter == 0)
        {
            MoveMade[PlayerID] = true;
            counter = CounterDelay;
        }
        else
            counter--;

        if(MoveMade[PlayerID])
        {
                    if(!MovePlayer(&Player[PlayerID]))        //Czyszczenie zmiennych i kończenie wyścigu
                    {
                        Player[0].X = MAPSIZE / 2 + 1;
                        Player[0].Y = 0;
                        Player[0].VX = 0;
                        Player[0].VY = 0;
                        Player[1].X = MAPSIZE / 2 - 1;
                        Player[1].Y = 0;
                        Player[1].VX = 0;
                        Player[1].VY = 0;

                        MenuMode = -1;
                        InGame = false;
                        WinnerID = 1 - PlayerID;
                        RaceOver = true;
                    }

            MoveMade[PlayerID] = false;

            PlayerID += 1;
            PlayerID %= 2;
        }

        if(MoveMade[1])      //Obaj gracze wykonali ruch, gra sprawdza ich poprawność
        {

                    if(!MovePlayer(&Player[1]))        //Czyszczenie zmiennych i kończenie wyścigu
                    {
                        Player[0].X = MAPSIZE / 2 + 1;
                        Player[0].Y = 0;
                        Player[0].VX = 0;
                        Player[0].VY = 0;
                        Player[1].X = MAPSIZE / 2 - 1;
                        Player[1].Y = 0;
                        Player[1].VX = 0;
                        Player[1].VY = 0;

                        MenuMode = -1;
                        InGame = false;
                        WinnerID = 0;
                        RaceOver = true;
                    }

            MoveMade[0] = false;
            MoveMade[1] = false;
            MoveSent = false;
        }

        for(int i = 0; i < PLAYER_COUNT; i++)   //Sprawdzanie, czy któryś z graczy dotarł do mety
        {
            if(MapData[Player[i].X][Player[i].Y].Finish)        //Czyszczenie zmiennych i kończenie wyścigu
            {
                Player[0].X = MAPSIZE / 2 + 1;
                Player[0].Y = 0;
                Player[0].VX = 0;
                Player[0].VY = 0;
                Player[0].CX = 0;
                Player[0].CY = 0;
                Player[1].X = MAPSIZE / 2 - 1;
                Player[1].Y = 0;
                Player[1].VX = 0;
                Player[1].VY = 0;
                Player[1].CX = 0;
                Player[1].CY = 0;

                MenuMode = -1;
                InGame = false;
                WinnerID = i;
                RaceOver = true;
            }
        }
    }

    int result = 1;
    char str4[65];

    if(RaceOver && (PlayerID == 1))             //Funkcja czyszcząca potoki
    {
//        while(result != 0)
//            result = getStringFromPipe(potoki, str4, 64);
    }

    glutPostRedisplay();                    //Odświeżanie wyświetlania gluta
}
