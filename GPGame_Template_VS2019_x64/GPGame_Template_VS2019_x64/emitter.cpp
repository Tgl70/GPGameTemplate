#include "emitter.h"
#include <time.h>

const float PI = 3.1415927f;
// (1.0, 200) for (0.3 dimension) -- (0.45, 100) for (0.8 dimension)
const float MAGNITUDE = 0.3f;
const float TTL = 200;

Emitter::Emitter() {

}

Emitter::Emitter(int n_particles) {
	this->n_particles = n_particles;

	Cube cube;
	cube.Load();
	cube.boundingBox.Load();
	cube.fillColor = glm::vec4(186.0f / 255.0f, 85.0f / 255.0f, 211.0f / 255.0f, 1.0f);

	this->placeholder = cube;

	for (int i = 0; i < n_particles; i++) {
		Particle particle;
		particle.Load();
		particle.boundingBox.Load();
		particle.mass = 0.1f;
		particle.fillColor = glm::vec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f);
		this->particles.push_back(particle);
	}
}

Emitter::~Emitter() {

}

void Emitter::Init(Graphics graphics, glm::vec3 position) {
	this->placeholder.mass = INFINITY;
	this->placeholder.Translate(graphics, position);
	this->placeholder.Scale(graphics, glm::vec3(0.04f, 0.04f, 0.04f));
}

void Emitter::Draw() {
	if (placeholder.visible) {
		placeholder.Draw();
		placeholder.boundingBox.Draw();
	}
	for (int i = 0; i < n_particles; i++) {
		if (particles[i].visible) {
			particles[i].Draw();
			//particles[i].boundingBox.Draw();
		}
	}
}

void Emitter::Refresh(Graphics graphics) {
	placeholder.Refresh(graphics);
	for (int i = 0; i < n_particles; i++) {
		if (particles[i].ttl == 0) {
			particles[i].visible = false;
		}
		else {
			// This is to prevent that the particles block each other inside the emitter
			if (particles[i].ttl == TTL - 5) {
				particles[i].visible = true;
			}
			particles[i].Gravity();
			particles[i].Translate(graphics, particles[i].velocity);
			particles[i].ttl -= 1;
		}

		this->particles[i].Refresh(graphics);
	}
}

void Emitter::Shoot(Graphics graphics) {
	srand((unsigned) time(0));
	for (int i = 0; i < n_particles; i++) {
		float alfa = (i * 360.0f / n_particles) * PI / 180.0f; // Converted the angle in Radians
		float r = (float)(rand()) / ((float)(RAND_MAX));
		if (particles[i].ttl == 0 && r > 0.75f) {
			particles[i].Shoot(graphics, placeholder.position_memory, alfa, MAGNITUDE, TTL);
		}
	}
}