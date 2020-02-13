// Simplified Renderer application for GP course
// Features:
// Reduced OpenGL version from 4.5 to 3.3 to allow it to render in older laptops.
// Added Shapes library for rendering cubes, spheres and vectors.
// Added examples of matrix multiplication on Update.
// Added resize screen and keyboard callbacks.
// Added FPS camera functionality
// Update 2019/01 updated libraries and created project for VS2017 including directory dependant links to libraries.
// Update 2020/01 updated libraries for x64 and for VS2020, also adding MAC compiled Libraries.

// Suggestions or extra help please do email me at S.Padilla@hw.ac.uk

// Standard C++ libraries
#include <iostream>
#include <vector>
using namespace std;

// Helper graphic libraries
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "graphics.h"
#include "shapes.h"
#include "emitter.h"

// MAIN FUNCTIONS
void startup();
void init();
void refresh();
void updateCamera();
void updateSceneElements();
void renderScene();

// CALLBACK FUNCTIONS
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);

// VARIABLES
bool        quit = false;
float       deltaTime = 0.00f;    // Keep track of time per frame.
float       lastTime = 0.0f;    // variable to keep overall time.
bool        keyStatus[1024];    // Hold key status.
bool		mouseEnabled = true; // keep track of mouse toggle.

// MAIN GRAPHICS OBJECT
Graphics    myGraphics;        // Runing all the graphics in this object

// OBJECTS
const int L = 20; // Number of cubes per side in the border
const int	N_PARTICLES = 20;

Cube        pavement;
Sphere      cucumber;
Sphere		bouncingBall;
Cube		border[L*4-4];
Emitter		emitter;

// Some global variable to do the animation.
int t = 0;            // Global variable for frame count
glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f);			// gravity constant


int main()
{
	int errorGraphics = myGraphics.Init();			// Launch window and graphics context
	if (errorGraphics) return 0;					// Close if something went wrong...

	startup();										// Setup all necessary information for startup (aka. load texture, shaders, models, etc).



	// MAIN LOOP run until the window is closed
	while (!quit) {

		// Update the camera transform based on interactive inputs or by following a predifined path.
		updateCamera();

		// Update position, orientations and any other relevant visual state of any dynamic elements in the scene.
		updateSceneElements();

		// Render a still frame into an off-screen frame buffer known as the backbuffer.
		renderScene();

		// Swap the back buffer with the front buffer, making the most recently rendered image visible on-screen.
		glfwSwapBuffers(myGraphics.window);        // swap buffers (avoid flickering and tearing)

	}


	myGraphics.endProgram();            // Close and clean everything up...

   // cout << "\nPress any key to continue...\n";
   // cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void startup() {
	// Keep track of the running time
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime;                        // start delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	// Callback graphics and key update functions - declared in main to avoid scoping complexity.
	// More information here : https://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowSizeCallback(myGraphics.window, onResizeCallback);            // Set callback for resize
	glfwSetKeyCallback(myGraphics.window, onKeyCallback);                    // Set Callback for keys
	glfwSetMouseButtonCallback(myGraphics.window, onMouseButtonCallback);    // Set callback for mouse click
	glfwSetCursorPosCallback(myGraphics.window, onMouseMoveCallback);        // Set callback for mouse move
	glfwSetScrollCallback(myGraphics.window, onMouseWheelCallback);            // Set callback for mouse wheel.

	// Calculate proj_matrix for the first time.
	myGraphics.aspect = (float)myGraphics.windowWidth / (float)myGraphics.windowHeight;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);

	// Load Geometry examples
	cucumber.Load();
	cucumber.boundingBox.Load();
	cucumber.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);    // You can change the shape fill colour, line colour or linewidth

	bouncingBall.Load();
	bouncingBall.boundingBox.Load();
	bouncingBall.fillColor = glm::vec4(255.0f / 255.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f);

	pavement.Load();
	pavement.boundingBox.Load();
	pavement.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	pavement.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

	for (int i = 0; i < L * 4 - 4; i++) {
		Cube cube;
		cube.Load();
		cube.boundingBox.Load();
		border[i] = cube;
	}

	emitter = Emitter(N_PARTICLES);

	// Optimised Graphics
	myGraphics.SetOptimisations();        // Cull and depth testing
}

void init() {
	pavement.mass = INFINITY;
	pavement.Scale(myGraphics, glm::vec3(1000.0f, 0.001f, 1000.0f));

	cucumber.mass = 1.0f;
	cucumber.Translate(myGraphics, glm::vec3(-3.0f, 2.5f, 0.0f));
	cucumber.Translate(myGraphics, glm::vec3(0.0f, 2.5f, 4.0f));
	cucumber.Rotate(myGraphics, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cucumber.Scale(myGraphics, glm::vec3(4.0f, 2.0f, 1.0f));

	bouncingBall.mass = 1.0f;
	bouncingBall.Translate(myGraphics, glm::vec3(-2.0f, 3.0f, 1.0f));
	bouncingBall.bouncer = true;

	int gap = 1;
	for (int i = 0; i < L * 4 - 4; i++) {
		border[i].mass = INFINITY;
		// Bottom border
		if (i < L) {
			border[i].Translate(myGraphics, glm::vec3(0.0f + gap * i, 0.5f, 0.0f));
			border[i].fillColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		// Left border
		else if (i < L * 2 - 2) {
			border[i].Translate(myGraphics, glm::vec3(0.0f + (L - 1) * gap, 0.5f, 0.0f + gap * (i - L + 1)));
			border[i].fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		// Top border
		else if (i < L * 3 - 2) {
			border[i].Translate(myGraphics, glm::vec3(0.0f + gap * (i - L * 2 + 2), 0.5f, 0.0f + gap * (L - 1)));
			border[i].fillColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		// Right border
		else {
			border[i].Translate(myGraphics, glm::vec3(0.0f, 0.5f, 0.0f + gap * (i - L * 3 + 3)));
			border[i].fillColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		}
	}
	emitter.Init(myGraphics, glm::vec3(0.0f, 2.0f, 0.0f));
}

void refresh() {
	pavement.Refresh(myGraphics);
	cucumber.Refresh(myGraphics);
	bouncingBall.Refresh(myGraphics);
	for (int i = 0; i < size(border); i++) {
		border[i].Refresh(myGraphics);
	}
	emitter.Refresh(myGraphics);
}

void updateCamera() {

	// calculate movement for FPS camera
	GLfloat xoffset = myGraphics.mouseX - myGraphics.cameraLastX;
	GLfloat yoffset = myGraphics.cameraLastY - myGraphics.mouseY;    // Reversed mouse movement
	myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX;
	myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	myGraphics.cameraYaw += xoffset;
	myGraphics.cameraPitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (myGraphics.cameraPitch > 89.0f) myGraphics.cameraPitch = 89.0f;
	if (myGraphics.cameraPitch < -89.0f) myGraphics.cameraPitch = -89.0f;

	// Calculating FPS camera movement (See 'Additional Reading: Yaw and Pitch to Vector Calculations' in VISION)
	glm::vec3 front;
	front.x = cos(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));
	front.y = sin(glm::radians(myGraphics.cameraPitch));
	front.z = sin(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));

	myGraphics.cameraFront = glm::normalize(front);

	// Update movement using the keys
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) myGraphics.cameraPosition += cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_S]) myGraphics.cameraPosition -= cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_A]) myGraphics.cameraPosition -= glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) myGraphics.cameraPosition += glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;

	// IMPORTANT PART
	// Calculate my view matrix using the lookAt helper function
	if (mouseEnabled) {
		myGraphics.viewMatrix = glm::lookAt(myGraphics.cameraPosition,			// eye
			myGraphics.cameraPosition + myGraphics.cameraFront,					// centre
			myGraphics.cameraUp);												// up
	}
}

void updateSceneElements() {

	glfwPollEvents();                                // poll callbacks

	// Calculate frame time/period -- used for all (physics, animation, logic, etc).
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime - lastTime;                // Calculate delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	// Do not forget your ( T * R * S ) http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/

	// MY OBJECTS
	if (t == 0) {
		init();
	}
	
	else {
		// If an object is not on the floor fall
		if (!cucumber.CheckCollision(pavement)) {
			cucumber.Gravity();
			cucumber.Translate(myGraphics, cucumber.velocity);
		}
		else {
			cucumber.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	if (t % 150 == 0) {
		emitter.Shoot(myGraphics);
	}
	
	refresh();

	t++; // increment movement variable

	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.

}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();

	// Draw objects in screen
	if (pavement.visible) {
		pavement.Draw();
		pavement.boundingBox.Draw();
	}
	if (cucumber.visible) {
		cucumber.Draw();
		cucumber.boundingBox.Draw();
	}
	if (bouncingBall.visible) {
		bouncingBall.Draw();
		bouncingBall.boundingBox.Draw();
	}
	for (int i = 0; i < L * 4 - 4; i++) {
		if (border[i].visible) {
			border[i].Draw();
			border[i].boundingBox.Draw();
		}
	}
	emitter.Draw();
}


// CallBack functions low level functionality.
void onResizeCallback(GLFWwindow* window, int w, int h) {    // call everytime the window is resized
	//myGraphics.windowWidth = w;
	//myGraphics.windowHeight = h;

	glfwGetFramebufferSize(window, &myGraphics.windowWidth, &myGraphics.windowHeight);

	myGraphics.aspect = (float)w / (float)h;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { // called everytime a key is pressed
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	// toggle showing mouse.
	if (keyStatus[GLFW_KEY_M]) {
		mouseEnabled = !mouseEnabled;
		myGraphics.ToggleMouse();
	}
	// If exit key pressed.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	myGraphics.mouseX = mouseX;
	myGraphics.mouseY = mouseY;

	// helper variables for FPS camera
	if (myGraphics.cameraFirstMouse) {
		myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX; myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY; myGraphics.cameraFirstMouse = false;
	}
}

void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}
