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
#include "emitter.h"
using namespace std;

// class to represent the cells in the maze
class Position {
public:
	Position();
	Position(int i, int j, int cost_g, int cost_h, Position* parent);
	// https://docs.microsoft.com/it-it/cpp/error-messages/compiler-errors-2/compiler-error-c2676?view=vs-2019
	// http://courses.cms.caltech.edu/cs11/material/cpp/donnie/cpp-ops.html
	bool operator==(const Position p);
	int i;
	int j;
	int cost_g;
	int cost_h;
	Position* parent;
};

// Class to manage all the elements of the maze demo
class MazeManager {
public:
	MazeManager();
	MazeManager(Graphics graphics, int side_lenght);
	~MazeManager();
	void Draw();
	void Refresh(Graphics graphics);
	void Init(Graphics graphics);
	void GenerateMaze(Graphics graphics);
	void SolveMaze();
	//A* functions
	vector<Position> A_Star();
	bool isClosed(vector<Position> closed, Position p);
	bool Goal(Position p);
	int Manhattan(Position p, Position g);

	// Maze components
	Cylinder agent;
	Cube target;
	vector<Cube> walls;
	vector<Cube> obstacles;
	Emitter e;

private:
	int side_lenght; // lenght of the side of the maze

	Position agent_position; 
	Position target_position;

	vector<vector<int>> map; // map of the maze
	vector<glm::vec3> maze_path; // path to solve the maze
};

