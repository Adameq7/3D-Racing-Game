#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include "wyscigi.h"

float MarkerRotateAngle = 0;        //Ta i zmienna poniżej są wykorzystywane przy rysowaniu znacznika położenia graczy
float NewMoveAngle[2];
option MenuOptions[16];             //Nazwy opcji w menu
int CurrentOption;                  //Aktualna opcja w menu
bool RestartMapPrompt = false;      //Zmienna wykorzystywana przy wczytywaniu mapy
bool OverwriteSavePrompt = false;   //Zmienna wykorzystywana przy zapisywaniu mapy
bool RestartMap = false;            //Zmienna wykorzystywana przy wczytywaniu mapy
bool OverwriteSave = false;         //Zmienna wykorzystywana przy zapisywaniu mapy
bool CannotLoadPrompt = false;      //Zmienna wykorzystywana przy wczytywaniu mapy
bool RaceOver = false;              //Czy wyścig się zakończył
bool PipesSet = false;              //Czy potoki zostały przygotowane
int WinnerID;                       //Kto wygrał wyścig
int GoBackTo = 1;                   //Do którego menu wrócić z menu zapisywania
GLuint font;                        //Czcionka
GLfloat MenuCamAngle = 0;           //Kąt kamery w menu

extern float CamPosX, CamPosY, CamPosZ, CamAngle, CamAngleY, CamTargetX, CamTargetY, CamTargetZ, CamDistance;
extern int Width, Height;
extern int RoadIndicesCount, TerrainIndicesCount;
extern int PlayerID;
extern int MenuMode;
extern bool InGame;
extern bool RestartMap, OverwriteSave, RestartMapPrompt, OverwriteSavePrompt, CannotLoadPrompt, RaceOver, MoveMade[2], PipesSet;
extern bool EnterPressed, EscapePressed;

extern tile MapData[MAPSIZE][MAPSIZE];
extern GLfloat TerrainVertices[MAPSIZE * MAPSIZE * 12];
extern GLubyte TerrainColors[MAPSIZE * MAPSIZE * 12];
extern unsigned int TerrainIndices[MAPSIZE * MAPSIZE * 4];
extern GLfloat RoadVertices[MAPSIZE * MAPSIZE * 12];
extern GLubyte RoadColors[MAPSIZE * MAPSIZE * 12];
extern unsigned int RoadIndices[MAPSIZE * MAPSIZE * 4];
extern GLfloat ModelVertices[ModelCount][MAX_VERTEX_COUNT];
extern unsigned int ModelIndices[ModelCount][MAX_VERTEX_COUNT * 3];
extern float WaterLevel;
extern car Player[PLAYER_COUNT];
extern PipesPtr potoki;

void DrawFinishScreen(int ID)       //Funkcja wyświetla ekran z informacją o tym, który gracz wygrał wyścig
{
    DrawTextPrep();
    char str[64], str2[2];
    itoa(ID + 1, str2, 10);

    strcpy(str, "Wygral gracz ");
    strcat(str, str2);
    strcat(str, ".");

    DrawText(0.0f, 0.0f, str, false);
}

void DrawMapInMenu()                //Funkcja rysuje mapę w tle menu
{
            glLoadIdentity();
            glDisable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            //Obliczenie pozycji kamery

            CamPosX = MAPSIZE / 2 + (sin(MenuCamAngle * (M_PI / 180)) * 120) * cos(60 * (M_PI / 180));
            CamPosY = MapData[MAPSIZE / 2][MAPSIZE / 2].Height + sin(60 * (M_PI / 180)) * 120;
            CamPosZ = MAPSIZE / 2 + (cos(MenuCamAngle * (M_PI / 180)) * 120) * cos(60 * (M_PI / 180));

            //Obracamy kamerę wokół środka mapy

            MenuCamAngle += 0.166f;

            gluLookAt(CamPosX, CamPosY, CamPosZ, MAPSIZE / 2, 15, MAPSIZE / 2, 0, 1, 0);

            //Rysujemy mapę

            DrawWater();
            DrawGround();
            DrawRoad();
            DrawPlayers();
}

void DrawTextPrep()                 //Funkcja przygotowuje OpenGL-a do rysowania tekstu
{
            glLoadIdentity();
            glEnable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            gluLookAt(0,0,1.0,0,0,0,0,1,0);
}

void DrawPlayers()                  //Funkcja rysuje modele graczy na mapie, oraz ich wektory przemieszczenia i znaczniki
{
        //Wskaźnik wektora obraca się wokół własnej osi

        MarkerRotateAngle += 0.5f;

        for(int i = 0; i < PLAYER_COUNT; i++)
        {
            float TransY = (MapData[Player[i].X + Player[i].VX][Player[i].Y + Player[i].VY].Height + MapData[Player[i].X + Player[i].VX + 1][Player[i].Y + Player[i].VY].Height + MapData[Player[i].X + Player[i].VX + 1][Player[i].Y + Player[i].VY + 1].Height + MapData[Player[i].X + Player[i].VX][Player[i].Y + Player[i].VY + 1].Height) / 4 + 0.1f;

            point P1, P2;
            color C;

            C.R = 0.0f;
            C.G = 1.0f;
            C.B = 0.0f;

            P1.X = Player[i].X + 0.5f;
            P1.Y = 0.1f + (MapData[Player[i].X][Player[i].Y].Height + MapData[Player[i].X + 1][Player[i].Y].Height + MapData[Player[i].X + 1][Player[i].Y + 1].Height + MapData[Player[i].X][Player[i].Y + 1].Height) / 4 + 0.1f;
            P1.Z = Player[i].Y + 0.5f;

            P2.X = Player[i].X + Player[i].VX + 0.5f;
            P2.Y = 0.3f + 0.15f * sin(MarkerRotateAngle * (M_PI / 180)) + TransY;
            P2.Z = Player[i].Y + Player[i].VY + 0.5f;

            DrawLine(P1, P2, C);            //Rysowanie przebiegu wektora przemieszczenia

            for(int i = 0; i < 2; i++)
            switch(Player[i].CX)     //Rysowanie strzałki pokazującej zmianę wektora w następnym ruchu
            {
                case -1:
                    switch(Player[i].CY)
                    {
                        case 1:
                            NewMoveAngle[i] = 225;
                        break;
                        case 0:
                            NewMoveAngle[i] = 180;
                        break;
                        case -1:
                            NewMoveAngle[i] = 135;
                        break;
                    }
                break;
                case 0:
                    switch(Player[i].CY)
                    {
                        case 1:
                            NewMoveAngle[i] = 270;
                        break;
                        case -1:
                            NewMoveAngle[i] = 90;
                        break;
                    }
                break;
                case 1:
                    switch(Player[i].CY)
                    {
                        case 1:
                            NewMoveAngle[i] = 315;
                        break;
                        case 0:
                            NewMoveAngle[i] = 0;
                        break;
                        case -1:
                            NewMoveAngle[i] = 45;
                        break;
                    }
                break;
            }

            //Rysowanie modeli samochodów i znacznika

            DrawModel(0, Player[i].X, Player[i].Y, 0, Player[i].R, Player[i].G, Player[i].B, Player[i].Angle, true);
            DrawModel(1, Player[i].X, Player[i].Y, 0, 0, 0, 0, Player[i].Angle, true);
            DrawModel(2, Player[i].X + Player[i].VX, Player[i].Y + Player[i].VY, TransY + 0.15f * sin(MarkerRotateAngle * (M_PI / 180)), Player[i].R, Player[i].G, Player[i].B, MarkerRotateAngle, false);
            DrawModel(3, Player[i].X + Player[i].VX, Player[i].Y + Player[i].VY, TransY, 0.0f, 1.0f, 0.0f, NewMoveAngle[i] - 90, true);

            //Rysowanie strzałki przy znaczniku

//            if(!(Player[PlayerID].CX == 0 && Player[PlayerID].CY == 0))
        }
}

void DrawOptions()              //Funkcja odpowiadająca za wyświetlanie i obsługę menu
{
    int j;              //j to zmienna używana przy wyborze opcji w menu
    bool result;
    char str[11], str2[3], str4[6];

    switch(MenuMode)            //Główny switch menu
    {
        case 0:                 //Główne Menu
            DrawMapInMenu();
            DrawTextPrep();

            if(CurrentOption < 0)
                CurrentOption = 0;
            if(CurrentOption > 2)
                CurrentOption = 2;

            j = 0;
            for(int i = 0; i < 3; i++)
            {
                DrawText(0, -(float)(i) * 0.2f, MenuOptions[i].Name, (CurrentOption == j) ? true : false );
                j++;
            }

            if(EnterPressed)
            {
                switch(CurrentOption)
                {
                    case 0:
                        MenuMode = 1;
                        CurrentOption = 0;
                    break;
                    /*
                    case 1:
                        PlayerID = 1;

                        switch(PipesSet)
                        {
                            case false:
                                potoki = initPipes(PlayerID);
                                PipesSet = true;

                                LoadGame("Current.txt");

                                BuildMapModel();
                                BuildRoadModel();

                                CamAngle = 180;
                                InGame = true;
                                MenuMode = -1;
                            break;
                            case true:
                                result = getStringFromPipe(potoki, str4, 3);

                                printf("    %d\n", result);

                                if((result != 0))
                                {
                                    LoadGame("Current.txt");

                                    BuildMapModel();
                                    BuildRoadModel();

                                    CamAngle = 180;
                                    InGame = true;
                                    MenuMode = -1;
                                }
                            break;
                        }
                    break;
                    */
                    case 2:
                        exit(0);
                    break;
                }

                EnterPressed = false;
            }

            if(EscapePressed)
            {
                EscapePressed = false;
                exit(0);
            }
        break;
        case 1:                 //Menu gracza "serwera"
            DrawMapInMenu();
            DrawTextPrep();

            GoBackTo = 1;       //Tutaj będziemy chcieli wrócić po zapisaniu gry

            if(CurrentOption < 0)
                CurrentOption = 0;
            if(CurrentOption > 4)
                CurrentOption = 4;

            j = 0;
            for(int i = 3; i < 8; i++)
            {
                DrawText(0, -(float)(j) * 0.1f, MenuOptions[i].Name, (CurrentOption == j) ? true : false );
                j++;
            }

            if(EnterPressed)
            {
                switch(CurrentOption)
                {
                    case 0:
                        CamAngle = 180;
                        InGame = true;
                        MenuMode = -1;

//                        SaveGame("Current.txt");

                        PlayerID = 0;

                        switch(PipesSet)
                        {
                            case false:
                            //    potoki = initPipes(PlayerID);
                                PipesSet = true;
                            break;
                            case true:
                            //    sendStringToPipe(potoki, "R");
                            break;
                        }

                    break;
                    case 1:
                        GenerateMap();
                        GenerateRoad();
                    break;
                    case 2:
                        MenuMode = 2;
                        CurrentOption = 0;
                    break;
                    case 3:
                        MenuMode = 3;
                        CurrentOption = 0;
                    break;
                    case 4:
                        MenuMode = 0;
                        CurrentOption = 0;
                    break;
                }

                EnterPressed = false;
            }

            if(EscapePressed)
            {
                MenuMode = 0;
                EscapePressed = false;
            }
        break;
        case 2:                 //Menu zapisywania
            DrawMapInMenu();
            DrawTextPrep();

            if(CurrentOption < 0)
                CurrentOption = 0;
            if(CurrentOption > 99)
                CurrentOption = 99;

            j = 0;

            if(!OverwriteSavePrompt)
            {
                for(int i = max(0, CurrentOption - 8); i < max(9, CurrentOption + 1); i++)
                {
                    itoa(i, str2, 10);

                    strcpy(str, "Save");
                    strcat(str, str2);
                    strcat(str, ".txt");

                    DrawText(-0.5f, 0.4f-(float)(j) * 0.1f, str, (CurrentOption == j || (CurrentOption > 8 && j == 8)) ? true : false);
                    j++;

                    str[0] = '\0';
                }
            }
            else
            {
                DrawText(0.0f, 0.3f, "Czy na pewno chcesz", false);
                DrawText(0.0f, 0.2f, "nadpisac zapis?", false);
                DrawText(-0.25f, 0.0f, "Tak", OverwriteSave ? true : false);
                DrawText(0.25f, 0.0f, "Nie", OverwriteSave ? false : true);
            }

            itoa(CurrentOption, str2, 10);

            strcpy(str, "Save");
            if(CurrentOption < 10)
                strcat(str, "0");
            strcat(str, str2);
            strcat(str, ".txt");

            if(EnterPressed)
            {
                if(!OverwriteSavePrompt)
                {
                    if(fopen(str, "r") == NULL)
                    {
                        SaveGame(str);
                    }
                    else
                    {
                        OverwriteSavePrompt = true;
                        OverwriteSave = false;
                    }
                }
                else
                {
                    if(OverwriteSave)
                        SaveGame(str);
                    OverwriteSavePrompt = false;
                }
                EnterPressed = false;
            }

            if(EscapePressed)
            {
                MenuMode = GoBackTo;
                EscapePressed = false;
            }
        break;
        case 3:                 //Menu wczytywania
            DrawMapInMenu();
            DrawTextPrep();

            if(CurrentOption < 0)
                CurrentOption = 0;
            if(CurrentOption > 99)
                CurrentOption = 99;

            j = 0;

            if(CannotLoadPrompt)
                DrawText(0.0f, 0.5f, "Nie mozna wczytac zapisu.", false);

            if(!RestartMapPrompt)
            {
                for(int i = max(0, CurrentOption - 8); i < max(9, CurrentOption + 1); i++)
                {
                    itoa(i, str2, 10);

                    strcpy(str, "Save");
                    strcat(str, str2);
                    strcat(str, ".txt");

                    DrawText(-0.5f, 0.4f-(float)(j) * 0.1f, str, (CurrentOption == j || (CurrentOption > 8 && j == 8)) ? true : false);
                    j++;

                    str[0] = '\0';
                }
            }
            else
            {
                DrawText(0.0f, 0.3f, "Czy przywrocic mape", false);
                DrawText(0.0f, 0.2f, "do stanu startowego?", false);
                DrawText(-0.25f, 0.0f, "Tak", RestartMap ? true : false);
                DrawText(0.25f, 0.0f, "Nie", RestartMap ? false : true);
            }

            itoa(CurrentOption, str2, 10);

            strcpy(str, "Save");
            if(CurrentOption < 10)
                strcat(str, "0");
            strcat(str, str2);
            strcat(str, ".txt");

            if(EnterPressed)
            {
                if(!RestartMapPrompt)
                {
                    if(fopen(str, "r") == NULL)
                    {
                        CannotLoadPrompt = true;
                    }
                    else
                    {
                        LoadGame(str);
                        BuildMapModel();
                        BuildRoadModel();
                        RestartMapPrompt = true;
                        RestartMap = false;
                    }
                }
                else
                {
                    if(RestartMap)
                    {
                        Player[0].X = MAPSIZE / 2 + 1;
                        Player[0].Y = 0;
                        Player[0].VX = 0;
                        Player[0].VY = 0;
                        Player[1].X = MAPSIZE / 2 - 1;
                        Player[1].Y = 0;
                        Player[1].VX = 0;
                        Player[1].VY = 0;
                    }

                    RestartMapPrompt = false;

                    MenuMode = 1;
                    CurrentOption = 0;
                }

                EnterPressed = false;
            }

            if(EscapePressed)
            {
                MenuMode = 1;
                EscapePressed = false;
            }
        break;
        case 4:                 //Menu pauzy w trakcie gry
            DrawMapInMenu();
            DrawTextPrep();

            if(CurrentOption < 0)
                CurrentOption = 0;
            if(CurrentOption > 2)
                CurrentOption = 2;

            j = 0;
            for(int i = 8; i < 11; i++)
            {
                DrawText(0, -(float)(j) * 0.1f, MenuOptions[i].Name, (CurrentOption == j) ? true : false );
                j++;
            }

            if(EnterPressed)
            {
                switch(CurrentOption)
                {
                    case 0:
                        InGame = true;
                        MenuMode = -1;
                    break;
                    case 1:
                        MenuMode = 2;
                    break;
                    case 2:
                        MenuMode = 1;
                    break;
                }

                EnterPressed = false;
            }

            if(EscapePressed)
            {
                InGame = true;
                MenuMode = -1;
                EscapePressed = false;
            }
        break;
    }
}

void DrawGround()               //Funkcja rysująca właściwą mapę
{
            glEnableClientState( GL_COLOR_ARRAY );
            glEnableClientState( GL_VERTEX_ARRAY );

            glColorPointer( 3, GL_UNSIGNED_BYTE, 0, TerrainColors );
            glVertexPointer( 3, GL_FLOAT, 0, TerrainVertices );
            glDrawElements( GL_QUADS, TerrainIndicesCount, GL_UNSIGNED_INT, TerrainIndices );
}

void DrawRoad()                 //Funkcja rysująca drogę na mapie
{
            glEnableClientState( GL_COLOR_ARRAY );
            glEnableClientState( GL_VERTEX_ARRAY );

            glColorPointer( 3, GL_UNSIGNED_BYTE, 0, RoadColors );
            glVertexPointer( 3, GL_FLOAT, 0, RoadVertices );
            glDrawElements( GL_QUADS, RoadIndicesCount, GL_UNSIGNED_INT, RoadIndices );

            glTranslatef(0.0f, 0.01f, 0.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor3f(1.0f, 1.0f, 1.0f);

            glDisableClientState( GL_COLOR_ARRAY );
            glEnableClientState( GL_VERTEX_ARRAY );

            glVertexPointer( 3, GL_FLOAT, 0, RoadVertices );
            glDrawElements( GL_QUADS, RoadIndicesCount, GL_UNSIGNED_INT, RoadIndices );

            glTranslatef(0.0f, -0.01f, 0.0f);
}

void DrawModel(int i, int X, int Y, float Z, float R, float G, float B, float Angle3, bool OnGround)            //Funkcja rysująca podany model w podanym miejscu i o podanym kolorze; można ustawić także jego obrót
{
    glColor3f(R, G, B);

    GLfloat Angle1 = atan((MapData[X + 1][Y].Height + MapData[X + 1][Y + 1].Height) / 2 - (MapData[X][Y].Height + MapData[X][Y + 1].Height) / 2) * (180 / M_PI) / TILESIZE;
    GLfloat Angle2 = atan((MapData[X][Y + 1].Height + MapData[X + 1][Y + 1].Height) / 2 - (MapData[X][Y].Height + MapData[X + 1][Y].Height) / 2) * (180 / M_PI) / TILESIZE;

    GLfloat TransY;     //O ile model zostanie przeniesiony w pionie

    GLfloat TransX = (float)X * TILESIZE + TILESIZE / 2;            //Przeniesienie w osi X
    if(OnGround)
        TransY = (MapData[X][Y].Height + MapData[X + 1][Y].Height + MapData[X + 1][Y + 1].Height + MapData[X][Y + 1].Height) / 4 + 0.1f;
    else
        TransY = Z;
    GLfloat TransZ = (float)Y * TILESIZE + TILESIZE / 2;            //Przeniesienie w osi Y

    glTranslatef(TransX, TransY, TransZ);

    if(OnGround)                //Model dostosuje się do podłoża
    {
        glRotatef(Angle1, 0, 0, 1);
        glRotatef(-Angle2, 1, 0, 0);
    }
    glRotatef(Angle3, 0, 1, 0);

    glDisableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, ModelVertices[i] );
    glDrawElements( GL_TRIANGLES, ModelIndices[i][MAX_VERTEX_COUNT * 3 - 1] * 3, GL_UNSIGNED_INT, ModelIndices[i] );

    glRotatef(-Angle3, 0, 1, 0);

    if(OnGround)
    {
        glRotatef(Angle2, 1, 0, 0);
        glRotatef(-Angle1, 0, 0, 1);
    }

    glTranslatef(-TransX, -TransY, -TransZ);
}

void DrawWater()    //Funkcja rysuje wodę na mapie
{
        glColor3f(0.0f, 0.33f, 1.0f);

        glBegin(GL_QUADS);
            glVertex3f(0.0f, WaterLevel, 0.0f);
            glVertex3f(MAPSIZE * TILESIZE, WaterLevel, 0.0f);
            glVertex3f(MAPSIZE * TILESIZE, WaterLevel, MAPSIZE * TILESIZE);
            glVertex3f(0.0f, WaterLevel, MAPSIZE * TILESIZE);
        glEnd();

}

void DrawLine(point P1, point P2, color C)      //Funkcja rysująca prostą linię pomiędzy dwoma punktami w przestrzeni
{
    glColor3f(C.R, C.G, C.B);

    glBegin(GL_LINES);
        glVertex3f(P1.X, P1.Y, P1.Z);
        glVertex3f(P2.X, P2.Y, P2.Z);
    glEnd();
}

void RenderScene(void)             //Główna funkcja rysująca
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc(GL_GREATER,0.4);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)Width / (float)Height, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);

//	gluPerspective(60.0f, (float)Width / (float)Height, 0.1f, 1000.0f);

    if(InGame)          //Rozgrywka właściwa
    {
        if(EscapePressed)
        {
            if(PlayerID == 0)          //Menu pauzy
            {
                MenuMode = 4;
                GoBackTo = 4;       //Po zapisaniu gry będziemy chcieli wrócić do menu pauzy
                InGame = false;
            }

            EscapePressed = false;
        }

        glViewport(0, 0, Width/2, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(Width / 2) / (float)Height, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        if(CamAngleY > 89.99)
            CamAngleY = 89.99;
        if(CamAngleY < -89.99)
            CamAngleY = -89.99;

        CamPosX = Player[1].X + (sin(CamAngle * (M_PI / 180)) * CamDistance) * cos(CamAngleY * (M_PI / 180));
        CamPosY = MapData[Player[1].X][Player[1].Y].Height + sin(CamAngleY * (M_PI / 180)) * CamDistance;
        CamPosZ = Player[1].Y + (cos(CamAngle * (M_PI / 180)) * CamDistance) * cos(CamAngleY * (M_PI / 180));

        CamTargetX = Player[1].X;
        CamTargetY = MapData[Player[1].X][Player[1].Y].Height;
        CamTargetZ = Player[1].Y;

        gluLookAt(CamPosX, CamPosY, CamPosZ, CamTargetX, CamTargetY,  CamTargetZ, 0.0f, 1.0f,  0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DrawPlayers();
        DrawWater();
        DrawGround();
        DrawRoad();

        glViewport(Width/2, 0, Width/2, Height);

        glLoadIdentity();

        if(CamAngleY > 89.99)
            CamAngleY = 89.99;
        if(CamAngleY < -89.99)
            CamAngleY = -89.99;

        CamPosX = Player[0].X + (sin(CamAngle * (M_PI / 180)) * CamDistance) * cos(CamAngleY * (M_PI / 180));
        CamPosY = MapData[Player[0].X][Player[0].Y].Height + sin(CamAngleY * (M_PI / 180)) * CamDistance;
        CamPosZ = Player[0].Y + (cos(CamAngle * (M_PI / 180)) * CamDistance) * cos(CamAngleY * (M_PI / 180));

        CamTargetX = Player[0].X;
        CamTargetY = MapData[Player[0].X][Player[0].Y].Height;
        CamTargetZ = Player[0].Y;

        gluLookAt(CamPosX, CamPosY, CamPosZ, CamTargetX, CamTargetY,  CamTargetZ, 0.0f, 1.0f,  0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DrawPlayers();
        DrawWater();
        DrawGround();
        DrawRoad();
    }

    if(MenuMode != -1)
        DrawOptions();

    if(RaceOver)
    {
        DrawFinishScreen(WinnerID);

        if(EnterPressed || EscapePressed)
        {
            if(PlayerID == 0)
                MenuMode = 1;
            else
                MenuMode = 0;

            RaceOver = false;
            EnterPressed = false;
            EscapePressed = false;
        }
    }

    glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}

void ChangeSize(int w, int h)   //Funkcja dostosowująca viewport OpenGL-a do (zmieniającego się) okna
{
	if (h == 0)
		h = 1;

    Width = w;
    Height = h;

	float ratio =  (float)w / (float)h;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(60.0f, ratio, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
}
