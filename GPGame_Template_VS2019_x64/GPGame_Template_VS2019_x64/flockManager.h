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

class FlockManager {
public:
	FlockManager();
	FlockManager(Graphics graphics, int flock_size, int obstacles_size);
	~FlockManager();
	void Draw();
	void Refresh(Graphics graphics);
	void GenerateFlock(Graphics graphics);
	Cylinder target;
	vector<Cube> obstacles;
	vector<Boid> flock;

private:
	glm::vec3 GetObstaclesDistance(int boidIndex);
	glm::vec3 GetAverageVelocity(int boidIndex);
	glm::vec3 GetSafeDistance(int boidIndex);
	glm::vec3 GetCenterOfMass(int boidIndex);
	glm::vec3 GetTargetPosition();
	int flock_size;
	int obstacles_size;
};

