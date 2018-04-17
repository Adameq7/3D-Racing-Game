#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "wyscigi.h"
#include "fifo.h"

float CamPosX = 0, CamPosY = 128, CamPosZ = -80, CamAngle = 0, CamAngleY = 0, CamTargetX, CamTargetY, CamTargetZ, CamDistance = 30.0f;  //Zmienne do obsługi kamery
float WaterLevel = 0;   //Poziom wody na mapie
car Player[2];  //Dane samochodów graczy
int Width = 1920;    //Szerokość okna
int Height = 1080;   //Wysokość okna
int PlayerID = 0;   //ID aktualnego gracza
int CurrentTurn = 0;    //Czyja tura w trakcie gry
int MenuMode = 0;   //W jakim menu obecnie jesteśmy
bool InGame = false; //Czy jesteśmy w trakcie właściwej rozgrywki
PipesPtr potoki;

int main(int argc, char **argv)
{
    srand(time(0)); //Losujemy liczby

    int x = rand()%4;

//    x = 2;
/*
    switch(x) //Eurobeat intensifies
    {
        case 0:
            PlayMusic("90s.mp3");
        break;
        case 1:
            PlayMusic("gas.mp3");
        break;
        case 2:
            PlayMusic("deja.mp3");
        break;
        case 3:
            PlayMusic("beat.mp3");
        break;
    }
*/
    Player[0].R = 1.0f; //Gracz 1 ma kolor czerwony
    Player[0].G = 0.0f;
    Player[0].B = 0.0f;
    Player[1].G = 1.0f; //Gracz 2 ma kolor zielony
    Player[1].R = 0.0f;
    Player[1].B = 0.0f;

    LoadFonts();    //Ładujemy czcionkę

    PrepareOptions();   //Przygotowujemy nazwy opcji w menu

    LoadModel(0, "Models\\CarChassis.obj");     //Ładujemy modele do pamięci
    LoadModel(1, "Models\\CarAttachments.obj");
    LoadModel(2, "Models\\Marker.obj");
    LoadModel(3, "Models\\Arrow.obj");

    GenerateMap();  //Generujemy mapę
    GenerateRoad(); //Generujemy drogę

    CamAngle = 180; //Domyślne ustawienie kamery
    CamAngleY = 90;

    glutEnterGameMode();

	glutInit(&argc, argv);      //Funkcje przygotowujące GLUT-a do działania
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowPosition(-1, -1);
//	glutInitWindowSize(Width, Height);
//    glutCreateWindow("Wyscigi");
    glutGameModeString("1920x1080:32");
    glutEnterGameMode();

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(ProcessGame);
	glutKeyboardFunc(ProcessNormalKeys);
	glutSpecialFunc(ProcessSpecialKeys);
	glutPassiveMotionFunc(ProcessMouseMovement);

	glutMainLoop(); //Główna pętla programu

	return 1;
}
