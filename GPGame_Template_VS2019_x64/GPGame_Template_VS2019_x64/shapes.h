#pragma once

#include <iostream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "graphics.h"
#include <glm/gtx/transform.hpp>

class Shapes {

public:
	Shapes();
	~Shapes();

	void Load();
	void Draw();
	void checkErrorShader(GLuint shader);

	vector<GLfloat> vertexPositions;

	GLuint          program;
	GLuint          vao;
	GLuint          buffer;
	GLint           mv_location;
	GLint           proj_location;
	GLint           color_location;
	glm::mat4		proj_matrix = glm::mat4(1.0f);
	glm::mat4		mv_matrix = glm::mat4(1.0f);

	glm::vec4		fillColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
	glm::vec4		lineColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
	float			lineWidth = 2.0f;

protected:
	string rawData;			// Import obj file from Blender (note: no textures or UVs).
	void LoadObj();
};

class BoundingBox : public Shapes {
public:
	BoundingBox();
	~BoundingBox();
};

class Collidable : public Shapes {
public:
	Collidable();
	//TRANSFORM
	void Translate(Graphics graphics, glm::vec3 t);
	void Rotate(Graphics graphics, float r, glm::vec3 t);
	void Scale(Graphics graphics, glm::vec3 t);
	void Refresh(Graphics graphics);
	
	//BOUNDING BOXES
	void CalculateMinMax(Graphics graphics);
	void CalculateBoundingBox(Graphics graphics);
	
	//COLLISIONS
	bool CheckCollision(Collidable c);
	//void Collide();

	//PHYSICS
	void Accelerate(float a, float t);

	BoundingBox boundingBox;
	glm::mat4 position_memory;
	glm::vec3 min;
	glm::vec3 max;

	float mass;
	glm::vec3 velocity;
};

class Cube : public Collidable {
public:
	Cube();
	Cube(float mass);
	~Cube();
};

class Sphere : public Collidable {
public:
	Sphere();
	Sphere(float mass);
	~Sphere();
};

class Arrow : public Collidable {
public:
	Arrow();
	~Arrow();
};

class Cylinder : public Collidable {
public:
	Cylinder();
	Cylinder(float mass);
	~Cylinder();
};

class Line : public Collidable {
public:
	Line();
	~Line();
};
