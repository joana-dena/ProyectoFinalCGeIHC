/*---------------------------------------------------------*/
/* ----------------   Proyecto Final  --------------------------*/
/*-----------------    2026-2   ---------------------------*/
/*------------- Computación Gráfica e ---------------*/
/*------------- Interacción Humano-Computadora ---------------*/

#include <AL/al.h>
#include <AL/alc.h>


#define _CRT_SECURE_NO_WARNINGS /////////////////////////////////////////////////////////////Proyecto
#include <Windows.h>
#include <ctime> ////////////////////////////////////////////////////////////////////////////Proyecto
#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>

// AUDIO
SDL_AudioSpec wavSpec;
Uint8* wavBuffer = nullptr;
Uint32 wavLength = 0;

SDL_AudioStream* stream = nullptr;

ALCdevice* device = nullptr;
ALCcontext* context = nullptr;

//AUDIO 

bool initOpenAL()

{
	device = alcOpenDevice(NULL);

	if (!device)
	{
		printf("No se pudo abrir dispositivo OpenAL\n");
		return false;
	}

	context = alcCreateContext(device, NULL);

	if (!alcMakeContextCurrent(context))
	{
		printf("No se pudo crear contexto OpenAL\n");
		return false;
	}

	printf("OpenAL inicializado correctamente\n");

	return true;
}
void closeOpenAL()
{
	alcMakeContextCurrent(NULL);

	if (context)
		alcDestroyContext(context);

	if (device)
		alcCloseDevice(device);
}


std::vector<unsigned int> secuenciaTexturas;
int frameActual = 0;
float tiempoAcumulado = 0.0f;
float fpsVideo = 12.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
float alturaActual = 70.0f;
Camera camera(glm::vec3(0.0f, alturaActual, 1000.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool, bool);

float myTime = 0.0f;
//Variables nuevas para mi implementacion
static float timer = 0.0f;
static int pasoActual = 0;

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(-1.0f, 0.0f, 0.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

//// LightColor
glm::vec3 LightColorNueva(1.0f, 1.0f, 1.0f);


// posiciones
int estadoAutoAmarillo = 0; //1Avanza; 2Reversa;
float	movAuto_x = 0.0f;
float	movAuto_y = 0.0f,
movAuto_z = 0.0f;
float orienta = 0.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;

// --------- PERRITO -----------
float movPerritoX = 0.0f,
movPerritoZ = 0.0f,
tiempoPerrito = 0.0f,
giroCabezaPerrito = 0.0f,
giroCuelloPerrito = 0.0f,
giroColaPerrito = 0.0f,
giroPataDerFPerrito = 0.0f,
giroPataIzqFPerrito = 0.0f,
giroPataDerTPerrito = 0.0f,
giroPataIzqTPerrito = 0.0f,
orientaPerrito = 90.0f;
int edoPerrito = 1;
bool animacionPerrito = false;

//Movimiento Bote Basura
float anguloBote1 = 0.0f;
float anguloBote2 = 0.0f;
float velocidadPendulo = 0.05f;
float tiempoPendulo = 0.0f;
bool animacionBotes = false;
float timerBotes = 0.0f;
bool detectado = false;

float factorRecogido = 1.0f; // 1.0 = Cerrada, 0.1 = Recogida

//Robot
float movRobotX = 0.0f,
movRobotZ = 0.0f,
tiempoRobot = 0.0f,
orientaRobot = 0.0f;
int edoRobot = 1;
bool animacionRobot = false;

//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f,
giroCabeza = 0.0f,
giroBrazoDer = 0.0f;
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotRodIzqInc = 0.0f,
giroMonitoInc = 0.0f,
giroCabezaInc = 0.0f,
giroBrazoDerInc = 0.0f;


//dron

float rotHelices = 0.0f;
float dronX = 0.0f;
float dronY = 80.0f; // ALTURA DEL DRON
float dronZ = 0.0f;
float velocidadDron = 0.5f;


struct Frame {
	float x, z;
	float yaw;
};

float tFord = 0.0f;
int indexFord = 0;
bool playAnimacion = false;
float currX = 954.365f, currZ = 825.198f, currYaw = 180.0f;
float rotLlantas = 0.0f;
Frame pasosFord[] = {
	{ 954.365f,  825.198f,  180.0f }, // 0: Inicio
	{ -199.134f, 826.909f,  180.0f }, // 1: Llega al punto de giro (Recto)
	{ -199.134f, 826.909f,  270.0f }, // 2: Mismo lugar, pero ya volteó a la izquierda
	{ -196.733f, 1055.2f,   270.0f }, // 3: Avanza ya orientado hacia el siguiente punto
	{ -196.733f, 1055.2f,   360.0f }, // 4: Gira en su lugar otra vez
	{ 885.784f,  1095.17f,  360.0f }  // 5: Final
};



#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float giroCabeza;
	float giroBrazoDer;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].giroCabeza = giroCabeza;
	KeyFrame[FrameIndex].giroBrazoDer = giroBrazoDer;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	giroCabeza = KeyFrame[0].giroCabeza;
	giroBrazoDer = KeyFrame[0].giroBrazoDer;
}


void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;
	giroBrazoDerInc = (KeyFrame[playIndex + 1].giroBrazoDer - KeyFrame[playIndex].giroCabeza) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	if (isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void)
{

	timer += 0.1f;

	if (timer > 10.0f) {
		pasoActual++;
		timer = 0.0f;
	}


	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;
			giroCabeza += giroCabezaInc;
			giroBrazoDer += giroBrazoDerInc;

			i_curr_steps++;
		}
	}

	if (playAnimacion) {
		tFord += 0.002f;

		if (tFord >= 1.0f) {
			tFord = 0.0f;
			indexFord++;

			if (indexFord >= 5) {
				indexFord = 0;
			}
		}

		int sig = indexFord + 1;
		sig %= 6;

		currX = pasosFord[indexFord].x + (pasosFord[sig].x - pasosFord[indexFord].x) * tFord;
		currZ = pasosFord[indexFord].z + (pasosFord[sig].z - pasosFord[indexFord].z) * tFord;
		currYaw = pasosFord[indexFord].yaw + (pasosFord[sig].yaw - pasosFord[indexFord].yaw) * tFord;

		if (pasosFord[indexFord].x != pasosFord[sig].x || pasosFord[indexFord].z != pasosFord[sig].z) {
			rotLlantas += 5.0f;
		}
	}



	if (animacion)
	{
		switch (estadoAutoAmarillo)
		{
		case 1: // REVERSA 200 
			movAuto_x -= 4.0f; // Retrocede
			orienta = 270.0f;
			if (movAuto_x <= -200.0f) {
				estadoAutoAmarillo = 2;
			}
			break;

		case 2: // SUBIR 200.5 
			movAuto_y += 4.0f;
			orienta = 0.0f;
			if (movAuto_y >= 100.5f) {
				estadoAutoAmarillo = 3;
			}
			break;

		case 3: // AVANZAR 310.5 
			movAuto_x += 4.0f;
			orienta = 90.0f;

			if (movAuto_x >= 110.5f) {
				estadoAutoAmarillo = 4;
			}
			break;

		case 4: // BAJAR 200.5 
			movAuto_y -= 3.0f;
			orienta = 180.0f;
			if (movAuto_y <= 0.0f) {
				estadoAutoAmarillo = 5;
			}
			break;

		case 5: // AVANZAR 150 
			movAuto_x += 3.0f;
			orienta = 90.0f;

			if (movAuto_x >= 260.5f) {
				animacion = false;
				estadoAutoAmarillo = 0;
			}
			break;
		}
	}

	if (animacionPerrito)
	{
		tiempoPerrito += 0.10f;
		giroCabezaPerrito = sin(tiempoPerrito) * 5.0f;
		giroCuelloPerrito = sin(tiempoPerrito) * 4.0f;
		giroColaPerrito = sin(tiempoPerrito * 2.0f) * 18.0f;

		giroPataDerFPerrito = sin(tiempoPerrito) * 25.0f;
		giroPataIzqFPerrito = -sin(tiempoPerrito) * 25.0f;

		giroPataDerTPerrito = -sin(tiempoPerrito) * 25.0f;
		giroPataIzqTPerrito = sin(tiempoPerrito) * 25.0f;


		switch (edoPerrito)
		{
		case 1:
			orientaPerrito = 0.0f;
			movPerritoX += 0.15f;

			if (movPerritoX >= 40.0f)
				edoPerrito = 2;
			break;

		case 2:
			orientaPerrito = -90.0f;
			movPerritoZ += 0.15f;

			if (movPerritoZ >= 30.0f)
				edoPerrito = 3;
			break;

		case 3:
			orientaPerrito = 180.0f;
			movPerritoX -= 0.15f;

			if (movPerritoX <= 0.0f)
				edoPerrito = 4;
			break;

		case 4:
			orientaPerrito = 90.0f;
			movPerritoZ -= 0.15f;

			if (movPerritoZ <= 0.0f)
				edoPerrito = 1;
			break;
		}
	}

//Animación robot
	if (animacionRobot)
	{
		switch (edoRobot)
		{
		case 1: // avanza derecha
			orientaRobot = 90.0f;
			movRobotX += 0.20f;
			if (movRobotX >= 40.0f)
				edoRobot = 2;
			break;

		case 2: // sube
			orientaRobot = 0.0f;
			movRobotZ += 0.20f;
			if (movRobotZ >= 30.0f)
				edoRobot = 3;
			break;

		case 3: // diagonal izquierda-arriba
			orientaRobot = -45.0f;
			movRobotX -= 0.15f;
			movRobotZ += 0.15f;
			if (movRobotX <= 15.0f && movRobotZ >= 55.0f)
				edoRobot = 4;
			break;

		case 4: // baja en diagonal derecha
			orientaRobot = 135.0f;
			movRobotX += 0.15f;
			movRobotZ -= 0.15f;
			if (movRobotX >= 45.0f && movRobotZ <= 25.0f)
				edoRobot = 5;
			break;

		case 5: // avanza izquierda
			orientaRobot = -90.0f;
			movRobotX -= 0.20f;
			if (movRobotX <= -20.0f)
				edoRobot = 6;
			break;

		case 6: // baja
			orientaRobot = 180.0f;
			movRobotZ -= 0.20f;
			if (movRobotZ <= -20.0f)
				edoRobot = 7;
			break;

		case 7: // diagonal al centro
			orientaRobot = 45.0f;
			movRobotX += 0.15f;
			movRobotZ += 0.15f;
			if (movRobotX >= 0.0f && movRobotZ >= 0.0f)
				edoRobot = 8;
			break;

		case 8: // reset
			movRobotX = 0.0f;
			movRobotZ = 0.0f;
			orientaRobot = 90.0f;
			edoRobot = 1;
			break;
		}
	}

//Animación botes
	glm::vec3 posBotes = glm::vec3(353.892f, alturaActual, 722.162f);
	float distancia = glm::distance(camera.Position, posBotes);
	if (distancia <= 50.0f) {
		if (!animacionBotes) {
			animacionBotes = true;
			timerBotes = 0.0f;
		}
	}
	if (animacionBotes) {
		timerBotes += 0.01f;
		if (timerBotes >= 3.0f) {
			animacionBotes = false;
			tiempoPendulo = 0.0f;
		}
		tiempoPendulo += 0.02f;
		anguloBote1 = sin(tiempoPendulo) * 15.0f;
		anguloBote2 = sin(tiempoPendulo + 3.1415f) * 15.0f;
	}
	else {
		anguloBote1 = 0.0f;
		anguloBote2 = 0.0f;
	}

	glm::vec3 posCortina = glm::vec3(35.0f, alturaActual, 195.0f);
	float dist = glm::distance(camera.Position, posCortina);

	if (dist <= 400.0f) {
		if (factorRecogido > 0.1f) factorRecogido -= 0.01f; // Se abre
	}
	else {
		if (factorRecogido < 1.0f) factorRecogido += 0.01f; // Se cierra
	}

/////////////////////// Dron

rotHelices += 20.0f;

if (rotHelices >= 360.0f)
{
	rotHelices = 0.0f;
}

// Movimiento circular del dron
dronX = cos(glfwGetTime() * 0.3f) * 50.0f;
dronZ = sin(glfwGetTime() * 0.3f) * 50.0f;

// Flotacion
dronY = 80.0f + sin(glfwGetTime() * 2.0f) * 5.0f;


}
void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Laboratorio - CGeIHC 2026-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initOpenAL();
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cout << "Error iniciando SDL Audio\n";
	}
	std::cout << "Buscando audio en: " << SDL_GetBasePath() << std::endl;
	std::cout << "SDL Error: " << SDL_GetError() << std::endl;

	if (SDL_LoadWAV("resources/sounds/unam.wav",
		&wavSpec,
		&wavBuffer,
		&wavLength) == NULL)
	{
		std::cout << "No se pudo cargar sonido\n";
	}

	stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_OUTPUT,
		&wavSpec,
		NULL,
		NULL);

	if (stream == NULL)
	{
		std::cout << "No se pudo abrir stream\n";
	}

	SDL_PutAudioStreamData(stream,
		wavBuffer,
		wavLength);

	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));


	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);



	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);



	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		/*
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
		*/
		"resources/skybox/px.jpg",
		"resources/skybox/nx.jpg",
		"resources/skybox/py.jpg",
		"resources/skybox/ny.jpg",
		"resources/skybox/pz.jpg",
		"resources/skybox/nz.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	Shader lonaShader("shaders/lona.vs", "shaders/lona.fs");

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model FI("resources/objects/FI/FI.obj");
	Model ford("resources/objects/FordCarrera/Ford_Carreras.obj");
	Model ExpoFord("resources/objects/ExpoFord/Expo_Carro_Ford.obj");
	Model StandFI("resources/objects/StandFI/StandFI.obj");
	Model StandBosch("resources/objects/StandBosch/StandBosch.obj");
	Model StandAmazon("resources/objects/StandAmazon/StandAmazon.obj");
	Model StandLiverpool("resources/objects/StandLiverpool/StandLiverpool.obj");
	Model StandPinterest("resources/objects/StandPinterest/StandPinterest.obj");
	Model StandCisco("resources/objects/StandCisco/StandCisco.obj");
	Model StandHuawei("resources/objects/StandHuawei/StandHuawei.obj");
	Model pumagua("resources/objects/pumagua/pumagua.obj");
	Model bici("resources/objects/bici/BiciCom.obj");
	Model cortina("resources/objects/cortina/cortina.obj");
	Model CarroFord("resources/objects/CarroFord/CarroFord.obj");
	Model CarroceriaFord("resources/objects/CarroFord/CarroceriaFord.obj");
	Model LlantaFord("resources/objects/CarroFord/Llanta1.obj");

	Model perritoCuerpo("resources/objects/perrito/cuerpo.obj");
	Model perritoCabeza("resources/objects/perrito/cabeza.obj");
	Model perritoCuello("resources/objects/perrito/cuello.obj");
	Model perritoCola("resources/objects/perrito/cola.obj");
	Model perritoPataDerF("resources/objects/perrito/pataDerF.obj");
	Model perritoPataIzqF("resources/objects/perrito/pataIzqF.obj");
	Model perritoPataDerT("resources/objects/perrito/pataDerT.obj");
	Model perritoPataIzqT("resources/objects/perrito/pataIzqT.obj");

	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto
	Model soporteBoteGris("resources/objects/boteBasura/Gris/SoporteBoteGris.obj");
	Model BoteGris("resources/objects/boteBasura/Gris/BoteGris.obj");
	Model soporteBoteVerde("resources/objects/boteBasura/Verde/SoporteBoteVerde.obj");
	Model BoteVerde("resources/objects/boteBasura/Verde/BoteVerde.obj");

	/////////////////////////////////////////////////////////////////////////////////////////////////////ROBOT
	Model robotCuerpo("resources/objects/bot/cuerpo.obj");
	Model robotBrazoDer("resources/objects/bot/brazoDer.obj");
	Model robotBrazoIzq("resources/objects/bot/brazoIzq.obj");
	Model robotPiernaDer("resources/objects/bot/piernaDer.obj");
	Model robotPiernaIzq("resources/objects/bot/piernaIzq.obj");


	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

	Model StandPantalla("resources/objects/StandPantalla/StandPantalla2.obj");
	Model Pantalla("resources/objects/Pantalla/Pantalla.obj");
	ModelAnim animacionEstudiante("resources/objects/PersonajeEstudiante/PersonajeEstudiante.dae");
	animacionEstudiante.initShaders(animShader.ID);
	Model Reloj("resources/objects/RelojAnalogico/Reloj.obj");
	Model Horas("resources/objects/RelojAnalogico/Horas1.obj");
	Model Minutos("resources/objects/RelojAnalogico/Minutos.obj");
	Model Segundos("resources/objects/RelojAnalogico/Segundos.obj");

	Model LonaFord("resources/objects/Ford/AnuncioFord.obj");
	Model LonaFi("resources/objects/LonaFi/AnuncioFi.obj");

	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

	ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	animacionPersonaje.initShaders(animShader.ID);

	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto


	Model dronCuerpo("resources/objects/Dron/Cuerpo.obj");
	Model helice1("resources/objects/Dron/helice1.obj");
	Model helice2("resources/objects/Dron/helice2.obj");
	Model helice3("resources/objects/Dron/helice3.obj");
	Model helice4("resources/objects/Dron/helice4.obj");
	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

	

	for (int i = 1; i <= 217; i++) {
		char nombreArchivo[150];

		sprintf(nombreArchivo, "resources/Textures/Pantalla/ezgif-frame-%03d.png", i);


		unsigned int id = TextureFromFile(nombreArchivo, ".");
		if (id != 0) {
			secuenciaTexturas.push_back(id);
		}
		else {

			std::cout << "Error al cargar: " << nombreArchivo << std::endl;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].giroCabeza = 0;
		KeyFrame[i].giroBrazoDer = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// --- Dibujado de la Pantalla ---
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		// --- Reloj ---
		time_t ahora = time(0);
		tm* ltm = localtime(&ahora);

		// Obtenemos los valores actuales
		int s = ltm->tm_sec;
		int m = ltm->tm_min;
		int h = ltm->tm_hour;

		float anguloSeg = -(float)s * 6.0f;
		float anguloMin = -(float)m * 6.0f;
		float anguloHor = -((float)(h % 12) * 30.0f + (float)m * 0.5f);


		glm::mat4 modelReloj = glm::mat4(1.0f);
		modelReloj = glm::translate(modelReloj, glm::vec3(-440.0f, 100.0f, -70.0f));
		modelReloj = glm::rotate(modelReloj, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelReloj = glm::rotate(modelReloj, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelReloj = glm::scale(modelReloj, glm::vec3(1.0f, 1.0f, 1.0f));

		staticShader.setMat4("model", modelReloj);
		staticShader.setBool("usarColorManual", false);
		Reloj.Draw(staticShader);

		glm::mat4 modelSeg = modelReloj;
		staticShader.setBool("usarColorManual", true);
		staticShader.setVec3("colorManual", glm::vec3(0.0f, 0.0f, 0.0f));
		modelSeg = glm::rotate(modelSeg, glm::radians(anguloSeg), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelSeg);
		Segundos.Draw(staticShader);

		glm::mat4 modelMin = modelReloj;
		staticShader.setBool("usarColorManual", true);
		staticShader.setVec3("colorManual", glm::vec3(0.0f, 0.0f, 0.0f));
		modelMin = glm::rotate(modelMin, glm::radians(anguloMin), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelMin);
		Minutos.Draw(staticShader);

		// 4. HORAS
		glm::mat4 modelHor = modelReloj;
		staticShader.setBool("usarColorManual", true);
		staticShader.setVec3("colorManual", glm::vec3(0.0f, 0.0f, 0.0f));
		modelHor = glm::rotate(modelHor, glm::radians(anguloHor), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelHor);
		Horas.Draw(staticShader);


		staticShader.setBool("usarColorManual", false);


		// --- Lógica para el video ---
		float tiempoActual = SDL_GetTicks() / 1000.0f; // Convertimos milisegundos a segundos
		static float tiempoAnterior = 0.0f;
		float deltaTimeVideo = tiempoActual - tiempoAnterior;
		tiempoAnterior = tiempoActual;

		tiempoAcumulado += deltaTimeVideo;
		if (tiempoAcumulado >= (1.0f / fpsVideo)) {
			if (!secuenciaTexturas.empty()) {
				frameActual = (frameActual + 1) % secuenciaTexturas.size();
			}
			tiempoAcumulado = 0.0f;
		}


		lonaShader.use();
		lonaShader.setMat4("projection", projectionOp);
		lonaShader.setMat4("view", viewOp);
		float timeValue = glfwGetTime() * 2.0f;
		lonaShader.setFloat("time", timeValue);

		lonaShader.setInt("tipoViento", 2);
		glm::mat4 modelLona = glm::mat4(1.0f);

		modelLona = glm::translate(modelLona, glm::vec3(-1000.0f, 5.0f, 230.0f));
		modelLona = glm::scale(modelLona, glm::vec3(5.0f, 5.0f, 5.0f));

		lonaShader.setMat4("model", modelLona);
		LonaFord.Draw(lonaShader);


		glm::mat4 modelLona2 = glm::mat4(1.0f);
		lonaShader.setInt("tipoViento", 1);
		modelLona2 = glm::translate(modelLona2, glm::vec3(-700.0f, 5.0f, 230.0f));
		modelLona2 = glm::scale(modelLona2, glm::vec3(5.0f, 5.0f, 5.0f));
		lonaShader.setMat4("model", modelLona2);
		LonaFi.Draw(lonaShader);


		staticShader.use();


		/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

		// input
		// -----
		//my_input(window);
		animate();


		/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto
		glActiveTexture(GL_TEXTURE0);
		if (!secuenciaTexturas.empty()) {
			glBindTexture(GL_TEXTURE_2D, secuenciaTexturas[frameActual]);
		}

		glm::mat4 modelPantallaMat = glm::mat4(1.0f);
		modelPantallaMat = glm::translate(modelPantallaMat, glm::vec3(-29.5f, 82.0f, -534.0f));
		modelPantallaMat = glm::scale(modelPantallaMat, glm::vec3(1.5f, 1.5f, 1.5f));
		staticShader.setMat4("model", modelPantallaMat);
		Pantalla.Draw(staticShader);


		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		/////////////////////////////////////////////////////////////////////////////////////////////////////proyecto

		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.6f, 0.6f, 0.6f)); // Luz suave de relleno
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f)); // Intensidad media
		staticShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));


		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.000009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[2].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(8.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(12.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		myShader.use();
		viewOp = camera.GetViewMatrix();
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -40.0f, 0.0f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);


		glBindVertexArray(0);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		piso.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		FI.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.5f, -470.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.45f));
		staticShader.setMat4("model", modelOp);
		StandFI.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 7.5f, 60.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		StandBosch.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(230.0f, 9.5f, -200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.2f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		StandAmazon.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-613.0f, 0.0f, -530.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.3f));
		staticShader.setMat4("model", modelOp);
		StandLiverpool.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(285.0f, 0.0f, -530.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.3f));
		staticShader.setMat4("model", modelOp);
		StandPinterest.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-377.0f, 0.0f, -530.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.3f));
		staticShader.setMat4("model", modelOp);
		StandCisco.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-315.0f, 0.0f, 60.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		StandHuawei.Draw(staticShader);

		// --- MATRIZ PADRE (CARROCERÍA) ---
		glm::mat4 modelCarro = glm::mat4(1.0f);
		modelCarro = glm::translate(modelCarro, glm::vec3(currX, 0.0f, currZ));
		modelCarro = glm::rotate(modelCarro, glm::radians(currYaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCarro = glm::scale(modelCarro, glm::vec3(6.0f));
		staticShader.setMat4("model", modelCarro);
		CarroceriaFord.Draw(staticShader);
		// --- MATRICES HIJAS (LLANTAS) ---
		auto dibujarLlanta = [&](glm::vec3 offset, float rotY, bool derecha) {
			glm::mat4 m = modelCarro;
			m = glm::translate(m, offset);
			m = glm::scale(m, glm::vec3(0.9375f));
			m = glm::rotate(m, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::rotate(m, glm::radians(rotLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
			staticShader.setMat4("model", m);
			LlantaFord.Draw(staticShader);
		};

		dibujarLlanta(glm::vec3(-7.5f, 2.5f, 13.0f), 0.0f, false);   // Del Izq
		dibujarLlanta(glm::vec3(-7.5f, 2.5f, -12.0f), 0.0f, false);  // Tras Izq
		dibujarLlanta(glm::vec3(7.5f, 2.5f, 13.0f), 180.0f, true);   // Del Der
		dibujarLlanta(glm::vec3(7.5f, 2.5f, -12.0f), 180.0f, true);  // Tras Der


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, 400.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		bici.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, 500.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		bici.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(257.159f, 0.0f, 551.814f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		pumagua.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-700.0f, -15.0f, 670.0f));
		modelOp = glm::scale(modelOp, glm::vec3(18.0f));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		ExpoFord.Draw(staticShader);

		glm::mat4 modelOp = glm::mat4(1.0f);
		modelOp = glm::translate(modelOp, glm::vec3(35.0f, 0.0f, 195.0f));
		float compensacionX = (23.0f) * (1.0f - factorRecogido);
		modelOp = glm::translate(modelOp, glm::vec3(compensacionX, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(46.0f * factorRecogido, 46.0f, 46.0f));
		staticShader.setMat4("model", modelOp);
		cortina.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-330.0f, 0.0f, 380.0f));
		modelOp = glm::scale(modelOp, glm::vec3(19.5f));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		ford.Draw(staticShader);

		////////////////////////////////////////// PERRO ROBOT
		glm::mat4 jerarquiaPerrito;
		jerarquiaPerrito = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f + movPerritoX, 0.0f, 25.0f + movPerritoZ));
		jerarquiaPerrito = glm::rotate(jerarquiaPerrito, glm::radians(180.0f + orientaPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPerrito = glm::scale(jerarquiaPerrito, glm::vec3(0.5f));

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(0.0f, 10.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoCuerpo.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(-10.0f, 10.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroCabezaPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoCabeza.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(-5.0f, 10.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroCuelloPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoCuello.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(10.0f, 7.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroColaPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoCola.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(-3.0f, 10.0f, 3.5f));
		modelOp = glm::rotate(modelOp, glm::radians(-giroPataDerFPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoPataDerF.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(-3.0f, 10.0f, -3.5f));
		modelOp = glm::rotate(modelOp, glm::radians(giroPataDerFPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoPataIzqF.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(3.0f, 10.0f, 3.5f));
		modelOp = glm::rotate(modelOp, glm::radians(-giroPataDerFPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoPataDerT.Draw(staticShader);

		modelOp = glm::translate(jerarquiaPerrito, glm::vec3(3.0f, 10.0f, -3.5f));
		modelOp = glm::rotate(modelOp, glm::radians(giroPataDerFPerrito), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		perritoPataIzqT.Draw(staticShader);
		 /////////////////////////////////////////////////////////////////////////////////ROBOT
		glm::mat4 jerarquiaRobot;

		jerarquiaRobot = glm::translate(glm::mat4(1.0f), glm::vec3(-210.0f + movRobotX, 0.0f, 0.0f + movRobotZ));
		jerarquiaRobot = glm::rotate(jerarquiaRobot, glm::radians(180.0f + orientaRobot), glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaRobot = glm::scale(jerarquiaRobot, glm::vec3(2.5f));
		staticShader.setMat4("model", jerarquiaRobot);
		robotCuerpo.Draw(staticShader);

		/////////////// DRON

		glm::mat4 dronBase = glm::mat4(1.0f);
		dronBase = glm::translate(dronBase, glm::vec3(dronX, dronY, dronZ));

		glm::mat4 modelCuerpo = dronBase;
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(0.8f));
		staticShader.setMat4("model", modelCuerpo);
		dronCuerpo.Draw(staticShader);

		// Helice 1
		glm::mat4 h1 = dronBase;
		h1 = glm::translate(h1, glm::vec3(19.549f, 5.0f, 15.0f));
		h1 = glm::rotate(h1, glm::radians(rotHelices), glm::vec3(0.0f, 1.0f, 0.0f));
		h1 = glm::scale(h1, glm::vec3(0.8f));
		staticShader.setMat4("model", h1);
		helice1.Draw(staticShader);

		// Helice 2
		glm::mat4 h2 = dronBase;
		h2 = glm::translate(h2, glm::vec3(9.324f, 5.0f, 25.14f));
		h2 = glm::rotate(h2, glm::radians(rotHelices), glm::vec3(0.0f, 1.0f, 0.0f));
		h2 = glm::scale(h2, glm::vec3(0.8f));
		staticShader.setMat4("model", h2);
		helice2.Draw(staticShader);

		// Helice 3
		glm::mat4 h3 = dronBase;
		h3 = glm::translate(h3, glm::vec3(0.0f, 5.0f, 15.916f));
		h3 = glm::rotate(h3, glm::radians(rotHelices), glm::vec3(0.0f, 1.0f, 0.0f));
		h3 = glm::scale(h3, glm::vec3(0.8f));
		staticShader.setMat4("model", h3);
		helice3.Draw(staticShader);

		// Helice 4
		glm::mat4 h4 = dronBase;
		h4 = glm::translate(h4, glm::vec3(9.324f, 5.0f, 10.679f));
		h4 = glm::rotate(h4, glm::radians(rotHelices), glm::vec3(0.0f, 1.0f, 0.0f));
		h4 = glm::scale(h4, glm::vec3(0.8f));
		staticShader.setMat4("model", h4);
		helice4.Draw(staticShader);
		//////////////////////////////////////////////////////////////////////////////////

		glm::mat4 grupoBotes = glm::translate(glm::mat4(1.0f),
			glm::vec3(353.892f, 0.0f, 722.162f));
		glm::mat4 baseGris = glm::translate(grupoBotes, glm::vec3(-20.0f, 0.0f, 0.0f));
		baseGris = glm::rotate(baseGris, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", baseGris);
		soporteBoteGris.Draw(staticShader);
		glm::mat4 BoteGrisMov = baseGris;
		BoteGrisMov = glm::rotate(BoteGrisMov, glm::radians(anguloBote1), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", BoteGrisMov);
		BoteGris.Draw(staticShader);
		glm::mat4 baseVerde = glm::translate(grupoBotes, glm::vec3(20.0f, 0.0f, 0.0f));
		baseVerde = glm::rotate(baseVerde, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", baseVerde);
		soporteBoteGris.Draw(staticShader);
		glm::mat4 BoteVerdeMov = baseVerde;
		BoteVerdeMov = glm::rotate(BoteVerdeMov, glm::radians(anguloBote2), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", BoteVerdeMov);
		BoteVerde.Draw(staticShader);


		staticShader.setBool("usarColorManual", false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);

	SDL_DestroyAudioStream(stream);
	SDL_free(wavBuffer);

	closeOpenAL();

	glfwTerminate();

	return 0;

}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	glm::vec3 currentPos = camera.Position;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	camera.Position.y = alturaActual;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		animacionRobot ^= true;
	}
	// 1. Puerta principal
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		alturaActual = 70.0f;
		camera.Position = glm::vec3(0.0f, alturaActual, 1000.0f);
		camera.Yaw = -90.0f;
		camera.Pitch = 0.0f;
		camera.ProcessMouseMovement(0, 0);
	}

	// 2. Elemento con animación (Pantalla)
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		alturaActual = 70.0f;
		camera.Position = glm::vec3(-29.5f, alturaActual, -500.0f);
		camera.Yaw = -90.0f;
		camera.Pitch = 40.0f;
		camera.ProcessMouseMovement(0, 0);
	}

	// 3. Elemento con gran detalle
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		alturaActual = 105.0f;
		camera.Position = glm::vec3(-694.441f, alturaActual, 702.916f);
		camera.Yaw = -218.1f;
		camera.Pitch = -15.4f;
		camera.ProcessMouseMovement(0, 0);
	}

	// 4. Elemento con gran detalle
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		alturaActual = 70.0f;
		camera.Position = glm::vec3(120.45f, alturaActual, -249.6f);
		camera.Yaw = 22.7f;
		camera.Pitch = -5.0f;
		camera.ProcessMouseMovement(0, 0);
	}

	// 5. Elemento con gran detalle
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		alturaActual = 70.0f;
		camera.Position = glm::vec3(-337.9f, alturaActual, -414.8f);
		camera.Yaw = -80.9f;
		camera.Pitch = -30.9f;
		camera.ProcessMouseMovement(0, 0);
	}

	// 6. Elemento con gran detalle
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		alturaActual = 50.0f;
		camera.Position = glm::vec3(-331.0f, alturaActual, 359.8);
		camera.Yaw = 56.3f;
		camera.Pitch = -27.6f;
		camera.ProcessMouseMovement(0, 0);
	}

	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		alturaActual = 50.0f;
	}

	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		alturaActual = 70.0f;
	}

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		alturaActual = 90.0f;
	}

	//perro
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		animacionPerrito ^= true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		playAnimacion = !playAnimacion;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		indexFord = 0;
		tFord = 0.0f;
		playAnimacion = false;
	}


	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}