#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/soil.h>
#include "wyscigi.h"

extern unsigned char* Font;

void DrawLetter(int Letter)             //Funkcja rysująca pojedynczą literę
{
    glRotatef(180, 1, 0, 0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0625f * (float)(Letter%16), 0.0625f * floor((float)Letter/16)); glVertex2f(0.0f, 0.0f);  // Point 1 (Front)
        glTexCoord2f(0.0625f * ((float)(Letter%16) + 1), 0.0625f * floor((float)Letter/16)); glVertex2f( LETTER_SIZE, 0.0f);  // Point 2 (Front)
        glTexCoord2f(0.0625f * ((float)(Letter%16) + 1), 0.0625f * (1 + floor((float)Letter/16))); glVertex2f( LETTER_SIZE,  LETTER_SIZE);  // Point 3 (Front)
        glTexCoord2f(0.0625f * (float)(Letter%16), 0.0625f * (1 + floor((float)Letter/16))); glVertex2f(0.0f,  LETTER_SIZE);  // Point 4 (Front)
    glEnd();
    glRotatef(-180, 1, 0, 0);
};

void DrawText(float X, float Y, char Text[64], bool Selected)               //Funkcja wypisujący dany tekst
{
    int Length = strlen(Text);

    if(Selected)
        glColor3f(1.0f, 1.0f, 0.0f);
    else
        glColor3f(1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, Font);

    int CurrentLetter = 0;

    float lwidth = 0.066;

    glTranslatef(X - ((float)Length / 2) * lwidth, Y, 0.0f);

    do{
        switch(Text[CurrentLetter])
        {
            case '?':
            DrawLetter(31);
            break;
            case '_':
            DrawLetter(0);
            break;
            case 'a':
            DrawLetter(65);
            break;
            case 'b':
            DrawLetter(66);
            break;
            case 'c':
            DrawLetter(67);
            break;
            case 'd':
            DrawLetter(68);
            break;
            case 'e':
            DrawLetter(69);
            break;
            case 'f':
            DrawLetter(70);
            break;
            case 'g':
            DrawLetter(71);
            break;
            case 'h':
            DrawLetter(72);
            break;
            case 'i':
            DrawLetter(73);
            break;
            case 'j':
            DrawLetter(74);
            break;
            case 'k':
            DrawLetter(75);
            break;
            case 'l':
            DrawLetter(76);
            break;
            case 'm':
            DrawLetter(77);
            break;
            case 'n':
            DrawLetter(78);
            break;
            case 'o':
            DrawLetter(79);
            break;
            case 'p':
            DrawLetter(80);
            break;
            case 'q':
            DrawLetter(81);
            break;
            case 'r':
            DrawLetter(82);
            break;
            case 's':
            DrawLetter(83);
            break;
            case 't':
            DrawLetter(84);
            break;
            case 'u':
            DrawLetter(85);
            break;
            case 'v':
            DrawLetter(86);
            break;
            case 'w':
            DrawLetter(87);
            break;
            case 'x':
            DrawLetter(88);
            break;
            case 'y':
            DrawLetter(89);
            break;
            case 'z':
            DrawLetter(90);
            break;
            case 'A':
            DrawLetter(33);
            break;
            case 'B':
            DrawLetter(34);
            break;
            case 'C':
            DrawLetter(35);
            break;
            case 'D':
            DrawLetter(36);
            break;
            case 'E':
            DrawLetter(37);
            break;
            case 'F':
            DrawLetter(38);
            break;
            case 'G':
            DrawLetter(39);
            break;
            case 'H':
            DrawLetter(40);
            break;
            case 'I':
            DrawLetter(41);
            break;
            case 'J':
            DrawLetter(42);
            break;
            case 'K':
            DrawLetter(43);
            break;
            case 'L':
            DrawLetter(44);
            break;
            case 'M':
            DrawLetter(45);
            break;
            case 'N':
            DrawLetter(46);
            break;
            case 'O':
            DrawLetter(47);
            break;
            case 'P':
            DrawLetter(48);
            break;
            case 'Q':
            DrawLetter(49);
            break;
            case 'R':
            DrawLetter(50);
            break;
            case 'S':
            DrawLetter(51);
            break;
            case 'T':
            DrawLetter(52);
            break;
            case 'U':
            DrawLetter(53);
            break;
            case 'V':
            DrawLetter(54);
            break;
            case 'W':
            DrawLetter(55);
            break;
            case 'X':
            DrawLetter(56);
            break;
            case 'Y':
            DrawLetter(57);
            break;
            case 'Z':
            DrawLetter(58);
            break;
            case '0':
            DrawLetter(16);
            break;
            case '1':
            DrawLetter(17);
            break;
            case '2':
            DrawLetter(18);
            break;
            case '3':
            DrawLetter(19);
            break;
            case '4':
            DrawLetter(20);
            break;
            case '5':
            DrawLetter(21);
            break;
            case '6':
            DrawLetter(22);
            break;
            case '7':
            DrawLetter(23);
            break;
            case '8':
            DrawLetter(24);
            break;
            case '9':
            DrawLetter(25);
            break;
            case '!':
            DrawLetter(1);
            break;
            case '"':
            DrawLetter(2);
            break;
            case '#':
            DrawLetter(3);
            break;
            case '$':
            DrawLetter(4);
            break;
            case '%':
            DrawLetter(5);
            break;
            case '&':
            DrawLetter(6);
            break;
            case '(':
            DrawLetter(8);
            break;
            case ')':
            DrawLetter(9);
            break;
            case '*':
            DrawLetter(10);
            break;
            case '+':
            DrawLetter(11);
            break;
            case '-':
            DrawLetter(118);
            break;
            case '/':
            DrawLetter(15);
            break;
            case '.':
            DrawLetter(14);
            break;
/*
            case 'ą':
            DrawLetter(96);
            break;
            case 'ć':
            DrawLetter(97);
            break;
            case 'ę':
            DrawLetter(98);
            break;
            case 'ł':
            DrawLetter(99);
            break;
            case 'ń':
            DrawLetter(100);
            break;
            case 'ó':
            DrawLetter(101);
            break;
            case 'ś':
            DrawLetter(102);
            break;
            case 'ź':
            DrawLetter(103);
            break;
            case 'ż':
            DrawLetter(104);
            break;
            case 'Ą':
            DrawLetter(112);
            break;
            case 'Ć':
            DrawLetter(113);
            break;
            case 'Ę':
            DrawLetter(114);
            break;
            case 'Ł':
            DrawLetter(115);
            break;
            case 'Ń':
            DrawLetter(116);
            break;
            case 'Ó':
            DrawLetter(117);
            break;
            case 'Ś':
            DrawLetter(118);
            break;
            case 'Ź':
            DrawLetter(119);
            break;
            case 'Ż':
            DrawLetter(120);
            break;
*/
        };

        glTranslatef(lwidth, 0.0f, 0.0f);

        CurrentLetter++;

    }while(CurrentLetter < Length);

    glTranslatef(-Length*lwidth, 0.0f, 0.0f);

    glTranslatef(-X + ((float)Length / 2) * lwidth, -Y, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
};
