#include "flockManager.h"
#include <time.h>

const float PI = 3.1415927f;
const float STARTING_OFFSET = 5.0f; // starting point of the big cube containing the flock demo
const int BOID_SPACE = 3; // lenght of boids' spawning parallelepiped
const int OBSTACLES_SPACE = 8; // lenght of obstacles' spawning parallelepiped
const float TARGET_OFFSET = 3.0f; // offset of the target with respect to the end of the obstacle space
const int N_NEIGHBOURS = 3; // number of neighbours each boid should look at when adjusting his velocity

FlockManager::FlockManager() {

}

// create all the elements used in the flock and make them not visible
FlockManager::FlockManager(Graphics graphics, int flock_size, int obstacles_size) {
	this->flock_size = flock_size;
	this->obstacles_size = obstacles_size;

	Cylinder cylinder;
	cylinder.Load();
	cylinder.boundingBox.Load();
	cylinder.fillColor = glm::vec4(186.0f / 255.0f, 85.0f / 255.0f, 211.0f / 255.0f, 1.0f);

	this->target = cylinder;
	this->target.visible = false;

	for (int i = 0; i < obstacles_size; i++) {
		Cube obstacle;
		obstacle.Load();
		obstacle.boundingBox.Load();
		obstacle.mass = INFINITY;
		obstacle.visible = false;
		obstacle.fillColor = glm::vec4(0.0f, 0.2f, 1.0f, 1.0f);
		this->obstacles.push_back(obstacle);
	}

	for (int i = 0; i < flock_size; i++) {
		Boid boid;
		boid.Load();
		boid.boundingBox.Load();
		boid.mass = 0.2f;
		boid.visible = false;
		boid.fillColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		this->flock.push_back(boid);
	}
}

FlockManager::~FlockManager() {

}

void FlockManager::Draw() {
	if (target.visible) {
		target.Draw();
		target.boundingBox.Draw();
	}
	for (int i = 0; i < obstacles_size; i++) {
		if (obstacles[i].visible) {
			obstacles[i].Draw();
			obstacles[i].boundingBox.Draw();
		}
	}
	for (int i = 0; i < flock_size; i++) {
		if (flock[i].visible) {
			flock[i].Draw();
			flock[i].boundingBox.Draw();
		}
	}
}

// move all the elements to their position (randomly generated inside constraints) 
//and make them visible and able to interact with the world (gravity and collisions)
void FlockManager::GenerateFlock(Graphics graphics) {
	srand((unsigned)(time(0)));
	// Generate flock
	for (int i = 0; i < flock_size; i++) {
		float rx = (float) (rand()) / ((float) (RAND_MAX / BOID_SPACE));
		float ry = (float)(rand()) / ((float)(RAND_MAX / BOID_SPACE));
		float rz = (float)(rand()) / ((float)(RAND_MAX / BOID_SPACE));
		flock[i].visible = true;
		glm::mat4 boid_matrix =
			glm::translate(glm::vec3(
				-STARTING_OFFSET - rx,
				STARTING_OFFSET + ry,
				STARTING_OFFSET + rz)) *
			glm::scale(glm::vec3(0.25f, 0.1f, 0.1f)) *
			glm::mat4(1.0f);
		flock[i].velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		flock[i].position_memory = boid_matrix;
	}
	// Generate obstacles
	for (int i = 0; i < obstacles_size; i++) {
		float rx = (float)(rand()) / ((float)(RAND_MAX / OBSTACLES_SPACE));
		float ry = (float)(rand()) / ((float)(RAND_MAX / (OBSTACLES_SPACE / 2)));
		float rz = (float)(rand()) / ((float)(RAND_MAX / (OBSTACLES_SPACE / 2)));
		obstacles[i].visible = true;
		glm::mat4 obstacles_matrix =
			// Add BOID_SPACE to have relative position between the flock and the obstacles spaces
			glm::translate(glm::vec3(
				-STARTING_OFFSET - BOID_SPACE - rx,
				STARTING_OFFSET + BOID_SPACE/4 + ry,
				STARTING_OFFSET + BOID_SPACE/4 + rz)) *
			glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
			glm::mat4(1.0f);
		obstacles[i].position_memory = obstacles_matrix;
	}
	// Generate target
	target.visible = true;
	glm::mat4 target_matrix =
		glm::translate(glm::vec3(
			-STARTING_OFFSET - BOID_SPACE - OBSTACLES_SPACE - TARGET_OFFSET,
			STARTING_OFFSET + BOID_SPACE/2,
			STARTING_OFFSET + BOID_SPACE/2)) *
		glm::rotate(PI/2, glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::vec3(1.0f, 0.2f, 1.0f)) *
		glm::mat4(1.0f);
	target.position_memory = target_matrix;
}

void FlockManager::Refresh(Graphics graphics) {
	float velocity = 1.0f;
	float previous_weight = velocity * 0.08f;
	float target_weight = velocity * 0.15f;
	float center_weight = velocity * 0.0001f;
	float safe_weight = velocity * 0.1f;
	float velocity_weight = velocity * 0.0004f;
	float obstacles_weight = velocity * 1.0f;

	target.Refresh(graphics);
	for (int i = 0; i < obstacles_size; i++) {
		obstacles[i].Refresh(graphics);
	}
	for (int i = 0; i < flock_size; i++) {
		// Rules

		// 1: Go towards the target
		flock[i].velocity = previous_weight * flock[i].velocity - ((flock[i].GetPosition() - GetTargetPosition()) * target_weight);

		// 2: Go towards the center of mass of neighbour boids
		flock[i].velocity = flock[i].velocity + GetCenterOfMass(i) * center_weight;

		// 3: Don't go too close to other boids
		flock[i].velocity = flock[i].velocity + GetSafeDistance(i) * safe_weight;

		// 4: Try to match velocity of neighbour boids
		flock[i].velocity = flock[i].velocity + GetAverageVelocity(i) * velocity_weight;

		// 5: Avoid obstacles
		flock[i].velocity = flock[i].velocity + GetObstaclesDistance(i) * obstacles_weight;

		flock[i].Refresh(graphics);
	}
}

glm::vec3 FlockManager::GetObstaclesDistance(int boidIndex) {
	float safe_distance = 2.0f;
	float boost = 1.2f;
	glm::vec3 velocity_update = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 delta = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 boidPostion = flock[boidIndex].GetPosition();

	for (int i = 0; i < obstacles_size; i++) {
		glm::vec3 i_position = glm::vec3(obstacles[i].position_memory[3][0], obstacles[i].position_memory[3][1], obstacles[i].position_memory[3][2]);
		float dist = glm::distance(i_position, boidPostion);
		if (dist < safe_distance) {
			delta = (boidPostion - i_position);

			if (delta[0] > delta[1] && delta[0] > delta[2]) {
				delta[1] = delta[1] * boost;
				delta[2] = delta[2] * boost;
			}
			else if (delta[1] > delta[2]) {
				delta[0] = delta[0] * boost;
				delta[2] = delta[2] * boost;
			}
			else {
				delta[0] = delta[0] * boost;
				delta[1] = delta[1] * boost;
			}

			delta = delta * pow((safe_distance - dist), 3);

			velocity_update = velocity_update + delta;
		}
	}
	return velocity_update;
}

glm::vec3 FlockManager::GetAverageVelocity(int boidIndex) {
	//distances
	float d1 = INFINITY;
	float d2 = INFINITY;
	float d3 = INFINITY;

	//boids
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 boidPostion = flock[boidIndex].GetPosition();

	for (int i = 0; i < flock_size; i++) {
		if (i != boidIndex) {
			glm::vec3 i_postion = flock[i].GetPosition();
			float dist = glm::distance(i_postion, boidPostion);
			if (dist < d1) {
				d1 = dist;
				v1 = flock[i].velocity;
			}
			else if (dist < d2) {
				d2 = dist;
				v2 = flock[i].velocity;
			}
			else if (dist < d3) {
				d3 = dist;
				v3 = flock[i].velocity;
			}
		}
	}
	return glm::vec3(
		(v1[0] + v2[0] + v3[0]) / 3,
		(v1[1] + v2[1] + v3[1]) / 3,
		(v1[2] + v2[2] + v3[2]) / 3
	);
}

glm::vec3 FlockManager::GetSafeDistance(int boidIndex) {
	float safe_distance = 0.3f;
	glm::vec3 velocity_update = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 boidPostion = flock[boidIndex].GetPosition();

	for (int i = 0; i < flock_size; i++) {
		if (i != boidIndex) {
			glm::vec3 i_position = flock[i].GetPosition();
			float dist = glm::distance(i_position, boidPostion);
			if (dist < safe_distance) {
				velocity_update = velocity_update + (boidPostion - i_position);
			}
		}
	}
	return velocity_update;
}

glm::vec3 FlockManager::GetCenterOfMass(int boidIndex) {
	//distances
	float d1 = INFINITY;
	float d2 = INFINITY;
	float d3 = INFINITY;
	
	//boids
	glm::vec3 b1;
	glm::vec3 b2;
	glm::vec3 b3;
	glm::vec3 boidPostion = flock[boidIndex].GetPosition();
	
	for (int i = 0; i < flock_size; i++) {
		if (i != boidIndex) {
			glm::vec3 i_postion = flock[i].GetPosition();
			float dist = glm::distance(i_postion, boidPostion);
			if (dist < d1) {
				d1 = dist;
				b1 = i_postion;
			}
			else if (dist < d2) {
				d2 = dist;
				b2 = i_postion;
			}
			else if (dist < d3) {
				d3 = dist;
				b3 = i_postion;
			}
		}
	}
	return glm::vec3(
		(b1[0] + b2[0] + b3[0]) / 3,
		(b1[1] + b2[1] + b3[1]) / 3,
		(b1[2] + b2[2] + b3[2]) / 3
	);
}

glm::vec3 FlockManager::GetTargetPosition() {
	return glm::vec3(target.position_memory[3][0], target.position_memory[3][1], target.position_memory[3][2]);
}