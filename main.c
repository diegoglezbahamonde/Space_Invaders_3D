#define OVR_OS_WIN32

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "ovrs_lib.h"

// OBSERVADOR
double paso0, alfa;
double eye_X,eye_Y,eye_Z;
const int ANCHO_NAVE = 2;

// MUNDO
const float MAX_EJE_X = 120;
const float MAX_EJE_Y = 120;
const float MAX_EJE_Z = 7;
float restriccion_eje_y = 10;
float restriccion_eje_x = 2;

//COLORES
const float grey[] = {(float)0.8, (float)0.8, (float)0.8, 1.0};
const float white[] = {1.0, 1.0, 1.0, 1.0};
const float blackgrey[] = {0.5,0.5,0.5, 1.0};
const float red[] = {1, 0.0, 0.0, 1};
const float black[] = {0,0,0,1};
const float blue[] = {0.0, 0.0, 1, 1};
const float green[] = {0.0, 1.0, 0, 1};
const float yellow[] = {0.0, 1.0, 1.0, 1};
const float azulito[] = {1.0, 1.0, 0, 1};
const float rose[] = {1, 0.0, 1.0, 1};


// OVNI
const int ALTURA_OVNI = 5, ANCHURA_OVNI = 5, PROFUNDIDAD_OVNI = 20;
const int DESPLAZ_OVNI = 1;
const int MAX_MS_OVNI = 10000, MIN_MS_OVNI = 5000;
int ovniVisible = 0;
int ovniPosX, ovniPosY;
// FIN OVNI

// MARCIANOS
const int ANCHURA_MARCIANO = 3, ALTURA_MARCIANO = 5;
const int DESPLAZ_MARCIANO_X = 3, DESPLAZ_MARCIANO_Y = 5;
const int SEPARA_MARCIANO_X = 2, SEPARA_MARCIANO_Y = 5;
const int NUM_MARCIANOS_FILA = 11, NUM_FILAS = 5;
const int TIMER_MARCIANO = 450;
typedef struct {
    int marcianoPosX;
    int marcianoPosY;
    int muerto;
    int tipo;
} Marciano;

Marciano marciano[5][11];
int sentido = 0;
// FIN MARCIANOS

// DISPARO MARCIANO
typedef struct  {
    int posX;
    int posY;
} DisparoMarciano;
const int NUM_DISPAROS = 3, MIN_FEC_DISPARO = 500, MAX_FEC_DISPARO = 1200;
const int DESPLAZ_DISPARO_MAR = 2;
const float ANCHURA_DISPARO_MAR = 0.65;
const int MIN_FEC_ABSOLUTA_DISPARO = 200, FEC_DISPARO_NIVEL = 100;
DisparoMarciano disparos[3];
// FIN DISPARO MARCIANO


// DISPARO OBSERVADOR
const float PROFUNDIDAD_DISPARO = 3, ANCHO_DISPARO = 0.5;
const int DESPLAZ_DISPARO = 6;
const int DISPARO_POSZ = 5;
int disparoPosX = -1, disparoPosY = -1;
// FIN DISPARO OBSERVADOR

// CASA
const float ANCHO_CASA = 6  , ALTO_CASA = 3;
const float DISTANCIA_ENTRECASAS = 29, DISTANCIA_OBSERVADORCASA = 12, DISTANCIA_CASA_X = 15;
typedef struct {
    int casaPosX;
    int casaPosY;
    int toques;
} BloqueCasa;
BloqueCasa bloques[3][12];
// FIN CASA

// JUGADOR
typedef struct  {
    int nivel;
    int puntuacion;
    int vidas;
} Jugador;
Jugador jugador1;
// FIN JUGADOR

// AUDIO
Mix_Chunk *soundDisparoNave;
Mix_Chunk *soundDisparoMarciano;
Mix_Chunk *soundMuerteMarciano;
Mix_Chunk *soundOvniVisto;
Mix_Chunk *soundNaveDestruida;
int sonidoHabilitado;
typedef enum {DISPARO_NAVE_S, DISPARO_MARCIANO_S, MUERTE_MARCIANO_S, OVNI_VISTO_S, NAVE_DESTRUIDA_S} Sonido;
// FIN AUDIO

// ÚTILES
int pause = 0;
int teclas[256];
int teclasEspeciales[256];
const int INCREMENTO_Y_PORNIVEL = 10;
const int INCREMENTO_X_EN_NIVEL = 3;
const int DISTANCIAINFERIOR_EJEY = 20;
const int TIEMPO_ENTRE_NIVEL = 100;
const int TIEMPO_DENTRO_NIVEL = 5, LAPSO_REDUCIR_VELOCIDAD_NIVEL = 1000;
const int MINIMO_VELOCIDAD_NIVEL = 100;
int velocidadNivel = 550;
int distanciaSuperior_ejeY = 10;
int textoPosY;
// FIN UTILES

// FUNCIONES
void aumentarVelocidad(int value);
void inicializarMarcianos();
void dibujarMarcianos();
void desplazarMarcianos(int value);
void matarMarcianos(int value);
void inicializarDisparosMarcianos();
void desplazarDisparosMarcianos();
void disparoMarcianoColumna(int col);
void disparoMarciano(int value);
void colisionDisparoMarciano(int a);
void colisionesDisparoNave();
void mostrarOvni(int value);
void dibujarOvni();
void desplazarOvni();
int generarNumeroAleatorio(int min, int max);
void idle(void);
void init(void);
void display(void);
void draw_scene(void);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void recalcularSentido();
void special(int key, int x, int y);
void specialUp(int key, int x, int y);
void operacionesTeclado();
void inicializarCasas();
void disparoObservador();
void mostrarTexto(float x, float y, void*font, char *s);
void gameOver();
void inicializarAudio();
void liberarRecursosAudio();
void reproducirAudio(Sonido);

void init(void)
{
	paso0 = 3;
	alfa = 0;
	eye_X = MAX_EJE_X / 2;
	printf("********************%i*****************", eye_X);
	eye_Y = 0;
	eye_Z = MAX_EJE_Z;

	for (int i = 0; i<256; i++)
        teclas[i] = 0;
    for (int i = 0; i<246; i++)
        teclasEspeciales[i] = 0;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 12; j++){
            bloques[i][j].toques = 3;
        }
    }
    jugador1.nivel=1;
    jugador1.puntuacion=0;
    jugador1.vidas=2;

    inicializarDisparosMarcianos();
    inicializarMarcianos();
    inicializarCasas();
    //inicializarAudio();
    textoPosY = MAX_EJE_Y/2;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	glutTimerFunc(TIMER_MARCIANO, desplazarMarcianos, 0);
    glutTimerFunc(generarNumeroAleatorio(MIN_MS_OVNI, MAX_MS_OVNI), mostrarOvni, 0);
    glutTimerFunc(LAPSO_REDUCIR_VELOCIDAD_NIVEL, aumentarVelocidad, 0);
    glutTimerFunc(generarNumeroAleatorio(MIN_FEC_DISPARO, MAX_FEC_DISPARO), disparoMarciano, 0);

	glClearColor((GLclampf)0.0,(GLclampf)0.0,(GLclampf)0.0,(GLclampf)1);

	glutFullScreen();
}

int main(int argc, char** argv)
{
	ovrs_init();


	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	ovrs_initDisplay(POS_RIGHT, draw_scene);

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

void aumentarVelocidad(int nivel){
    if(pause==0){
        velocidadNivel = velocidadNivel <= MINIMO_VELOCIDAD_NIVEL ? MINIMO_VELOCIDAD_NIVEL : velocidadNivel -  TIEMPO_DENTRO_NIVEL;
    }
    glutTimerFunc(LAPSO_REDUCIR_VELOCIDAD_NIVEL, aumentarVelocidad , 0);
}


void inicializarMarcianos(){
    int auxY, auxX;
    for(int i=0; i<NUM_FILAS; i++){
        auxY = MAX_EJE_Y - (SEPARA_MARCIANO_Y * (i+1)) - distanciaSuperior_ejeY;
        auxX = MAX_EJE_X - ANCHURA_MARCIANO;
        for (int j=0; j<NUM_MARCIANOS_FILA; j++){
            marciano[i][j].marcianoPosX = auxX;
            marciano[i][j].marcianoPosY = auxY;
            marciano[i][j].muerto = 0;
            marciano[i][j].tipo = i;
            auxX -= (SEPARA_MARCIANO_X + ANCHURA_MARCIANO);
        }
    }
}

void dibujarMarcianos(){
    for (int i=0; i<NUM_FILAS; i++){
        for(int j=0; j <NUM_MARCIANOS_FILA; j++){
            if (marciano[i][j].muerto == 0){
                glPushMatrix();
                    if (i==0) glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
                    else if (i==1) glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rose);
                    else if (i==2) glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
                    else if (i==3) glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
                    else glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, azulito);
                    glTranslatef(marciano[i][j].marcianoPosX, marciano[i][j].marcianoPosY,  ALTURA_MARCIANO);
                    glutSolidCube(ANCHURA_MARCIANO);
                glPopMatrix();
            }
        }
    }
}

int colisionFinalCasa(int n, int m){
    int colision=0;
    for (int i=0; i<3; i++){
        if (bloques[i][0].casaPosY-(ANCHO_CASA/2)<=marciano[n][m].marcianoPosY && marciano[n][m].marcianoPosY<=bloques[i][0].casaPosY+(ANCHO_CASA/2)){
            for(int j=0; j<12; j++){
                if (bloques[i][j].toques!=0){
                    if (bloques[i][j].casaPosX-(ANCHO_CASA/2)<=marciano[n][m].marcianoPosX-ALTURA_MARCIANO && marciano[n][m].marcianoPosX-ALTURA_MARCIANO<=bloques[i][j].casaPosX+(ANCHO_CASA/2)){
                            colision=1;
                    }
                }
            }
        }
    }
    return colision;
}

int colisionFinalNave(int i, int j){
    if (marciano[i][j].marcianoPosY-ALTURA_MARCIANO<=eye_Y+ANCHO_NAVE){
        return 1;
    }
    else return 0;
}

void comprobarMarcianosFinal(){
    int llegoFinal=0;
    if(pause==0){
        for (int i=0; i<NUM_FILAS; i++){
            for (int j=0; j<NUM_MARCIANOS_FILA; j++){
                if (marciano[i][j].muerto==0){
                    if (colisionFinalCasa(i,j)==1||colisionFinalNave(i,j)==1){
                        llegoFinal=1;
                    }
                }
            }
        }
    }
    if (llegoFinal==1){
        gameOver();
    } else {
        glutTimerFunc(velocidadNivel, desplazarMarcianos, 0);
    }
}

void recalcularSentido(){
    if (sentido == 0){
        sentido = marciano[1][10].marcianoPosX <= ANCHURA_MARCIANO ? 1 : sentido;
    } else {
        sentido = marciano[1][0].marcianoPosX >= MAX_EJE_X - ANCHURA_MARCIANO ? 0 : sentido;
    }
}

void desplazarMarcianos(int value)
{
    if(pause==0){
        int sentidoAnterior = sentido;
        recalcularSentido();
        for (int i=0; i < NUM_FILAS; i++)
        {
            for (int j=0; j< NUM_MARCIANOS_FILA; j++)
            {
                if (sentido == 0)
                {
                    marciano[i][j].marcianoPosX -= DESPLAZ_MARCIANO_X;
                }
                else
                {
                    marciano[i][j].marcianoPosX += DESPLAZ_MARCIANO_X;
                }
                if (sentido != sentidoAnterior) marciano[i][j].marcianoPosY -= DESPLAZ_MARCIANO_Y;
            }
        }
    }
    comprobarMarcianosFinal();
}

void inicializarDisparosMarcianos(){
    for(int i=0; i<NUM_DISPAROS; i++){
        disparos[i].posX = -1;
        disparos[i].posY = -1;
    }
}

void desplazarDisparosMarcianos(){
    for (int i=0; i < NUM_DISPAROS; i++){
        colisionDisparoMarciano(i);
        if (disparos[i].posX != -1){
            disparos[i].posY -= DESPLAZ_DISPARO_MAR;
            if (disparos[i].posY <= 0){
                disparos[i].posX = -1;
                disparos[i].posY = -1;
            }
        }
    }
}

void disparoMarcianoColumna(int col){
    int posX=-1, posY=-1;
    for (int i = NUM_FILAS-1; i >= 0; i--){
            if(marciano[i][col].muerto == 0){
                posX = marciano[i][col].marcianoPosX - (ANCHURA_MARCIANO/2);
                posY = marciano[i][col].marcianoPosY - ANCHURA_MARCIANO;
            break;
            }
    }
        int i;
        for (i = 0; i < NUM_DISPAROS && disparos[i].posX != -1; i++);
        if (i < NUM_DISPAROS){
            disparos[i].posX = posX;
            disparos[i].posY = posY;
            reproducirAudio(DISPARO_MARCIANO_S);
        }

}
int columnaConMarcianos(int col){
    int hayMarcianos = 0;
    for (int i = 0; i < NUM_FILAS; i++){
        if (marciano[i][col].muerto == 0)
        {
            hayMarcianos = 1;
            break;
        }
    }
    return hayMarcianos;
}

void disparoMarciano(int value){
   if (pause == 0){
        int columna = 0;
        int intentos = 30;
        do {
            columna = generarNumeroAleatorio(0, 11);
            intentos--;
        } while (columnaConMarcianos(columna) == 0 && intentos >= 0);
        disparoMarcianoColumna(columna);
    }
    glutTimerFunc(generarNumeroAleatorio(MIN_FEC_DISPARO, MAX_FEC_DISPARO), disparoMarciano, 0);
}

void dibujarDisparoMarciano(){
    for(int i = 0; i < NUM_DISPAROS; i++){
        if (disparos[i].posX != -1){
            glPushMatrix();
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
                glTranslatef(disparos[i].posX, disparos[i].posY, ALTURA_MARCIANO - (ANCHURA_MARCIANO/4));
                glutSolidCube(ANCHURA_DISPARO_MAR);
            glPopMatrix();
        }
    }
}

void mostrarOvni(int value){
    if(pause == 0){
        ovniVisible = 1;
        ovniPosX = MAX_EJE_X - ANCHURA_OVNI;
        ovniPosY = MAX_EJE_Y - ALTURA_OVNI;
    }
    glutTimerFunc(generarNumeroAleatorio(MIN_MS_OVNI, MAX_MS_OVNI), mostrarOvni, 0);
}

void desplazarOvni(){
    if (ovniVisible == 1){
        ovniPosX = ovniPosX <= 0 ? -1 : ovniPosX - DESPLAZ_OVNI;
        ovniVisible = ovniPosX > 0 ? 1 : 0;
    }
}

void dibujarOvni(){
    if (ovniVisible == 1) {
        glPushMatrix();
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
            glTranslatef(ovniPosX, ovniPosY, ALTURA_MARCIANO);
            glRotatef(90, 0, 1, 0);
            glutSolidCylinder(ANCHURA_OVNI, PROFUNDIDAD_OVNI, 20, 2);
        glPopMatrix();
    }
}

int generarNumeroAleatorio(int min, int max){
    srand(time(NULL));
    return rand()%max + min;
}


void desplazarDisparo(int value){
    if (disparoPosY != -1){
        colisionesDisparoNave();
        glutTimerFunc(1, desplazarDisparo, 0);
    }
    if (disparoPosY >= MAX_EJE_Y){
        disparoPosY = -1;
    }

}

void restablecerNaveGolpeada(){
    for (int i=0; i<NUM_DISPAROS; i++){
        disparos[i].posX=-1;
        disparos[i].posY=-1;
        reproducirAudio(NAVE_DESTRUIDA_S);
    }
    Sleep(2000);
    jugador1.vidas--;
}

void colisionNave(int a){
    if (eye_Y+(ANCHO_NAVE/2) <= disparos[a].posY && disparos[a].posY<=eye_Y + ANCHO_NAVE){
        if (eye_X - ANCHO_NAVE <= disparos[a].posX && disparos[a].posX <= eye_X + ANCHO_NAVE){
            if(jugador1.vidas >0 ){
                restablecerNaveGolpeada();
            }
            else{
                gameOver();
            }
        }
    }
}

void colisionCasaMarciano(int a){
    for (int i=0; i<3; i++){
        if (bloques[i][0].casaPosY-(ANCHO_CASA/2)<=disparos[a].posY && disparos[a].posY<=bloques[i][0].casaPosY+(ANCHO_CASA/2)){
            for(int j=0; j<12; j++){
                if (bloques[i][j].casaPosX-(ANCHO_CASA/2)<=disparos[a].posX && disparos[a].posX<=bloques[i][j].casaPosX+(ANCHO_CASA/2)){
                    if (bloques[i][j].toques!=0){
                        disparos[a].posX=-1;
                        disparos[a].posY=-1;
                        bloques[i][j].toques--;
                    }
                }
            }
        }
    }
}

void colisionDisparoMarciano(int a){
    colisionCasaMarciano(a);
    colisionNave(a);
}

void colisionMarciano(){
    for (int i=0; i< NUM_FILAS; i++){
        if ((marciano[i][0].marcianoPosY - ALTURA_MARCIANO<=disparoPosY && disparoPosY<=marciano[i][0].marcianoPosY)
            ||(marciano[i][0].marcianoPosY - ALTURA_MARCIANO<=disparoPosY+ANCHO_DISPARO && disparoPosY+ANCHO_DISPARO<=marciano[i][0].marcianoPosY)
            ){
            for (int j=0; j<NUM_MARCIANOS_FILA; j++){
                if ((marciano[i][j].marcianoPosX - (ANCHURA_MARCIANO/2)<=disparoPosX && disparoPosX<=marciano[i][j].marcianoPosX + (ANCHURA_MARCIANO/2))
                    ||(marciano[i][j].marcianoPosX - (ANCHURA_MARCIANO/2)<=disparoPosX+ANCHO_DISPARO && disparoPosX+ANCHO_DISPARO<=marciano[i][j].marcianoPosX + (ANCHURA_MARCIANO/2))
                    ){
                    if(marciano[i][j].muerto!=1){
                        disparoPosY=-1;
                        marciano[i][j].muerto=1;
                        jugador1.puntuacion+=(250-50*(i+1));
                        reproducirAudio(MUERTE_MARCIANO_S);
                    }
                }
            }
        }
    }
}

void colisionCasaNave(){
    for (int i=0; i<3; i++){
        if (bloques[i][0].casaPosY-(ANCHO_CASA/2)<=disparoPosY && disparoPosY<=bloques[i][0].casaPosY+(ANCHO_CASA/2)){
            for(int j=0; j<12; j++){
                if (bloques[i][j].casaPosX-(ANCHO_CASA/2)<=disparoPosX && disparoPosX<=bloques[i][j].casaPosX+(ANCHO_CASA/2)){
                    if (bloques[i][j].toques!=0){
                        disparoPosY=-1;
                        bloques[i][j].toques--;
                    }
                }
            }
        }
    }
}

void colisionOvni(){
    if ((ovniPosY - (ALTURA_OVNI/2)<=disparoPosY && disparoPosY<=ovniPosY + (ANCHURA_OVNI/2))
            ||(ovniPosY - (ALTURA_OVNI/2)<=disparoPosY+ANCHO_DISPARO && disparoPosY<=ovniPosY + (ANCHURA_OVNI/2))
        ){
        if ((ovniPosX <=disparoPosX && disparoPosX<=ovniPosX + (PROFUNDIDAD_OVNI))
            ||(ovniPosX <=disparoPosX+ANCHO_DISPARO && disparoPosX<=ovniPosX+ (PROFUNDIDAD_OVNI))
        ){
            if(ovniVisible!=0){
                disparoPosY=-1;
                ovniVisible=0;
                jugador1.puntuacion+=500;
                reproducirAudio(MUERTE_MARCIANO_S);
            }
        }
    }
}
void aumentarNivel(){
    velocidadNivel = TIMER_MARCIANO - (TIEMPO_DENTRO_NIVEL*jugador1.nivel) < MINIMO_VELOCIDAD_NIVEL ? MINIMO_VELOCIDAD_NIVEL : TIMER_MARCIANO - (TIEMPO_DENTRO_NIVEL*jugador1.nivel);
    distanciaSuperior_ejeY += INCREMENTO_Y_PORNIVEL;
    jugador1.nivel++;
    jugador1.puntuacion += 1000 *jugador1.nivel;
    jugador1.vidas++;
    inicializarMarcianos();
    Sleep(2000);
}

void comprobarMuertos(){
    int todosMuertos=1;
    for (int i=0; i<NUM_FILAS;i++){
        for (int j=0; j<NUM_MARCIANOS_FILA;j++){
           if(marciano[i][j].muerto==0){
                todosMuertos=0;
           }
        }
    }
    if (todosMuertos==1){
        aumentarNivel();
    }
}

void colisionesDisparoNave(){
    colisionCasaNave();
    colisionMarciano();
    colisionOvni();
    if (disparoPosY!=-1){
        disparoPosY += DESPLAZ_DISPARO;
    }
    else{
        comprobarMuertos();
    }
}

void mostrarTexto(){
    glPushMatrix();
        glColor3ub (192, 192, 192);
        char *c="Score: ";
        char *v="Vidas: ";
        char *tip = "Desplazar: <- -> | Atacar: <Espacio> | Desactivar sonido: <M> | Pausar juego: <P>";
        char *n="Nivel: ";
        char *i;
        char *k;
        char *h;
        char puntuacion[20];
        char vidas[10];
        char nivel[10];
        sprintf(puntuacion,"%d",jugador1.puntuacion);
        sprintf(vidas,"%d",jugador1.vidas+1);
        sprintf(nivel,"%d",jugador1.nivel);
        glRasterPos3i(0, textoPosY, 20);
        for(i=c; *i!='\0';i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*i);
        }
        glRasterPos2i(110, textoPosY);
        for(i=puntuacion; *i!='\0';i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*i);
        }
        glRasterPos2i(1230, textoPosY);
        for(k=v; *k!='\0';k++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*k);
        }
          glRasterPos2i(1300, textoPosY);
        for(k=vidas; *k!='\0';k++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*k);
        }
        glRasterPos2i(630, textoPosY);
        for(h=n; *h!='\0';h++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*h);
        }
        glRasterPos2i(700, textoPosY);
        for(h=nivel; *h!='\0';h++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*h);
        }
        glRasterPos2i(0, 0);
        for (k=tip; *k != '\0'; k++){
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *k);
        }
    glPopMatrix();
}

void inicializarAudio(){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, AUDIO_S16, 2, 512);
    soundDisparoMarciano = Mix_LoadWAV("sonidos/SpaceGunInvader.wav");
    if (!soundDisparoMarciano)
    {
        printf("\n\nError Mix_LoadWAV: %s\n", Mix_GetError());
        exit(0);
    }
    soundDisparoNave = Mix_LoadWAV("sonidos/SpaceGunDefender.wav");
    soundMuerteMarciano = Mix_LoadWAV("sonidos/InvaderEliminated.wav");
    soundOvniVisto = Mix_LoadWAV("sonidos/OvniSighted.wav");
    soundNaveDestruida = Mix_LoadWAV("sonidos/PlayerDestroyed.wav");
    sonidoHabilitado = 1;
}

void liberarRecursosAudio(){
    Mix_FreeChunk(soundDisparoMarciano);
    Mix_FreeChunk(soundDisparoNave);
    Mix_FreeChunk(soundMuerteMarciano);
    Mix_FreeChunk(soundOvniVisto);
    Mix_FreeChunk(soundNaveDestruida);
    SDL_Quit();
}

void reproducirAudio(Sonido tipo){
    if (sonidoHabilitado == 1){
        Mix_Chunk *aux;
        switch(tipo){
            case DISPARO_MARCIANO_S:
                aux = soundDisparoMarciano;
                break;
            case DISPARO_NAVE_S:
                aux = soundDisparoNave;
                break;
            case MUERTE_MARCIANO_S:
                aux = soundMuerteMarciano;
                break;
            case OVNI_VISTO_S:
                aux = soundOvniVisto;
                break;
            case NAVE_DESTRUIDA_S:
                aux = soundNaveDestruida;
                break;
        }
        Mix_PlayChannel(-1, aux, 0);
    }
}

void idle(void)
{
    desplazarDisparosMarcianos();
    desplazarOvni();
	glutPostRedisplay();
}


/*Funcion que se ejecuta cada ciclo para mostrar la escena*/
void display(void)
{
    operacionesTeclado();
	struct observer ob = ovrs_displayObserver(eye_X, eye_Y, eye_Z);
	alfa = ob.alfa;
}

/*Funcion que define la escena*/
void draw_scene(void){

	int i;
	float col[] = {0, 0, 0, 1};
	float lpos[][4] =
	{
		{-8, 2, 10, 1},
		{0, 15, 0, 1}
	};
	float lcol[][4] =
	{
		{(float)0.8, (float)0.8, (float)0.8, 1},
		{(float)0.4, (float)0.3, (float)0.3, 1}
	};

	for(i=0; i<2; i++)
	{
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lpos[i]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lcol[i]);
	}

    // Campo
	glPushMatrix();
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(MAX_EJE_X, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3f(MAX_EJE_X, MAX_EJE_Y, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, MAX_EJE_Y, 0);
        glEnd();
	glPopMatrix();

    // Disparos
    disparoObservador();
    dibujarDisparoMarciano();

    dibujarMarcianos();
    dibujarOvni();

    // Casas
	inicializarCasas();

	glutSwapBuffers();

}

//Función Casa
    void inicializarCasas(){
        int casaPosX = DISTANCIA_CASA_X;
        int casaPosY = DISTANCIA_OBSERVADORCASA;
        for (int k = 0; k < 4; k++){
            for (int i=0; i<3; i++){
                int casaPosXAux = casaPosX;
                for(int j=0; j<3; j++){
                    if (bloques[i][j+(3*k)].toques!=0) {
                        bloques[i][j+(3*k)].casaPosX=casaPosX;
                        bloques[i][j+(3*k)].casaPosY=casaPosY;

                        float color[4];
                        float colorValue;
                        switch (bloques[i][j+(3*k)].toques){
                            case 3:
                                colorValue = 1.0;
                                break;
                            case 2:
                                colorValue = 0.8;
                                break;
                            case 1:
                                colorValue = 0.5;
                                break;
                        }
                        for(int i=0; i<3; i++) color[i] = colorValue;
                        color[3] = 1.0;

                        glPushMatrix();
                            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
                            glTranslatef(casaPosX, casaPosY, ALTO_CASA);
                            glutSolidCube(ANCHO_CASA);
                        glPopMatrix();
                        glPushMatrix();
                            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
                            glTranslatef(casaPosX, casaPosY, ALTO_CASA*2);
                            glutSolidCube(ANCHO_CASA);
                        glPopMatrix();
                        glPushMatrix();
                            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
                            glTranslatef(casaPosX, casaPosY, ALTO_CASA*3);
                            glutSolidCube(ANCHO_CASA);
                        glPopMatrix();
                    }
                    casaPosX += ANCHO_CASA;
                }
                casaPosY += ANCHO_CASA;
                casaPosX = casaPosXAux;
            }
            casaPosX += DISTANCIA_ENTRECASAS;
            casaPosY = DISTANCIA_OBSERVADORCASA;
        }

}

//Función Disparo Observador

void disparoObservador(){
    if (disparoPosY != -1){
        glPushMatrix();
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
            glTranslatef(disparoPosX, disparoPosY, DISPARO_POSZ);
            glRotatef(90, 1, 0, 0);
            glutSolidCylinder(ANCHO_DISPARO, PROFUNDIDAD_DISPARO, 20, 2);
        glPopMatrix();
    }
}

//Operaciones teclado

void operacionesTeclado(){
    if (teclas[27] == 1){
		gameOver();
    }
     if (teclas['p'] || teclas['P']){
        if (pause == 0){
            pause = 1;
        } else {
            pause = 0;
        }
    }

    if(pause==0){
        if (teclas[32] == 1){
            if (disparoPosY == -1){
                disparoPosX = eye_X;
                disparoPosY = eye_Y;
                glutTimerFunc(1, desplazarDisparo, 0);
                reproducirAudio(DISPARO_NAVE_S);
            }
        }

        if (teclasEspeciales[GLUT_KEY_LEFT] == 1){
            eye_X =  eye_X == 0 ? eye_X : eye_X - paso0 ;
        }
        if (teclasEspeciales[GLUT_KEY_RIGHT] == 1){
            eye_X =  eye_X == MAX_EJE_X ? eye_X : eye_X + paso0;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
    teclas[key] = 1;
}

void keyboardUp(unsigned char key, int x, int y){
    teclas[key] = 0;
}

void special(int key, int x, int y) {
    teclasEspeciales[key] = 1;
}

void specialUp(int key, int x, int y) {
    teclasEspeciales[key] = 0;
}

void gameOver(){
    printf("Puntuacion: %i", jugador1.puntuacion);
    liberarRecursosAudio();
    ovrs_close();
    exit(0);
}
