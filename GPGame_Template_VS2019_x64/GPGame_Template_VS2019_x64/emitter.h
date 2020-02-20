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

class Emitter {
public:
	Emitter();
	Emitter(int n_particles);
	~Emitter();
	void Init(Graphics graphics, glm::vec3 position);
	void Draw();
	void Refresh(Graphics graphics);
	void Shoot(Graphics graphics);
	Cube placeholder;
	vector<Particle> particles;

private:
	int n_particles;
};

