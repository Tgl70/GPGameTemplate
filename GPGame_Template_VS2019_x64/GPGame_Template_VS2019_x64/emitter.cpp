#include "emitter.h"

const float PI = 3.1415927f;
const float MAGNITUDE = 1.0f;
const float TTL = 10;

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
		float alfa = (i * 360.0f / n_particles) * PI / 180.0f; // Converted the angle in Radians
		
		Particle particle(1.0f);
		particle.Load();
		particle.boundingBox.Load();
		particle.fillColor = glm::vec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f);
		particle.velocity = glm::vec3(MAGNITUDE * cos(alfa), MAGNITUDE * 0.0f, MAGNITUDE * sin(alfa));
		this->particles.push_back(particle);
	}
}

Emitter::~Emitter() {

}

void Emitter::Init(Graphics graphics, glm::vec3 position) {
	this->placeholder.Translate(graphics, position);
	this->placeholder.Scale(graphics, glm::vec3(0.05f, 0.05f, 0.05f));

	for (int i = 0; i < n_particles; i++) {
		this->particles[i].Translate(graphics, position);
		this->particles[i].Scale(graphics, glm::vec3(0.01f, 0.01f, 0.01f));
	}
}

void Emitter::Draw() {
	if (placeholder.visible) {
		placeholder.Draw();
		placeholder.boundingBox.Draw();
	}
	for (int i = 0; i < n_particles; i++) {
		if (particles[i].visible) {
			particles[i].Draw();
			particles[i].boundingBox.Draw();
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
			//particles[i].Gravity();
			particles[i].Translate(graphics, particles[i].velocity);
			particles[i].ttl -= 1;
		}

		this->particles[i].Refresh(graphics);
	}
}

void Emitter::Shoot() {
	for (int i = 0; i < n_particles; i++) {
		particles[i].visible = true;
		particles[i].position_memory = placeholder.position_memory;
		particles[i].ttl = TTL;
	}
}