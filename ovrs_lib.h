#define OVR_OS_WIN32

#define POS_TOP 0
#define POS_BOTTOM 1
#define POS_LEFT 2
#define POS_RIGHT 3
#define POS_CENTER 4

#define grToRad(grados) ((grados) * 0.0174532925)
#define radToGr(radianes) ((radianes) / 0.0174532925)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <LibOVR/src/OVR_CAPI.h>
#include <LibOVR/src/OVR_CAPI_GL.h>

/*Estructura de devuelve la funcion ovrs_displayObserver con los datos locales del observador*/
struct observer{
	double x;
	double y;
	double z;
	double alfa;
	double beta;
	double gamma;
};

/*Es el descriptor de nuestras gafas*/
static ovrHmd hmd;

/*Es la primera funcion que hay que llamar. Antes de iniciar el contexto opengl*/
void ovrs_init(void);
/* Funcion que define znear en la escena*/
void ovrs_setZnear(float _znear);
/* Funcion que define zfar en la escena*/
void ovrs_setZfar(float _zfar);
/* Funcion que inicializa el Display */
/*parametro 1 : Posicion del monitor las gafas en el sistema */
/*parametro 2 : funcion que muestra la escena opengl */
void ovrs_initDisplay(int ExtendDesktopPosition,void (*callbackfunc)());
/* Funcion que finaliza la libreria ovrs. Hay que llamarla antes de salir del programa. */
void ovrs_close(void);
/* Funcion que muestra y oculta la advertencia de seguridad */
void ovrs_showWarning(int visible);
/*Funcion que siempre se tiene que llamar en Display */
/*Define la posicion del observador en el escena (eye_X, eye_Y, eye_Z)*/
/*Devuelve una estructura que contine la posicion de la gafa x,y,z */
/*y su orientarcion alfa,beta,gamma respecto a la posicion del observador */
/*con orientacion 0,0,0 */
struct observer ovrs_displayObserver(double eye_X,double eye_Y,double eye_Z);
