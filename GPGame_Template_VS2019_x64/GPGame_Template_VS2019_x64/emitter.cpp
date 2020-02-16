#include "emitter.h"

const float PI = 3.1415927f;
// (1.0, 200) for (0.3 dimension) -- (0.45, 100) for (0.8 dimension)
const float MAGNITUDE = 0.45f;
const float TTL = 100;

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
			particles[i].Gravity();
			particles[i].Translate(graphics, particles[i].velocity);
			particles[i].ttl -= 1;
		}

		this->particles[i].Refresh(graphics);
	}
}

void Emitter::Shoot(Graphics graphics) {
	for (int i = 0; i < n_particles; i++) {
		float alfa = (i * 360.0f / n_particles) * PI / 180.0f; // Converted the angle in Radians
		particles[i].visible = true;
		particles[i].position_memory = placeholder.position_memory;
		particles[i].Scale(graphics, glm::vec3(0.8f, 0.8f, 0.8f));
		particles[i].velocity = glm::vec3(MAGNITUDE * cos(alfa), MAGNITUDE * 1.0f, MAGNITUDE * sin(alfa));
		particles[i].ttl = TTL;
	}
}