#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "graphics.h"
#include "shapes.h"
#include <glm/gtx/transform.hpp>
#include <cmath>
using namespace std;

// class to represent the particle emitter objects
class Emitter {
public:
	Emitter();
	Emitter(int n_particles);
	~Emitter();
	void Init(Graphics graphics, glm::vec3 position);
	void Draw();
	void Refresh(Graphics graphics);
	void Shoot(Graphics graphics); // Function to shoot the particles
	Cube placeholder; // The source of our particles
	vector<Particle> particles; // Particles under the control of this emitter

private:
	int n_particles; // number of particles under the control of this emitter
};

