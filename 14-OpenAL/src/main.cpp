#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

//Shadow includes
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow* window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesFountain;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
//shader para visualizar textura
Shader shaderViewTexture;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 5.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;
Box boxRenderImagen;
Box rio;

ShadowBox* shadowBox;

/*Modelos a utilizar en el Escenario 1 (Jardin)*/
//Perro
Model modelPerro;
//Arbol
Model modelArbol;
// Persona Haciendo Ejercicio
Model EjercicioModelAnimate;

/*Modelos a utilizar en el Escenario 2 (Alcantarilla)*/
Model modelBasura;
Model modelInsecto;
Model modelBote;
Model modelPopo;

/*Modelos a Utilizar en el Escenario 3 (Baños de la escuela)*/
// Toilet
Model modelToilet;
// Letrero
Model modelLetrero;
// Destapador
Model modelDestapador;
//Escoba
Model modelEscoba;
// Persona de Triste
Model TristeModelAnimate;
//Trabajador enojado
Model TrabajadorModelAnimate;

/*Modelos a Utilizar en el Escenario 4 (Pasillo de la escuela)*/
// Mochila
Model modelMochila;
// Planta
Model modelPlanta;
// Libro
Model modelLibro;
// Escritorio
Model modelEscritorio;
// Lapiz
Model modelLapiz;
// Silla
Model modelSilla;
// Regla
Model modelRegla;
// Jaula
Model modelJaula;
// Girl
Model GirlModelAnimate;
// Boy
Model BoyModelAnimate;

// Fountain
Model modelFountain;

/*Persona Principal (Hamster)*/
Model hamsModelAnimate;

// Terrain model instance
Terrain terrain(-1, -1, 200, 16, "../Textures/HighMap.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID, textureTitlePlayID, textureTitleCreditsID, textureTitleExitID, textureCreditsID, textureWinID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleFireID, texId;
GLuint skyboxTextureID;

GLuint texturaActivaID;

bool iniciaPartida = false, presionaOpcion = false;
int titleOpt = 0;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/Fondo2/nz.png",
		"../Textures/Fondo2/pz.png",
		"../Textures/Fondo2/py.png",
		"../Textures/Fondo2/ny.png",
		"../Textures/Fondo2/px.png",
		"../Textures/Fondo2/nx.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixFountain = glm::mat4(1.0f);
/*Personaje Principal*/
glm::mat4 modelMatrixHams = glm::mat4(1.0f);
/*Escenario 1*/
glm::mat4 modelMatrixEjercicio = glm::mat4(1.0f);
/*Escenario 3*/
glm::mat4 modelMatrixTriste = glm::mat4(1.0f);
glm::mat4 modelMatrixTrabajador = glm::mat4(1.0f);
/*Escenario 4*/
glm::mat4 modelMatrixGirl = glm::mat4(1.0f);
glm::mat4 modelMatrixBoy = glm::mat4(1.0f);
glm::mat4 matrixModelJaula = glm::mat4(1.0);

int animationIndex = 1;
int modelSelected = 2;

/* Posiciones que tendran los objetos en cascado dentro del escenario 1 */
//Arbol
std::vector<glm::vec3> arbolPosition = { glm::vec3(-39.0625, 4.5, -85.9375), glm::vec3(-82.8125, 4.5, -23.4375) };
std::vector<float> arbolOrientation = { 90, 90};
//Perro
std::vector<glm::vec3> perroPosition = { glm::vec3(-85.9375, 4.5, -77.34375), glm::vec3(-36.328125, 4.5, -34.765625),
	glm::vec3(-4.296875, 4.5, -38.671875)/*, glm::vec3(-84.375, 4.5, -48.046875)*/ };
std::vector<float> perroOrientation = { 180, -90, -90 };

/* Posiciones que tendran los objetos en cascado dentro del escenario 2 */
//Basura
std::vector<glm::vec3> basuraPosition = { glm::vec3(43.75, 0, -48.828125), glm::vec3(91.015625, 0, -38.28125) };
std::vector<float> basuraOrientation = { 0, 0};
//Insecto
std::vector<glm::vec3> insectoPosition = { glm::vec3(66.015625, 0.8, -72.65625), glm::vec3(32.03125, 0.8, -32.421875) };
std::vector<float> insectoOrientation = { -90, -90};
//Bote
std::vector<glm::vec3> botePosition = { glm::vec3(57.03125, 1.5, -97.265625), glm::vec3(72.65625, 1.5, -32.03125) };
std::vector<float> boteOrientation = { 0, 90};
//Popo
std::vector<glm::vec3> popoPosition = { glm::vec3(70.3125, 0.8, -47.65625), glm::vec3(54.296875, 0.8, -21.875),
	glm::vec3(73.046875, 3.75, 5.46875), glm::vec3(57.421875, 3.75, 5.46875), glm::vec3(47.65625, 3.75, 5.46875),
	glm::vec3(42.578125, 3.75, 25.0) };
std::vector<float> popoOrientation = { 0, 0, 90, 0, -90, 0};

/* Posiciones que tendran los objetos en cascado dentro del escenario 3 */
// Toilet
std::vector<glm::vec3> toiletPosition = { glm::vec3(73.046875, 4.2, 3.125), glm::vec3(57.421875, 4.2, 3.125),
	glm::vec3(47.65625, 4.2, 3.125), glm::vec3(40.234375, 4.2, 15.234375), glm::vec3(40.234375, 4.2, 25.0),
	glm::vec3(40.234375, 4.2, 34.765625), glm::vec3(40.234375, 4.2, 44.53125) };
std::vector<float> toiletOrientation = { 0, 0, 0, 90, 90, 90, 90 };
// Letrero
std::vector<glm::vec3> letreroPosition = { glm::vec3(96.09375, 4.5, 34.375), glm::vec3(3.90625, 4.5, 3.90625),
	glm::vec3(17.96875, 4.5, 58.984375) };
std::vector<float> letreroOrientation = { 0, 90, 0 };
// Destapador
std::vector<glm::vec3> destapadorPosition = { glm::vec3(31.25, 0, 17.1875), glm::vec3(31.25, 0, 40.625) };
std::vector<float> destapadorOrientation = { 0, 0 };
// Escoba
std::vector<glm::vec3> escobaPosition = { glm::vec3(18.203125, 3.55, 92.96875), glm::vec3(67.96875, 3.55, 93.359375),
	glm::vec3(55.46875, 3.55, 68.75) };
std::vector<float> escobaOrientation = { 0, 90, 90 };

/* Posiciones que tendran los objetos en cascado dentro del escenario 4 */
// Mochila
std::vector<glm::vec3> mochilaPosition = { glm::vec3(-47.265625, -3.9, 24.21875), glm::vec3(-80.46875, -3.9, 38.28125),
	glm::vec3(-59.375, -3.9, 161.71875) };
std::vector<float> mochilaOrientation = { 0, 0, 0 };
// Planta
std::vector<glm::vec3> plantaPosition = { glm::vec3(-35.546875, 0, 3.515625), glm::vec3(-61.328125, 0, 38.28125),
	glm::vec3(-60.9375, 0, 55.078125),glm::vec3(-90.625, 0, 89.84375), glm::vec3(-19.531255, 0, 80.468755),
	glm::vec3(-4.296875, 0, 91.40625) };
std::vector<float> plantaOrientation = { 0, 0, 0, 0, 0 };
// Libro
std::vector<glm::vec3> libroPosition = { glm::vec3(-25.78125, 0, 17.1875), glm::vec3(-53.515626, 0, 16.40625),
	glm::vec3(-33.984375, 0, 61.328125) };
std::vector<float> libroOrientation = { 0, 90, 0 };
// Escritorio
std::vector<glm::vec3> escritorioPosition = { glm::vec3(-78.90625, 0, 64.84375) };
std::vector<float> escritorioOrientation = { 0 };
// Lapiz
std::vector<glm::vec3> lapizPosition = { glm::vec3(-51.953125, 0, 66.796875), glm::vec3(-48.046875, 0, 9.765625) };
std::vector<float> lapizOrientation = { 0, 0 };
// Silla
std::vector<glm::vec3> sillaPosition = { glm::vec3(-91.015625, 4.5, 67.96875), glm::vec3(-49.218755, 4.5, 79.6875)/*,
	glm::vec3(-168.75, 4.5, -96.09375)*/ };
std::vector<float> sillaOrientation = { 90, 90, 90 };
// Regla
std::vector<glm::vec3> reglaPosition = { glm::vec3(-93.75, 4.5, 6.640625), glm::vec3(-11.71875, 4.5, 56.25) };
std::vector<float> reglaOrientation = { 90, 0 };
// Jaula
std::vector<glm::vec3> jaulaPosition = { glm::vec3(-78.515625, 4.5, 95.3125) };
std::vector<float> jaulaOrientation = { 0 };

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"aircraft", glm::vec3(10.0, 0.0, -17.5)},
		{"lambo", glm::vec3(23.0, 0.0, 0.0)},
		{"heli", glm::vec3(5.0, 10.0, -5.0)},
		{"fountain", glm::vec3(5.0, 0.0, -40.0)},
		{"fire", glm::vec3(0.0, 0.0, 7.0)}
};

double deltaTime;
double currTime, lastTime;

// Jump variables
bool isJump = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;
float angule;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

 // OpenAL Defines
#define NUM_BUFFERS 4
#define NUM_SOURCES 4
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
//Sourse 3
ALfloat source3Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source3Vel[] = { 0.0, 0.0, 0.0 };

// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = { true, true, true, true };

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
//void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat* data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for (unsigned int i = 0; i < nParticlesFire; i++) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine{};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	/*glfwSetScrollCallback(window, scrollCallback);*/
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderViewTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxRenderImagen.init();
	boxRenderImagen.setShader(&shaderViewTexture);
	boxRenderImagen.setScale(glm::vec3(2.0f, 2.0f, 1.0f));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	rio.init();
	rio.setShader(&shaderMulLighting);

	/* Ruta de los modelos que se utilizaran en el Escenario 1*/
	//Arbol
	modelArbol.loadModel("../models/Escenario1/arbol/arbol.obj");
	modelArbol.setShader(&shaderMulLighting);
	//Perro
	modelPerro.loadModel("../models/Escenario1/perro/perro.obj");
	modelPerro.setShader(&shaderMulLighting);
	//Persona haciendo ejercicio
	EjercicioModelAnimate.loadModel("../models/Escenario1/Jumping Jacks/Jumping Jacks.dae");
	EjercicioModelAnimate.setShader(&shaderMulLighting);

	/* Ruta de los modelos que se utilizaran en el Escenario 2*/
	//Basura
	modelBasura.loadModel("../models/Escenario2/Basura/basura.obj");
	modelBasura.setShader(&shaderMulLighting);
	//Insecto
	modelInsecto.loadModel("../models/Escenario2/Bug/bug.obj");
	modelInsecto.setShader(&shaderMulLighting);
	//Bote
	modelBote.loadModel("../models/Escenario2/bote/bote toxico.obj");
	modelBote.setShader(&shaderMulLighting);
	//Popo
	modelPopo.loadModel("../models/Escenario2/Poop/poop.obj");
	modelPopo.setShader(&shaderMulLighting);

	/* Ruta de los modelos que se utilizaran en el Escenario 3*/
	//Baño
	modelToilet.loadModel("../models/Escenario3/Toilet/Toilet.obj");
	modelToilet.setShader(&shaderMulLighting);
	//Letrero
	modelLetrero.loadModel("../models/Escenario3/letrero/letrero.obj");
	modelLetrero.setShader(&shaderMulLighting);
	//Destapador
	modelDestapador.loadModel("../models/Escenario3/Destapador/destapador.obj");
	modelDestapador.setShader(&shaderMulLighting);
	//Escoba
	modelEscoba.loadModel("../models/Escenario3/escoba/escoba-1.obj");
	modelEscoba.setShader(&shaderMulLighting);
	//Triste
	TristeModelAnimate.loadModel("../models/Escenario3/Sad Idle/Sad Idle.dae");
	TristeModelAnimate.setShader(&shaderMulLighting);
	//Trabajador
	TrabajadorModelAnimate.loadModel("../models/Escenario3/Angry/Angry.dae");
	TrabajadorModelAnimate.setShader(&shaderMulLighting);

	/* Ruta de los modelos que se utilizaran en el Escenario 4*/
	// Mochila
	modelMochila.loadModel("../models/Escenario4/Mochila/mochila.obj");
	modelMochila.setShader(&shaderMulLighting);
	// Planta
	modelPlanta.loadModel("../models/Escenario4/Flower/flower.obj");
	modelPlanta.setShader(&shaderMulLighting);
	// Libro
	modelLibro.loadModel("../models/Escenario4/Libro/libro.obj");
	modelLibro.setShader(&shaderMulLighting);
	// Escritorio
	modelEscritorio.loadModel("../models/Escenario4/Escritorio/escritorio.obj");
	modelEscritorio.setShader(&shaderMulLighting);
	// Lapiz
	modelLapiz.loadModel("../models/Escenario4/Pencil/pencil-01.obj");
	modelLapiz.setShader(&shaderMulLighting);
	// Silla
	modelSilla.loadModel("../models/Escenario4/Butaka/butaka.obj");
	modelSilla.setShader(&shaderMulLighting);
	// Regla
	modelRegla.loadModel("../models/Escenario4/regla/ruler.obj");
	modelRegla.setShader(&shaderMulLighting);
	// Jaula
	modelJaula.loadModel("../models/Escenario4/jaula/jaula.obj");
	modelJaula.setShader(&shaderMulLighting);
	//Girl
	GirlModelAnimate.loadModel("../models/Escenario4/Standing Greeting/Standing Greeting.dae");
	GirlModelAnimate.setShader(&shaderMulLighting);
	//Boy
	BoyModelAnimate.loadModel("../models/Escenario4/Surprised/Surprised.dae");
	BoyModelAnimate.setShader(&shaderMulLighting);

	//Fountain
	modelFountain.loadModel("../models/fountain/fountain.obj");
	modelFountain.setShader(&shaderMulLighting);

	//Mayow
	//mayowModelAnimate.loadModel("../models/mayow/personaje2.fbx");
	/*mayowModelAnimate.setShader(&shaderMulLighting);*/

	hamsModelAnimate.loadModel("../models/hams/hams.fbx");
	hamsModelAnimate.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(2.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/cesped.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTitlePlay("../Textures/Menu-Inicio.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTitlePlay.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTitlePlay.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTitlePlayID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTitlePlayID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTitlePlay.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTitleCredits("../Textures/Menu-Cred.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTitleCredits.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTitleCredits.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTitleCreditsID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTitleCreditsID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTitleCredits.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTitleExit("../Textures/Menu-Salida.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTitleExit.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTitleExit.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTitleExitID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTitleExitID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTitleExit.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureCredits("../Textures/Credits.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCredits.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCredits.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCreditsID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCreditsID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCredits.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureWin("../Textures/Win.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureWin.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureWin.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureWinID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWinID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWin.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureWall("../Textures/Water.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureWall.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureWall.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureWallID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWallID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWall.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureWindow("../Textures/ventana.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureWindow.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureWindow.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureWindowID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWindowID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWindow.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureHighway("../Textures/highway.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureHighway.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureHighway.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureHighwayID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureHighwayID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureHighway.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureLandingPad("../Textures/landingPad.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureLandingPad.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureLandingPad.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureLandingPadID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureLandingPadID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureLandingPad.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/grassflower.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/drenaje.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/Piso-school.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/Piso-bath.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	Texture textureParticlesFountain("../Textures/waterParticule.png");
	bitmap = textureParticlesFountain.loadImage();
	data = textureParticlesFountain.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFountainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesFountain.freeImage(bitmap);

	Texture textureParticleFire("../Textures/fire.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0, 1, 0);
	n = glm::cross(glm::vec3(1, 0, 0), v);
	if (glm::length(n) < 0.00001f) {
		n = glm::cross(glm::vec3(0, 1, 0), v);
	}
	u = glm::cross(v, n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	//buffer[0] = alutCreateBufferFromFile("../sounds/fountain.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/HamsDance.wav");
	//buffer[2] = alutCreateBufferFromFile("../sounds/darth_vader.wav");
	//buffer[3] = alutCreateBufferFromFile("../sounds/carroad.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 5.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);

	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 1.0f);
	alSourcefv(source[3], AL_POSITION, source3Pos);
	alSourcefv(source[3], AL_VELOCITY, source3Vel);
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_FALSE);
	alSourcef(source[3], AL_MAX_DISTANCE, 500);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();
	shaderViewTexture.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelFountain.destroy();

	/*Persona Principal*/
	hamsModelAnimate.destroy();

	/* Destruccion de los objetos en el Escenario 1*/
	modelArbol.destroy();
	modelPerro.destroy();
	// Custom objects animate
	EjercicioModelAnimate.destroy();

	/* Destruccion de los objetos en el Escenario 2*/
	modelBasura.destroy();
	modelInsecto.destroy();
	modelBote.destroy();
	modelPopo.destroy();

	/* Destruccion de los objetos en el Escenario 3*/
	modelToilet.destroy();
	modelLetrero.destroy();
	modelDestapador.destroy();
	modelEscoba.destroy();
	// Custom objects animate
	TristeModelAnimate.destroy();
	TrabajadorModelAnimate.destroy();

	/* Destruccion de los objetos en el Escenario 4*/
	modelMochila.destroy();
	modelPlanta.destroy();
	modelLibro.destroy();
	modelEscritorio.destroy();
	modelLapiz.destroy();
	modelSilla.destroy();
	modelRegla.destroy();
	modelJaula.destroy();
	// Custom objects animate
	GirlModelAnimate.destroy();
	BoyModelAnimate.destroy();

	// Custom objects animate
	//mayowModelAnimate.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureTitlePlayID);
	glDeleteTextures(1, &textureTitleCreditsID);
	glDeleteTextures(1, &textureTitleExitID);
	glDeleteTextures(1, &textureCreditsID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	if (!iniciaPartida) {
		bool statusEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
		if (titleOpt == 0 && statusEnter) {
			iniciaPartida = true;
		}
		else if (titleOpt == 1 && statusEnter) {
			texturaActivaID = textureCreditsID;
		}
		else if (titleOpt == 2 && statusEnter) {
			exitApp = true;
		}
		bool upPressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
		bool downPressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
		if (!presionaOpcion && (upPressed || downPressed)) {
			presionaOpcion = true;

			if (downPressed) {
				titleOpt += 1;
				if (titleOpt > 2)
					titleOpt = 0;
			}
			else if (upPressed) {
				titleOpt -= 1;
				if (titleOpt < 0)
					titleOpt = 2;
			}
			if (titleOpt == 0)
				texturaActivaID = textureTitlePlayID;
			else if (titleOpt == 1)
				texturaActivaID = textureTitleCreditsID;
			else if (titleOpt == 2)
				texturaActivaID = textureTitleExitID;
		}
		else if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && upPressed) ||
			(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && downPressed))
		{
			presionaOpcion = false;
			upPressed = false;
			downPressed = false;
		}
	}

	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE) {
		//std::cout << "Esta conectado el Joystick" << std::endl;
		const char* nombreJoy = glfwGetJoystickName(GLFW_JOYSTICK_1);
		//std::cout << "Nombre de Joystick" << nombreJoy << std::endl;
		int axisCount = 0, buttonCount = 0;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);

		if (fabs(axes[0]) >= 0.1f) {
			modelMatrixHams = glm::rotate(modelMatrixHams, -axes[0] * 0.01f, glm::vec3(0, 1, 0));
			animationIndex = 0;
		}
		if (fabs(axes[1]) >= 0.3f) {
			modelMatrixHams = glm::translate(modelMatrixHams, glm::vec3(0.0f, 0.0f, -axes[1] * 0.1f));
			animationIndex = 1;
		}
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	//if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
	//	modelMatrixHams = glm::rotate(modelMatrixHams, glm::radians(1.0f), glm::vec3(0, 1, 0));
	//	animationIndex = 1;
	//}
	//else if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
	//	modelMatrixHams = glm::rotate(modelMatrixHams, glm::radians(-1.0f), glm::vec3(0, 1, 0));
	//	animationIndex = 1;
	//}if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
	//	modelMatrixHams = glm::translate(modelMatrixHams, glm::vec3(0, 0, 1.0));
	//	animationIndex = 1;
	//}
	//else if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
	//	modelMatrixHams = glm::translate(modelMatrixHams, glm::vec3(0, 0, -1.0));
	//	animationIndex = 1;
	//}

	//bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	//if(!isJump && keySpaceStatus){
	//	isJump = true;
	//	startTimeJump = currTime;
	//	tmv = 0;
	//	sourcesPlay[3] = true;
	//}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	//modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(13.0f, 0.05f, -5.0f));
	//modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-90.0f), glm::vec3(0, 1, 0));

	modelMatrixHams = glm::translate(modelMatrixHams, glm::vec3(-95.703125, 0.05, -48.046875));
	modelMatrixHams = glm::rotate(modelMatrixHams, glm::radians(90.0f), glm::vec3(0, 1, 0));

	/*Modelos animados del escenario 1*/
	//Ejercicio
	modelMatrixEjercicio = glm::translate(modelMatrixEjercicio, glm::vec3(-78.125, 3.3f, -33.59375));

	/*Modelos animados del escenario 3*/
	//Trsite
	modelMatrixTriste = glm::translate(modelMatrixTriste, glm::vec3(87.5, 3.2f, 50.390625));
	//Trabajador
	modelMatrixTrabajador = glm::translate(modelMatrixTrabajador, glm::vec3(49.21875, 3.2, 86.328125));

	/*Modelos animados del escenario 4*/
	//Girl
	modelMatrixGirl = glm::translate(modelMatrixGirl, glm::vec3(-78.515625, 3.3f, 78.515625));
	//Boy
	modelMatrixBoy = glm::translate(modelMatrixBoy, glm::vec3(-19.140625, 3.3f, 89.0625));

	modelMatrixFountain = glm::translate(modelMatrixFountain, glm::vec3(5.0, 0.0, -40.0));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(modelMatrixFountain[3][0], modelMatrixFountain[3][2]) + 0.2;
	modelMatrixFountain = glm::scale(modelMatrixFountain, glm::vec3(10.0f, 10.0f, 10.0f));

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);
	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);
	texturaActivaID = textureTitlePlayID;

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		std::vector<float> matrixDartJoints;
		std::vector<glm::mat4> matrixDart;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!iniciaPartida) {
			glViewport(0, 0, screenWidth, screenHeight);
			shaderViewTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
			shaderViewTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texturaActivaID);
			boxRenderImagen.render();
			glfwSwapBuffers(window);
			continue;
		}

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		if (modelSelected == 2) {
			axis = glm::axis(glm::quat_cast(modelMatrixHams));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixHams));
			target = modelMatrixHams[3];
		}

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		if (modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		// Matriz de proyección del shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();
		glm::mat4 lightSpaceMatrix;
		lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));
		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		 //glm::vec3 spotPosition = glm::vec3(modelMatrixHeli * glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		 //shaderMulLighting.setInt("spotLightCount", 1);
		 //shaderTerrain.setInt("spotLightCount", 1);
		 //shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		 //shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		 //shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 //shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		 //shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		 //shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		 //shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		 //shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		 //shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		 //shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		 //shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		 //shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		 //shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 //shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		 //shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		 //shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		 //shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		 //shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		 //shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		 //shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));

		 ///*******************************************
		 // * Propiedades PointLights
		 // *******************************************/
		 //shaderMulLighting.setInt("pointLightCount", lamp1Position.size() + lamp2Orientation.size());
		 //shaderTerrain.setInt("pointLightCount", lamp1Position.size() + lamp2Orientation.size());
		 //for (int i = 0; i < lamp1Position.size(); i++) {
		 //	glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
		 //	matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp1Position[i]);
		 //	matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
		 //	matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5, 0.5, 0.5));
		 //	matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0, 10.3585, 0));
		 //	glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		 //}
		 //for (int i = 0; i < lamp2Position.size(); i++) {
		 //	glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
		 //	matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp2Position[i]);
		 //	matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
		 //	matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(1.0, 1.0, 1.0));
		 //	matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.759521, 5.00174, 0));
		 //	glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
		 //	shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
		 //	shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.01);
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
		 //	shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
		 //	shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.02);
		 //}

		 /*******************************************
		  * 1.- We render the depth buffer
		  *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		 // reset viewport
		 /*glViewport(0, 0, screenWidth, screenHeight);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 // render Depth map to quad for visual debugging
		 shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, depthMap);
		 boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		 boxViewDepth.render();*/

		 /*******************************************
		  * 2.- We render the normal objects
		  *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene();
		/*******************************************
		 * Debug to box light box
		 *******************************************/
		 /*glm::vec3 front = glm::normalize(-lightPos);
		 glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		 glm::vec3 up = glm::normalize(glm::cross(front, right));
		 glDisable(GL_CULL_FACE);
		 glm::mat4 boxViewTransform = glm::mat4(1.0f);
		 boxViewTransform = glm::translate(boxViewTransform, centerBox);
		 boxViewTransform[0] = glm::vec4(right, 0.0);
		 boxViewTransform[1] = glm::vec4(up, 0.0);
		 boxViewTransform[2] = glm::vec4(front, 0.0);
		 boxViewTransform = glm::scale(boxViewTransform, glm::vec3(shadowBox->getWidth(), shadowBox->getHeight(), shadowBox->getLength()));
		 boxLightViewBox.enableWireMode();
		 boxLightViewBox.render(boxViewTransform);
		 glEnable(GL_CULL_FACE);*/

		 /*******************************************
		  * Creacion de colliders
		  * IMPORTANT do this before interpolations
		  *******************************************/

		 /* Collider de los objetos del Escenario 1*/
		 // Arbol colliders
		 for (int i = 0; i < arbolPosition.size(); i++) {
		 	 AbstractModel::OBB arbolCollider;
			 glm::mat4 modelMatrixColliderArbol = glm::mat4(1.0);
			 modelMatrixColliderArbol = glm::translate(modelMatrixColliderArbol, arbolPosition[i]);
			 modelMatrixColliderArbol = glm::rotate(modelMatrixColliderArbol, glm::radians(arbolOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "arbol-" + std::to_string(i), arbolCollider, modelMatrixColliderArbol);
			 // Set the orientation of collider before doing the scale
			 arbolCollider.u = glm::quat_cast(modelMatrixColliderArbol);
			 modelMatrixColliderArbol = glm::scale(modelMatrixColliderArbol, glm::vec3(0.1, 0.1, 0.1));
			 modelMatrixColliderArbol = glm::translate(modelMatrixColliderArbol, modelArbol.getObb().c);
			 arbolCollider.c = glm::vec3(modelMatrixColliderArbol[3]);
			 arbolCollider.e = modelArbol.getObb().e * glm::vec3(0.1, 0.1, 0.1);
			 std::get<0>(collidersOBB.find("arbol-" + std::to_string(i))->second) = arbolCollider;
		 } 		 
		 // Perro colliders
		 for (int i = 0; i < perroPosition.size(); i++) {
			 AbstractModel::OBB perroCollider;
			 glm::mat4 modelMatrixColliderPerro = glm::mat4(1.0);
			 modelMatrixColliderPerro = glm::translate(modelMatrixColliderPerro, perroPosition[i]);
			 modelMatrixColliderPerro = glm::rotate(modelMatrixColliderPerro, glm::radians(perroOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "perro-" + std::to_string(i), perroCollider, modelMatrixColliderPerro);
			 // Set the orientation of collider before doing the scale
			 perroCollider.u = glm::quat_cast(modelMatrixColliderPerro);
			 modelMatrixColliderPerro = glm::scale(modelMatrixColliderPerro, glm::vec3(0.6, 0.6, 0.6));
			 modelMatrixColliderPerro = glm::translate(modelMatrixColliderPerro, modelPerro.getObb().c);
			 perroCollider.c = glm::vec3(modelMatrixColliderPerro[3]);
			 perroCollider.e = modelPerro.getObb().e * glm::vec3(0.6, 0.6, 0.6);
			 std::get<0>(collidersOBB.find("perro-" + std::to_string(i))->second) = perroCollider;
		 }
		 // Collider de	Ejercicio
		 AbstractModel::OBB ejercicioCollider;
		 glm::mat4 modelmatrixColliderEjercicio = glm::mat4(modelMatrixEjercicio);
		 modelmatrixColliderEjercicio = glm::rotate(modelmatrixColliderEjercicio, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		 // Set the orientation of collider before doing the scale
		 ejercicioCollider.u = glm::quat_cast(modelmatrixColliderEjercicio);
		 modelmatrixColliderEjercicio = glm::scale(modelmatrixColliderEjercicio, glm::vec3(0.005, 0.03, 0.03));
		 modelmatrixColliderEjercicio = glm::translate(modelmatrixColliderEjercicio, EjercicioModelAnimate.getObb().c);
		 ejercicioCollider.e = EjercicioModelAnimate.getObb().e * glm::vec3(0.005, 0.03, 0.03);
		 ejercicioCollider.c = glm::vec3(modelmatrixColliderEjercicio[3]);
		 addOrUpdateColliders(collidersOBB, "ejercicio", ejercicioCollider, modelMatrixEjercicio);
		
		 /* Collider de los objetos del escenario 2*/
		 // Basura colliders
		 for (int i = 0; i < basuraPosition.size(); i++) {
			 AbstractModel::OBB basuraCollider;
			 glm::mat4 modelMatrixColliderBasura = glm::mat4(1.0);
			 modelMatrixColliderBasura = glm::translate(modelMatrixColliderBasura, basuraPosition[i]);
			 modelMatrixColliderBasura = glm::rotate(modelMatrixColliderBasura, glm::radians(basuraOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "basura-" + std::to_string(i), basuraCollider, modelMatrixColliderBasura);
			 // Set the orientation of collider before doing the scale
			 basuraCollider.u = glm::quat_cast(modelMatrixColliderBasura);
			 modelMatrixColliderBasura = glm::scale(modelMatrixColliderBasura, glm::vec3(6.0, 6.0, 6.0));
			 modelMatrixColliderBasura = glm::translate(modelMatrixColliderBasura, modelBasura.getObb().c);
			 basuraCollider.c = glm::vec3(modelMatrixColliderBasura[3]);
			 basuraCollider.e = modelBasura.getObb().e * glm::vec3(6.0, 6.0, 6.0);
			 std::get<0>(collidersOBB.find("basura-" + std::to_string(i))->second) = basuraCollider;
		 }
		 // Insecto colliders
		 for (int i = 0; i < insectoPosition.size(); i++) {
			 AbstractModel::OBB insectoCollider;
			 glm::mat4 modelMatrixColliderInsecto = glm::mat4(1.0);
			 modelMatrixColliderInsecto = glm::translate(modelMatrixColliderInsecto, insectoPosition[i]);
			 modelMatrixColliderInsecto = glm::rotate(modelMatrixColliderInsecto, glm::radians(insectoOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "insecto-" + std::to_string(i), insectoCollider, modelMatrixColliderInsecto);
			 // Set the orientation of collider before doing the scale
			 insectoCollider.u = glm::quat_cast(modelMatrixColliderInsecto);
			 modelMatrixColliderInsecto = glm::scale(modelMatrixColliderInsecto, glm::vec3(0.4, 0.4, 0.4));
			 modelMatrixColliderInsecto = glm::translate(modelMatrixColliderInsecto, modelInsecto.getObb().c);
			 insectoCollider.c = glm::vec3(modelMatrixColliderInsecto[3]);
			 insectoCollider.e = modelInsecto.getObb().e * glm::vec3(0.4, 0.4, 0.4);
			 std::get<0>(collidersOBB.find("insecto-" + std::to_string(i))->second) = insectoCollider;
		 }
		 // Bote colliders
		 for (int i = 0; i < botePosition.size(); i++) {
			 AbstractModel::OBB boteCollider;
			 glm::mat4 modelMatrixColliderBote = glm::mat4(1.0);
			 modelMatrixColliderBote = glm::translate(modelMatrixColliderBote, botePosition[i]);
			 modelMatrixColliderBote = glm::rotate(modelMatrixColliderBote, glm::radians(boteOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "bote-" + std::to_string(i), boteCollider, modelMatrixColliderBote);
			 // Set the orientation of collider before doing the scale
			 boteCollider.u = glm::quat_cast(modelMatrixColliderBote);
			 modelMatrixColliderBote = glm::scale(modelMatrixColliderBote, glm::vec3(1.0, 1.0, 1.0));
			 modelMatrixColliderBote = glm::translate(modelMatrixColliderBote, modelBote.getObb().c);
			 boteCollider.c = glm::vec3(modelMatrixColliderBote[3]);
			 boteCollider.e = modelBote.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			 std::get<0>(collidersOBB.find("bote-" + std::to_string(i))->second) = boteCollider;
		 }
		 // Popo colliders
		 for (int i = 0; i < popoPosition.size(); i++) {
			 AbstractModel::OBB popoCollider;
			 glm::mat4 modelMatrixColliderPopo = glm::mat4(1.0);
			 modelMatrixColliderPopo = glm::translate(modelMatrixColliderPopo, popoPosition[i]);
			 modelMatrixColliderPopo = glm::rotate(modelMatrixColliderPopo, glm::radians(popoOrientation[i]), glm::vec3(0, 1, 0));
			 addOrUpdateColliders(collidersOBB, "popo-" + std::to_string(i), popoCollider, modelMatrixColliderPopo);
			 // Set the orientation of collider before doing the scale
			 popoCollider.u = glm::quat_cast(modelMatrixColliderPopo);
			 modelMatrixColliderPopo = glm::scale(modelMatrixColliderPopo, glm::vec3(0.1, 0.1, 0.1));
			 modelMatrixColliderPopo = glm::translate(modelMatrixColliderPopo, modelPopo.getObb().c);
			 popoCollider.c = glm::vec3(modelMatrixColliderPopo[3]);
			 popoCollider.e = modelPopo.getObb().e * glm::vec3(0.1, 0.1, 0.1);
			 std::get<0>(collidersOBB.find("popo-" + std::to_string(i))->second) = popoCollider;
		 }

		/* Collider de los objetos del escenario 3*/
		// Toilet colliders
		for (int i = 0; i < toiletPosition.size(); i++) {
			AbstractModel::OBB toiletCollider;
			glm::mat4 modelMatrixColliderToilet = glm::mat4(1.0);
			modelMatrixColliderToilet = glm::translate(modelMatrixColliderToilet, toiletPosition[i]);
			modelMatrixColliderToilet = glm::rotate(modelMatrixColliderToilet, glm::radians(toiletOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "toilet-" + std::to_string(i), toiletCollider, modelMatrixColliderToilet);
			// Set the orientation of collider before doing the scale
			toiletCollider.u = glm::quat_cast(modelMatrixColliderToilet);
			modelMatrixColliderToilet = glm::scale(modelMatrixColliderToilet, glm::vec3(0.7, 0.7, 0.7));
			modelMatrixColliderToilet = glm::translate(modelMatrixColliderToilet, modelToilet.getObb().c);
			toiletCollider.c = glm::vec3(modelMatrixColliderToilet[3]);
			toiletCollider.e = modelToilet.getObb().e * glm::vec3(0.7, 0.7, 0.7);
			std::get<0>(collidersOBB.find("toilet-" + std::to_string(i))->second) = toiletCollider;
		}
		// Letrero colliders
		for (int i = 0; i < letreroPosition.size(); i++) {
			AbstractModel::OBB letreroCollider;
			glm::mat4 modelMatrixColliderLetrero = glm::mat4(1.0);
			modelMatrixColliderLetrero = glm::translate(modelMatrixColliderLetrero, letreroPosition[i]);
			modelMatrixColliderLetrero = glm::rotate(modelMatrixColliderLetrero, glm::radians(letreroOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "letrero-" + std::to_string(i), letreroCollider, modelMatrixColliderLetrero);
			// Set the orientation of collider before doing the scale
			letreroCollider.u = glm::quat_cast(modelMatrixColliderLetrero);
			modelMatrixColliderLetrero = glm::scale(modelMatrixColliderLetrero, glm::vec3(1.0, 1.0, 1.0));
			modelMatrixColliderLetrero = glm::translate(modelMatrixColliderLetrero, modelLetrero.getObb().c);
			letreroCollider.c = glm::vec3(modelMatrixColliderLetrero[3]);
			letreroCollider.e = modelLetrero.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			std::get<0>(collidersOBB.find("letrero-" + std::to_string(i))->second) = letreroCollider;
		}
		// Destapador colliders
		for (int i = 0; i < destapadorPosition.size(); i++) {
			AbstractModel::OBB destapadorCollider;
			glm::mat4 modelMatrixColliderDestapador = glm::mat4(1.0);
			modelMatrixColliderDestapador = glm::translate(modelMatrixColliderDestapador, destapadorPosition[i]);
			modelMatrixColliderDestapador = glm::rotate(modelMatrixColliderDestapador, glm::radians(destapadorOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "destapador-" + std::to_string(i), destapadorCollider, modelMatrixColliderDestapador);
			// Set the orientation of collider before doing the scale
			destapadorCollider.u = glm::quat_cast(modelMatrixColliderDestapador);
			modelMatrixColliderDestapador = glm::scale(modelMatrixColliderDestapador, glm::vec3(0.5, 0.5, 0.5));
			modelMatrixColliderDestapador = glm::translate(modelMatrixColliderDestapador, modelDestapador.getObb().c);
			destapadorCollider.c = glm::vec3(modelMatrixColliderDestapador[3]);
			destapadorCollider.e = modelDestapador.getObb().e * glm::vec3(0.5, 0.5, 0.5);
			std::get<0>(collidersOBB.find("destapador-" + std::to_string(i))->second) = destapadorCollider;
		}
		// Escoba colliders
		for (int i = 0; i < escobaPosition.size(); i++) {
			AbstractModel::OBB escobaCollider;
			glm::mat4 modelMatrixColliderEscoba = glm::mat4(1.0);
			modelMatrixColliderEscoba = glm::translate(modelMatrixColliderEscoba, escobaPosition[i]);
			modelMatrixColliderEscoba = glm::rotate(modelMatrixColliderEscoba, glm::radians(escobaOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "escoba-" + std::to_string(i), escobaCollider, modelMatrixColliderEscoba);
			// Set the orientation of collider before doing the scale
			escobaCollider.u = glm::quat_cast(modelMatrixColliderEscoba);
			modelMatrixColliderEscoba = glm::scale(modelMatrixColliderEscoba, glm::vec3(0.3, 0.13, 0.3));
			modelMatrixColliderEscoba = glm::translate(modelMatrixColliderEscoba, modelEscoba.getObb().c);
			escobaCollider.c = glm::vec3(modelMatrixColliderEscoba[3]);
			escobaCollider.e = modelEscoba.getObb().e * glm::vec3(0.3, 0.13, 0.3);
			std::get<0>(collidersOBB.find("escoba-" + std::to_string(i))->second) = escobaCollider;
		}
		// Collider de Triste
		AbstractModel::OBB tristeCollider;
		glm::mat4 modelmatrixColliderTriste = glm::mat4(modelMatrixTriste);
		modelmatrixColliderTriste = glm::rotate(modelmatrixColliderTriste, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		tristeCollider.u = glm::quat_cast(modelmatrixColliderTriste);
		modelmatrixColliderTriste = glm::scale(modelmatrixColliderTriste, glm::vec3(0.015, 0.015, 0.015));
		modelmatrixColliderTriste = glm::translate(modelmatrixColliderTriste, TristeModelAnimate.getObb().c);
		tristeCollider.e = TristeModelAnimate.getObb().e * glm::vec3(0.015, 0.015, 0.015);
		tristeCollider.c = glm::vec3(modelmatrixColliderTriste[3]);
		addOrUpdateColliders(collidersOBB, "triste", tristeCollider, modelMatrixTriste);
		// Collider de Trabajador
		AbstractModel::OBB trabajadorCollider;
		glm::mat4 modelmatrixColliderTrabajador = glm::mat4(modelMatrixTrabajador);
		modelmatrixColliderTrabajador = glm::rotate(modelmatrixColliderTrabajador, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		trabajadorCollider.u = glm::quat_cast(modelmatrixColliderTrabajador);
		modelmatrixColliderTrabajador = glm::scale(modelmatrixColliderTrabajador, glm::vec3(0.03, 0.03, 0.03));
		modelmatrixColliderTrabajador = glm::translate(modelmatrixColliderTrabajador, TrabajadorModelAnimate.getObb().c);
		trabajadorCollider.e = TrabajadorModelAnimate.getObb().e * glm::vec3(0.03, 0.03, 0.03);
		trabajadorCollider.c = glm::vec3(modelmatrixColliderTrabajador[3]);
		addOrUpdateColliders(collidersOBB, "trabajador", trabajadorCollider, modelMatrixTrabajador);

		/* Collider de los objetos del escenario 4*/
		//Collider de la mochila
		for (int i = 0; i < mochilaPosition.size(); i++) {
			AbstractModel::OBB mochilaCollider;
			glm::mat4 modelMatrixColliderMochila = glm::mat4(1.0);
			modelMatrixColliderMochila = glm::translate(modelMatrixColliderMochila, mochilaPosition[i]);
			modelMatrixColliderMochila = glm::rotate(modelMatrixColliderMochila, glm::radians(mochilaOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "mochila-" + std::to_string(i), mochilaCollider, modelMatrixColliderMochila);
			// Set the orientation of collider before doing the scale
			mochilaCollider.u = glm::quat_cast(modelMatrixColliderMochila);
			modelMatrixColliderMochila = glm::scale(modelMatrixColliderMochila, glm::vec3(6.0, 6.0, 6.0));
			modelMatrixColliderMochila = glm::translate(modelMatrixColliderMochila, modelMochila.getObb().c);
			mochilaCollider.c = glm::vec3(modelMatrixColliderMochila[3]);
			mochilaCollider.e = modelMochila.getObb().e * glm::vec3(6.0, 6.0, 6.0);
			std::get<0>(collidersOBB.find("mochila-" + std::to_string(i))->second) = mochilaCollider;
		}
		//Collider de la planta
		for (int i = 0; i < plantaPosition.size(); i++) {
			AbstractModel::OBB plantaCollider;
			glm::mat4 modelMatrixColliderPlanta = glm::mat4(1.0);
			modelMatrixColliderPlanta = glm::translate(modelMatrixColliderPlanta, plantaPosition[i]);
			modelMatrixColliderPlanta = glm::rotate(modelMatrixColliderPlanta, glm::radians(plantaOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "planta-" + std::to_string(i), plantaCollider, modelMatrixColliderPlanta);
			// Set the orientation of collider before doing the scale
			plantaCollider.u = glm::quat_cast(modelMatrixColliderPlanta);
			modelMatrixColliderPlanta = glm::scale(modelMatrixColliderPlanta, glm::vec3(0.5, 0.5, 0.5));
			modelMatrixColliderPlanta = glm::translate(modelMatrixColliderPlanta, modelPlanta.getObb().c);
			plantaCollider.c = glm::vec3(modelMatrixColliderPlanta[3]);
			plantaCollider.e = modelPlanta.getObb().e * glm::vec3(0.5, 0.5, 0.5);
			std::get<0>(collidersOBB.find("planta-" + std::to_string(i))->second) = plantaCollider;
		}
		//Collider del Libro
		for (int i = 0; i < libroPosition.size(); i++) {
			AbstractModel::OBB libroCollider;
			glm::mat4 modelMatrixColliderLibro = glm::mat4(1.0);
			modelMatrixColliderLibro = glm::translate(modelMatrixColliderLibro, libroPosition[i]);
			modelMatrixColliderLibro = glm::rotate(modelMatrixColliderLibro, glm::radians(libroOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "libro-" + std::to_string(i), libroCollider, modelMatrixColliderLibro);
			// Set the orientation of collider before doing the scale
			libroCollider.u = glm::quat_cast(modelMatrixColliderLibro);
			modelMatrixColliderLibro = glm::scale(modelMatrixColliderLibro, glm::vec3(2.0, 3.0, 2.5));
			modelMatrixColliderLibro = glm::translate(modelMatrixColliderLibro, modelLibro.getObb().c);
			libroCollider.c = glm::vec3(modelMatrixColliderLibro[3]);
			libroCollider.e = modelLibro.getObb().e * glm::vec3(2.0, 3.0, 2.5);
			std::get<0>(collidersOBB.find("libro-" + std::to_string(i))->second) = libroCollider;
		}
		//Collider del Escritorio
		for (int i = 0; i < escritorioPosition.size(); i++) {
			AbstractModel::OBB escritorioCollider;
			glm::mat4 modelMatrixColliderEscritorio = glm::mat4(1.0);
			modelMatrixColliderEscritorio = glm::translate(modelMatrixColliderEscritorio, escritorioPosition[i]);
			modelMatrixColliderEscritorio = glm::rotate(modelMatrixColliderEscritorio, glm::radians(escritorioOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "escritorio-" + std::to_string(i), escritorioCollider, modelMatrixColliderEscritorio);
			// Set the orientation of collider before doing the scale
			escritorioCollider.u = glm::quat_cast(modelMatrixColliderEscritorio);
			modelMatrixColliderEscritorio = glm::scale(modelMatrixColliderEscritorio, glm::vec3(2.0, 1.0, 1.0));
			modelMatrixColliderEscritorio = glm::translate(modelMatrixColliderEscritorio, modelEscritorio.getObb().c);
			escritorioCollider.c = glm::vec3(modelMatrixColliderEscritorio[3]);
			escritorioCollider.e = modelEscritorio.getObb().e * glm::vec3(2.0, 1.0, 1.0);
			std::get<0>(collidersOBB.find("escritorio-" + std::to_string(i))->second) = escritorioCollider;
		}
		//Collider del Lapiz
		for (int i = 0; i < lapizPosition.size(); i++) {
			AbstractModel::OBB lapizCollider;
			glm::mat4 modelMatrixColliderLapiz = glm::mat4(1.0);
			modelMatrixColliderLapiz = glm::translate(modelMatrixColliderLapiz, lapizPosition[i]);
			modelMatrixColliderLapiz = glm::rotate(modelMatrixColliderLapiz, glm::radians(lapizOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lapiz-" + std::to_string(i), lapizCollider, modelMatrixColliderLapiz);
			// Set the orientation of collider before doing the scale
			lapizCollider.u = glm::quat_cast(modelMatrixColliderLapiz);
			modelMatrixColliderLapiz = glm::scale(modelMatrixColliderLapiz, glm::vec3(0.7, 0.7, 0.7));
			modelMatrixColliderLapiz = glm::translate(modelMatrixColliderLapiz, modelLapiz.getObb().c);
			lapizCollider.c = glm::vec3(modelMatrixColliderLapiz[3]);
			lapizCollider.e = modelLapiz.getObb().e * glm::vec3(0.7, 0.7, 0.7);
			std::get<0>(collidersOBB.find("lapiz-" + std::to_string(i))->second) = lapizCollider;
		}
		//Collider de la Silla
		for (int i = 0; i < sillaPosition.size(); i++) {
			AbstractModel::OBB sillaCollider;
			glm::mat4 modelMatrixColliderSilla = glm::mat4(1.0);
			modelMatrixColliderSilla = glm::translate(modelMatrixColliderSilla, sillaPosition[i]);
			modelMatrixColliderSilla = glm::rotate(modelMatrixColliderSilla, glm::radians(sillaOrientation[i]), glm::vec3(1, 0, 0));
			addOrUpdateColliders(collidersOBB, "silla-" + std::to_string(i), sillaCollider, modelMatrixColliderSilla);
			// Set the orientation of collider before doing the scale
			sillaCollider.u = glm::quat_cast(modelMatrixColliderSilla);
			modelMatrixColliderSilla = glm::scale(modelMatrixColliderSilla, glm::vec3(1.5, 1.5, 1.5));
			modelMatrixColliderSilla = glm::translate(modelMatrixColliderSilla, modelSilla.getObb().c);
			sillaCollider.c = glm::vec3(modelMatrixColliderSilla[3]);
			sillaCollider.e = modelSilla.getObb().e * glm::vec3(1.5, 1.5, 1.5);
			std::get<0>(collidersOBB.find("silla-" + std::to_string(i))->second) = sillaCollider;
		}
		//Collider de la Regla
		for (int i = 0; i < reglaPosition.size(); i++) {
			AbstractModel::OBB reglaCollider;
			glm::mat4 modelMatrixColliderRegla = glm::mat4(1.0);
			modelMatrixColliderRegla = glm::translate(modelMatrixColliderRegla, reglaPosition[i]);
			modelMatrixColliderRegla = glm::rotate(modelMatrixColliderRegla, glm::radians(reglaOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "regla-" + std::to_string(i), reglaCollider, modelMatrixColliderRegla);
			// Set the orientation of collider before doing the scale
			reglaCollider.u = glm::quat_cast(modelMatrixColliderRegla);
			modelMatrixColliderRegla = glm::scale(modelMatrixColliderRegla, glm::vec3(0.7, 0.7, 0.7));
			modelMatrixColliderRegla = glm::translate(modelMatrixColliderRegla, modelRegla.getObb().c);
			reglaCollider.c = glm::vec3(modelMatrixColliderRegla[3]);
			reglaCollider.e = modelRegla.getObb().e * glm::vec3(0.7, 0.7, 0.7);
			std::get<0>(collidersOBB.find("regla-" + std::to_string(i))->second) = reglaCollider;
		}
		// Collider de Girl
		AbstractModel::OBB girlCollider;
		glm::mat4 modelmatrixColliderGirl = glm::mat4(modelMatrixGirl);
		modelmatrixColliderGirl = glm::rotate(modelmatrixColliderGirl, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		girlCollider.u = glm::quat_cast(modelmatrixColliderGirl);
		modelmatrixColliderGirl = glm::scale(modelmatrixColliderGirl, glm::vec3(0.03, 0.03, 0.03));
		modelmatrixColliderGirl = glm::translate(modelmatrixColliderGirl, GirlModelAnimate.getObb().c);
		girlCollider.e = GirlModelAnimate.getObb().e * glm::vec3(0.03, 0.03, 0.03);
		girlCollider.c = glm::vec3(modelmatrixColliderGirl[3]);
		addOrUpdateColliders(collidersOBB, "girl", girlCollider, modelMatrixGirl);
		// Collider de Boy
		AbstractModel::OBB boyCollider;
		glm::mat4 modelmatrixColliderBoy = glm::mat4(modelMatrixBoy);
		modelmatrixColliderBoy = glm::rotate(modelmatrixColliderBoy, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		boyCollider.u = glm::quat_cast(modelmatrixColliderBoy);
		modelmatrixColliderBoy = glm::scale(modelmatrixColliderBoy, glm::vec3(0.0225, 0.0225, 0.0225));
		modelmatrixColliderBoy = glm::translate(modelmatrixColliderBoy, BoyModelAnimate.getObb().c);
		boyCollider.e = BoyModelAnimate.getObb().e * glm::vec3(0.0225, 0.0225, 0.0225);
		boyCollider.c = glm::vec3(modelmatrixColliderBoy[3]);
		addOrUpdateColliders(collidersOBB, "boy", boyCollider, modelMatrixBoy);

		//// Collider de mayow
		//AbstractModel::OBB mayowCollider;
		//glm::mat4 modelmatrixColliderMayow = glm::mat4(modelMatrixMayow);
		//modelmatrixColliderMayow = glm::rotate(modelmatrixColliderMayow,
		//		glm::radians(-90.0f), glm::vec3(1, 0, 0));
		//// Set the orientation of collider before doing the scale
		//mayowCollider.u = glm::quat_cast(modelmatrixColliderMayow);
		//modelmatrixColliderMayow = glm::scale(modelmatrixColliderMayow, glm::vec3(0.021, 0.021, 0.021));
		//modelmatrixColliderMayow = glm::translate(modelmatrixColliderMayow,
		//		glm::vec3(mayowModelAnimate.getObb().c.x,
		//				mayowModelAnimate.getObb().c.y,
		//				mayowModelAnimate.getObb().c.z));
		//mayowCollider.e = mayowModelAnimate.getObb().e * glm::vec3(0.021, 0.021, 0.021) * glm::vec3(0.787401574, 0.787401574, 0.787401574);
		//mayowCollider.c = glm::vec3(modelmatrixColliderMayow[3]);
		//addOrUpdateColliders(collidersOBB, "mayow", mayowCollider, modelMatrixMayow);

		// Collider de Hams
		AbstractModel::OBB hamsCollider;
		glm::mat4 modelmatrixColliderHams = glm::mat4(modelMatrixHams);
		modelmatrixColliderHams = glm::rotate(modelmatrixColliderHams, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		// Set the orientation of collider before doing the scale
		hamsCollider.u = glm::quat_cast(modelmatrixColliderHams);
		modelmatrixColliderHams = glm::scale(modelmatrixColliderHams, glm::vec3(0.021, 0.021, 0.021));
		modelmatrixColliderHams = glm::translate(modelmatrixColliderHams, hamsModelAnimate.getObb().c);
		hamsCollider.e = hamsModelAnimate.getObb().e * glm::vec3(1.7f, 0.75f, 3.5f);
		hamsCollider.c = glm::vec3(modelmatrixColliderHams[3]);
		addOrUpdateColliders(collidersOBB, "hams", hamsCollider, modelMatrixHams);

		/*******************************************
		 * Render de colliders
		 *******************************************/
		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			//boxCollider.render(matrixCollider);
		}

		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			//sphereCollider.render(matrixCollider);
		}

		/*******************************************
		 * Test Colisions
		 *******************************************/
		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt
					&& testOBBOBB(std::get<0>(it->second),
						std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
						<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt
					&& testSphereSphereIntersection(std::get<0>(it->second),
						std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
						<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
					std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
						<< jt->first << std::endl;
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end();
			colIt++) {
			std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.find(colIt->first);
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second) {
					if (jt->first.compare("hams") == 0 && angule < 0.99f) {
						modelMatrixHams = std::get<1>(jt->second);
					}
					else {
						addOrUpdateColliders(collidersOBB, jt->first);
					}

				}
				else {
					if (jt->first.compare("hams") == 0)
						modelMatrixHams = std::get<1>(jt->second);
					/*if (jt->first.compare("dart") == 0)
						modelMatrixDart = std::get<1>(jt->second);*/
				}
			}
		}

		/*******************************************
		* Interpolation key frames with disconect objects
		*******************************************/
		glfwSwapBuffers(window);


		/****************************+
		 * Open AL sound data
		 */
		 //source0Pos[0] = modelMatrixFountain[3].x;
		 //source0Pos[1] = modelMatrixFountain[3].y;
		 //source0Pos[2] = modelMatrixFountain[3].z;
		 //alSourcefv(source[0], AL_POSITION, source0Pos);

		 //source2Pos[0] = modelMatrixDart[3].x;
		 //source2Pos[1] = modelMatrixDart[3].y;
		 //source2Pos[2] = modelMatrixDart[3].z;
		 //alSourcefv(source[2], AL_POSITION, source2Pos);

		 //source3Pos[0] = modelMatrixLambo[3].x;
		 //source3Pos[1] = modelMatrixLambo[3].y;
		 //source3Pos[2] = modelMatrixLambo[3].z;
		 //alSourcefv(source[3], AL_POSITION, source3Pos);



		 // Listener for the Thris person camera
		listenerPos[0] = modelMatrixHams[3].x;
		listenerPos[1] = modelMatrixHams[3].y;
		listenerPos[2] = modelMatrixHams[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(modelMatrixHams[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixHams[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		// Listener for the First person camera
		/*listenerPos[0] = camera->getPosition().x;
		listenerPos[1] = camera->getPosition().y;
		listenerPos[2] = camera->getPosition().z;
		alListenerfv(AL_POSITION, listenerPos);
		listenerOri[0] = camera->getFront().x;
		listenerOri[1] = camera->getFront().y;
		listenerOri[2] = camera->getFront().z;
		listenerOri[3] = camera->getUp().x;
		listenerOri[4] = camera->getUp().y;
		listenerOri[5] = camera->getUp().z;*/
		alListenerfv(AL_ORIENTATION, listenerOri);

		for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
			if (sourcesPlay[i]) {
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}
	}
}

void prepareScene() {

	skyboxSphere.setShader(&shaderSkybox);

	terrain.setShader(&shaderTerrain);

	//Hams
	hamsModelAnimate.setShader(&shaderMulLighting);

	/* Carga de shader a los objetos del Escenario 1*/
	//Arbol
	modelArbol.setShader(&shaderMulLighting);
	//Perro
	modelPerro.setShader(&shaderMulLighting);
	//Ejercicio
	EjercicioModelAnimate.setShader(&shaderMulLighting);

	/* Carga de shader a los objetos del Escenario 2*/
	//Basura
	modelBasura.setShader(&shaderMulLighting);
	//Insecto
	modelInsecto.setShader(&shaderMulLighting);
	//Bote
	modelBote.setShader(&shaderMulLighting);
	//Popo
	modelPopo.setShader(&shaderMulLighting);

	/* Carga de shader a los objetos del Escenario 3*/
	//Toilet
	modelToilet.setShader(&shaderMulLighting);
	//Letrero
	modelLetrero.setShader(&shaderMulLighting);
	//Destapador
	modelDestapador.setShader(&shaderMulLighting);
	//Escoba
	modelEscoba.setShader(&shaderMulLighting);
	//Triste
	TristeModelAnimate.setShader(&shaderMulLighting);
	//Trabajdor
	TrabajadorModelAnimate.setShader(&shaderMulLighting);

	/* Carga de shader a los objetos del Escenario 4*/
	//Mochila
	modelMochila.setShader(&shaderMulLighting);
	//Planta
	modelPlanta.setShader(&shaderMulLighting);
	//Libro
	modelLibro.setShader(&shaderMulLighting);
	//Escritorio
	modelEscritorio.setShader(&shaderMulLighting);
	//Lapiz
	modelLapiz.setShader(&shaderMulLighting);
	//Silla
	modelSilla.setShader(&shaderMulLighting);
	//Regla
	modelRegla.setShader(&shaderMulLighting);
	//Jaula
	modelJaula.setShader(&shaderMulLighting);
	//Girl
	GirlModelAnimate.setShader(&shaderMulLighting);
	//Boy
	BoyModelAnimate.setShader(&shaderMulLighting);

	//Mayow
	//mayowModelAnimate.setShader(&shaderMulLighting);
}

void prepareDepthScene() {

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	//Hams
	hamsModelAnimate.setShader(&shaderDepth);

	/* Carga de shader a los objetos del Escenario 1*/
	//Arbol
	modelArbol.setShader(&shaderDepth);
	//Perro
	modelPerro.setShader(&shaderDepth);
	//Ejercicio
	EjercicioModelAnimate.setShader(&shaderDepth);

	/* Carga de shader a los objetos del Escenario 2*/
	//Basura
	modelBasura.setShader(&shaderDepth);
	//Insecto
	modelInsecto.setShader(&shaderDepth);
	//Bote
	modelBote.setShader(&shaderDepth);
	//Popo
	modelPopo.setShader(&shaderDepth);

	/* Carga de shader de sombra a los objetos del Escenario 3*/
	//Baño
	modelToilet.setShader(&shaderDepth);
	//Letrero
	modelLetrero.setShader(&shaderDepth);
	//Desptapador
	modelDestapador.setShader(&shaderDepth);
	//Escoba
	modelEscoba.setShader(&shaderDepth);
	//Trsite
	TristeModelAnimate.setShader(&shaderDepth);
	//Trabajador
	TrabajadorModelAnimate.setShader(&shaderDepth);

	/* Carga de shader de sombra a los objetos del Escenario 4*/
	//Mochila
	modelMochila.setShader(&shaderDepth);
	//Planta
	modelPlanta.setShader(&shaderDepth);
	//Libro
	modelLibro.setShader(&shaderDepth);
	//Escritorio
	modelEscritorio.setShader(&shaderDepth);
	//Lapiz
	modelLapiz.setShader(&shaderDepth);
	//Silla
	modelSilla.setShader(&shaderDepth);
	//Regla
	modelRegla.setShader(&shaderDepth);
	//Jaula
	modelJaula.setShader(&shaderDepth);
	//Girl
	GirlModelAnimate.setShader(&shaderDepth);
	//Boy
	BoyModelAnimate.setShader(&shaderDepth);

	////Mayow
	//mayowModelAnimate.setShader(&shaderDepth);
}

void renderScene(bool renderParticles) {
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureWallID);
	rio.setPosition(glm::vec3(57.8125, 0.1, -44.336));
	rio.setScale(glm::vec3(3.75f, 0.1f, 90.0f));
	shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1, 20)));
	rio.render();
	shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));

	/*******************************************
	* Custom objects obj
	*******************************************/

	//// Fountain
	//glDisable(GL_CULL_FACE);
	//modelFountain.render(modelMatrixFountain);
	//glEnable(GL_CULL_FACE);
	// 
	// Se regresa el cull faces IMPORTANTE para la capa
	glEnable(GL_CULL_FACE);

	/* Renderizacion de los objetos en cascada de los objetos del Escenario 1*/
	//Arbol
	for (int i = 0; i < arbolPosition.size(); i++) {
		arbolPosition[i].y = terrain.getHeightTerrain(arbolPosition[i].x, arbolPosition[i].z);
		modelArbol.setPosition(arbolPosition[i]);
		modelArbol.setScale(glm::vec3(0.1, 0.1, 0.1));
		modelArbol.setOrientation(glm::vec3(0, arbolOrientation[i], 0));
		modelArbol.render();
	} 	
	//Perro
	for (int i = 0; i < perroPosition.size(); i++) {
		perroPosition[i].y = terrain.getHeightTerrain(perroPosition[i].x, perroPosition[i].z);
		modelPerro.setPosition(perroPosition[i]);
		modelPerro.setScale(glm::vec3(0.6, 0.6, 0.6));
		modelPerro.setOrientation(glm::vec3(0, perroOrientation[i], 0));
		modelPerro.render();
	}

	/* Renderizacion de los objetos en cascada de los objetos del Escenario 3*/
	//Basura
	for (int i = 0; i < basuraPosition.size(); i++) {
		basuraPosition[i].y = terrain.getHeightTerrain(basuraPosition[i].x, basuraPosition[i].z);
		modelBasura.setPosition(basuraPosition[i]);
		modelBasura.setScale(glm::vec3(6.0, 6.0, 6.0));
		modelBasura.setOrientation(glm::vec3(0, basuraOrientation[i], 0));
		modelBasura.render();
	}
	//Insecto
	for (int i = 0; i < insectoPosition.size(); i++) {
		//insectoPosition[i].y = terrain.getHeightTerrain(insectoPosition[i].x, insectoPosition[i].z);
		modelInsecto.setPosition(insectoPosition[i]);
		modelInsecto.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelInsecto.setOrientation(glm::vec3(0, insectoOrientation[i], 0));
		modelInsecto.render();
	}
	//Bote
	for (int i = 0; i < botePosition.size(); i++) {
		//botePosition[i].y = terrain.getHeightTerrain(botePosition[i].x, botePosition[i].z);
		modelBote.setPosition(botePosition[i]);
		modelBote.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelBote.setOrientation(glm::vec3(0, boteOrientation[i], 0));
		modelBote.render();
	}
	//Popo
	for (int i = 0; i < popoPosition.size(); i++) {
		//popoPosition[i].y = terrain.getHeightTerrain(popoPosition[i].x, popoPosition[i].z);
		modelPopo.setPosition(popoPosition[i]);
		modelPopo.setScale(glm::vec3(0.1, 0.1, 0.1));
		modelPopo.setOrientation(glm::vec3(0, popoOrientation[i], 0));
		modelPopo.render();
	}

	/* Renderizacion de los objetos en cascada de los objetos del Escenario 3*/
	//Toilet
	for (int i = 0; i < toiletPosition.size(); i++) {
		/*toiletPosition[i].y = terrain.getHeightTerrain(toiletPosition[i].x, toiletPosition[i].z);*/
		modelToilet.setPosition(toiletPosition[i]);
		modelToilet.setScale(glm::vec3(0.7, 0.7, 0.7));
		modelToilet.setOrientation(glm::vec3(0, toiletOrientation[i], 0));
		modelToilet.render();
	}
	//Letrero
	for (int i = 0; i < letreroPosition.size(); i++) {
		/*letreroPosition[i].y = terrain.getHeightTerrain(letreroPosition[i].x, letreroPosition[i].z);*/
		modelLetrero.setPosition(letreroPosition[i]);
		modelLetrero.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLetrero.setOrientation(glm::vec3(0, letreroOrientation[i], 0));
		modelLetrero.render();
	}
	//Destapador
	for (int i = 0; i < destapadorPosition.size(); i++) {
		destapadorPosition[i].y = terrain.getHeightTerrain(destapadorPosition[i].x, destapadorPosition[i].z);
		modelDestapador.setPosition(destapadorPosition[i]);
		modelDestapador.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelDestapador.setOrientation(glm::vec3(0, destapadorOrientation[i], 0));
		modelDestapador.render();
	}
	//Escoba
	for (int i = 0; i < escobaPosition.size(); i++) {
		/*escobaPosition[i].y = terrain.getHeightTerrain(escobaPosition[i].x, escobaPosition[i].z);*/
		modelEscoba.setPosition(escobaPosition[i]);
		modelEscoba.setScale(glm::vec3(0.3, 0.13, 0.3));
		modelEscoba.setOrientation(glm::vec3(0, escobaOrientation[i], 0));
		modelEscoba.render();
	}

	/* Renderizacion de los objetos en cascada de los objetos del Escenario 4*/
	//Mochila
	for (int i = 0; i < mochilaPosition.size(); i++) {
		/*mochilaPosition[i].y = terrain.getHeightTerrain(mochilaPosition[i].x, mochilaPosition[i].z);*/
		modelMochila.setPosition(mochilaPosition[i]);
		modelMochila.setScale(glm::vec3(6.0, 6.0, 6.0));
		modelMochila.setOrientation(glm::vec3(0, mochilaOrientation[i], 0));
		modelMochila.render();
	}
	//Planta
	for (int i = 0; i < plantaPosition.size(); i++) {
		plantaPosition[i].y = terrain.getHeightTerrain(plantaPosition[i].x, plantaPosition[i].z);
		modelPlanta.setPosition(plantaPosition[i]);
		modelPlanta.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelPlanta.setOrientation(glm::vec3(0, plantaOrientation[i], 0));
		modelPlanta.render();
	}
	//Libro
	for (int i = 0; i < libroPosition.size(); i++) {
		libroPosition[i].y = terrain.getHeightTerrain(libroPosition[i].x, libroPosition[i].z);
		modelLibro.setPosition(libroPosition[i]);
		modelLibro.setScale(glm::vec3(2.0, 3.0, 2.5));
		modelLibro.setOrientation(glm::vec3(libroOrientation[i], 0, 0));
		modelLibro.render();
	}
	//Escritorio
	for (int i = 0; i < escritorioPosition.size(); i++) {
		escritorioPosition[i].y = terrain.getHeightTerrain(escritorioPosition[i].x, escritorioPosition[i].z);
		modelEscritorio.setPosition(escritorioPosition[i]);
		modelEscritorio.setScale(glm::vec3(2.0, 1.0, 1.0));
		modelEscritorio.setOrientation(glm::vec3(0, escritorioOrientation[i], 0));
		modelEscritorio.render();
	}
	//Lapiz
	for (int i = 0; i < lapizPosition.size(); i++) {
		lapizPosition[i].y = terrain.getHeightTerrain(lapizPosition[i].x, lapizPosition[i].z);
		modelLapiz.setPosition(lapizPosition[i]);
		modelLapiz.setScale(glm::vec3(0.7, 0.7, 0.7));
		modelLapiz.setOrientation(glm::vec3(0, lapizOrientation[i], 0));
		modelLapiz.render();
	}
	//Silla
	for (int i = 0; i < sillaPosition.size(); i++) {
		/*sillaPosition[i].y = terrain.getHeightTerrain(sillaPosition[i].x, sillaPosition[i].z);*/
		modelSilla.setPosition(sillaPosition[i]);
		modelSilla.setScale(glm::vec3(1.5, 1.5, 1.5));
		modelSilla.setOrientation(glm::vec3(sillaOrientation[i], 0, 0));
		modelSilla.render();
	}
	//Regla
	for (int i = 0; i < reglaPosition.size(); i++) {
		reglaPosition[i].y = terrain.getHeightTerrain(reglaPosition[i].x, reglaPosition[i].z);
		modelRegla.setPosition(reglaPosition[i]);
		modelRegla.setScale(glm::vec3(0.7, 0.7, 0.7));
		modelRegla.setOrientation(glm::vec3(0, reglaOrientation[i], 0));
		modelRegla.render();
	}
	//Regla
	for (int i = 0; i < jaulaPosition.size(); i++) {
		/*jaulaPosition[i].y = terrain.getHeightTerrain(jaulaPosition[i].x, jaulaPosition[i].z);*/
		modelJaula.setPosition(jaulaPosition[i]);
		modelJaula.setScale(glm::vec3(2.0, 2.0, 2.0));
		modelJaula.setOrientation(glm::vec3(0, jaulaOrientation[i], 0));
		modelJaula.render();
	}

	/*******************************************
	//* Custom Anim objects obj
	//*******************************************/

	/*Modelos animados del Escenario 1*/
	//Ejercicio
	glm::mat4 modelMatrixEjercicioBody = glm::mat4(modelMatrixEjercicio);
	modelMatrixEjercicioBody = glm::scale(modelMatrixEjercicioBody, glm::vec3(0.03f));
	EjercicioModelAnimate.render(modelMatrixEjercicioBody);

	/*Modelos animados del Escenario 3*/
	//Triste
	glm::mat4 modelMatrixTristeBody = glm::mat4(modelMatrixTriste);
	modelMatrixTristeBody = glm::scale(modelMatrixTristeBody, glm::vec3(0.015f));
	TristeModelAnimate.render(modelMatrixTristeBody);
	//Trabajador
	glm::mat4 modelMatrixTrabajadorBody = glm::mat4(modelMatrixTrabajador);
	modelMatrixTrabajadorBody = glm::scale(modelMatrixTrabajadorBody, glm::vec3(0.03f));
	TrabajadorModelAnimate.render(modelMatrixTrabajadorBody);

	/*Modelos animados del Escenario 4*/
	//Girl
	glm::mat4 modelMatrixGirlBody = glm::mat4(modelMatrixGirl);
	modelMatrixGirlBody = glm::scale(modelMatrixGirlBody, glm::vec3(0.03f));
	GirlModelAnimate.render(modelMatrixGirlBody);
	//Boy
	glm::mat4 modelMatrixBoyBody = glm::mat4(modelMatrixBoy);
	modelMatrixBoyBody = glm::scale(modelMatrixBoyBody, glm::vec3(0.0225f));
	BoyModelAnimate.render(modelMatrixBoyBody);

	modelMatrixHams[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv + terrain.getHeightTerrain(modelMatrixHams[3][0], modelMatrixHams[3][2]);
	tmv = currTime - startTimeJump;
	if (modelMatrixHams[3][1] < terrain.getHeightTerrain(modelMatrixHams[3][0], modelMatrixHams[3][2])) {
		isJump = false;
		modelMatrixHams[3][1] = terrain.getHeightTerrain(modelMatrixHams[3][0], modelMatrixHams[3][2]);
	}
	//modelMatrixMayow[3][1] = terrain.getHeightTerrain(modelMatrixMayow[3][0], modelMatrixMayow[3][2]);
	glm::mat4 modelMatrixHamsBody = glm::mat4(modelMatrixHams);
	modelMatrixHamsBody = glm::translate(modelMatrixHams, glm::vec3(0, 0.35f, 0));
	modelMatrixHamsBody = glm::scale(modelMatrixHamsBody, glm::vec3(0.01, 0.01, 0.01));
	glm::vec3 normalTerrain = glm::normalize(terrain.getNormalTerrain(modelMatrixHams[3][0], modelMatrixHams[3][2]));
	glm::vec3 upHams = glm::normalize(modelMatrixHamsBody[1]);
	angule = glm::dot(normalTerrain, upHams);
	/*angule = 1.0;*/
	hamsModelAnimate.setAnimationIndex(animationIndex);
	hamsModelAnimate.render(modelMatrixHamsBody);

	///**********
	// * Update the position with alpha objects
	// */
	//// Update the aircraft
	//blendingUnsorted.find("aircraft")->second = glm::vec3(modelMatrixAircraft[3]);
	//// Update the lambo
	//blendingUnsorted.find("lambo")->second = glm::vec3(modelMatrixLambo[3]);
	//// Update the helicopter
	//blendingUnsorted.find("heli")->second = glm::vec3(modelMatrixHeli[3]);

	/**********
	* Sorter with alpha objects
	*/
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++) {
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	* Render de las transparencias
	*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
		if (it->second.first.compare("aircraft") == 0) {
			// Render for the aircraft model
			//glm::mat4 modelMatrixAircraftBlend = glm::mat4(modelMatrixAircraft);
			//modelMatrixAircraftBlend[3][1] = terrain.getHeightTerrain(modelMatrixAircraftBlend[3][0], modelMatrixAircraftBlend[3][2]) + 2.0;
			//modelAircraft.render(modelMatrixAircraftBlend);
		}
		else if (it->second.first.compare("lambo") == 0) {
			// Lambo car
			//glm::mat4 modelMatrixLamboBlend = glm::mat4(modelMatrixLambo);
			//modelMatrixLamboBlend[3][1] = terrain.getHeightTerrain(modelMatrixLamboBlend[3][0], modelMatrixLamboBlend[3][2]);
			//modelMatrixLamboBlend = glm::scale(modelMatrixLamboBlend, glm::vec3(1.3, 1.3, 1.3));
			//modelLambo.render(modelMatrixLamboBlend);
			glActiveTexture(GL_TEXTURE0);
			//glm::mat4 modelMatrixLamboLeftDor = glm::mat4(modelMatrixLamboBlend);
			//modelMatrixLamboLeftDor = glm::translate(modelMatrixLamboLeftDor, glm::vec3(1.08676, 0.707316, 0.982601));
			//modelMatrixLamboLeftDor = glm::rotate(modelMatrixLamboLeftDor, glm::radians(dorRotCount), glm::vec3(1.0, 0, 0));
			//modelMatrixLamboLeftDor = glm::translate(modelMatrixLamboLeftDor, glm::vec3(-1.08676, -0.707316, -0.982601));
 /*			modelLamboLeftDor.render(modelMatrixLamboLeftDor);
			modelLamboRightDor.render(modelMatrixLamboBlend);
			modelLamboFrontLeftWheel.render(modelMatrixLamboBlend);
			modelLamboFrontRightWheel.render(modelMatrixLamboBlend);
			modelLamboRearLeftWheel.render(modelMatrixLamboBlend);
			modelLamboRearRightWheel.render(modelMatrixLamboBlend)*/;
			// Se regresa el cull faces IMPORTANTE para las puertas
		}
		else if (it->second.first.compare("heli") == 0) {
			// Helicopter
			/*glm::mat4 modelMatrixHeliChasis = glm::mat4(modelMatrixHeli);*/
			//modelHeliChasis.render(modelMatrixHeliChasis);

			//glm::mat4 modelMatrixHeliHeli = glm::mat4(modelMatrixHeliChasis);
			//modelMatrixHeliHeli = glm::translate(modelMatrixHeliHeli, glm::vec3(0.0, 0.0, -0.249548));
			/*modelMatrixHeliHeli = glm::rotate(modelMatrixHeliHeli, rotHelHelY, glm::vec3(0, 1, 0));*/
			//modelMatrixHeliHeli = glm::translate(modelMatrixHeliHeli, glm::vec3(0.0, 0.0, 0.249548));
			/*modelHeliHeli.render(modelMatrixHeliHeli);*/
		}
		else if (renderParticles && it->second.first.compare("fountain") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
			modelMatrixParticlesFountain = glm::translate(modelMatrixParticlesFountain, glm::vec3(73.046875, 4.2, 3.125));
			modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(modelMatrixParticlesFountain[3][0], modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
			modelMatrixParticlesFountain = glm::scale(modelMatrixParticlesFountain, glm::vec3(3.0, 3.0, 3.0));
			modelMatrixParticlesFountain = glm::translate(modelMatrixParticlesFountain, glm::vec3(0, -0.75, 0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
			/*shaderParticlesFountain.turnOn();*/
			shaderParticlesFountain.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesFountain.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesFountain.setInt("ParticleTex", 0);
			shaderParticlesFountain.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, -0.3f, 0.0f)));
			shaderParticlesFountain.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesFountain));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			/*shaderParticlesFountain.turnOff();*/
			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("fire") == 0) {
			/**********
			 * Init Render particles systems
			 */
			//lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			//currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			//shaderParticlesFire.setInt("Pass", 1);
			//shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			//shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_1D, texId);
			//glEnable(GL_RASTERIZER_DISCARD);
			//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			//glBeginTransformFeedback(GL_POINTS);
			//glBindVertexArray(particleArray[1 - drawBuf]);
			//glVertexAttribDivisor(0, 0);
			//glVertexAttribDivisor(1, 0);
			//glVertexAttribDivisor(2, 0);
			//glDrawArrays(GL_POINTS, 0, nParticlesFire);
			//glEndTransformFeedback();
			//glDisable(GL_RASTERIZER_DISCARD);

			//shaderParticlesFire.setInt("Pass", 2);
			//glm::mat4 modelFireParticles = glm::mat4(1.0);
			//modelFireParticles = glm::translate(modelFireParticles, it->second.second);
			//modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
			//shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

			///*shaderParticlesFire.turnOn();*/
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			//glDepthMask(GL_FALSE);
			//glBindVertexArray(particleArray[drawBuf]);
			//glVertexAttribDivisor(0, 1);
			//glVertexAttribDivisor(1, 1);
			//glVertexAttribDivisor(2, 1);
			//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			//glBindVertexArray(0);
			//glDepthMask(GL_TRUE);
			//drawBuf = 1 - drawBuf;
			///*shaderParticlesFire.turnOff();*/

			// /****************************+
			//  * Open AL sound data
			//  */
			//source1Pos[0] = modelFireParticles[3].x;
			//source1Pos[1] = modelFireParticles[3].y;
			//source1Pos[2] = modelFireParticles[3].z;
			//alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
		}

	}
	glEnable(GL_CULL_FACE);
}

int main(int argc, char** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
