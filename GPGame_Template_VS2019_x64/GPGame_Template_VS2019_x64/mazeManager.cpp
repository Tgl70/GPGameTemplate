#include "mazeManager.h"
#include <time.h>
#include "maze.h"

using namespace std;

const int GAP = 1;
const pair<int, int> moves[4] = { make_pair(0, 1), make_pair(0, -1), make_pair(1, 0), make_pair(-1, 0) };

Position::Position() {

}

Position::Position(int i, int j, int cost_g, int cost_h, Position* parent) {
	this->i = i;
	this->j = j;
	this->cost_g = cost_g;
	this->cost_h = cost_h;
	if (parent != NULL) {
		//this->parent = new Position((*parent).i, (*parent).j, (*parent).cost_g, (*parent).cost_h, p);
		this->parent = parent;
	}
	else {
		this->parent = NULL;
	}
}

bool Position::operator==(Position p) {
	return this->i == p.i && this->j == p.j;
}

MazeManager::MazeManager() {

}

MazeManager::MazeManager(Graphics graphics, int side_lenght) {
	this->side_lenght = side_lenght;
	this->V = side_lenght * side_lenght;

	Cylinder agent;
	agent.Load();
	agent.boundingBox.Load();
	agent.mass = 5.0f;
	agent.fillColor = glm::vec4(186.0f / 255.0f, 85.0f / 255.0f, 211.0f / 255.0f, 1.0f);

	this->agent = agent;
	this->agent.visible = false;

	Cube target;
	target.Load();
	target.boundingBox.Load();
	target.fillColor = glm::vec4(186.0f / 255.0f, 85.0f / 255.0f, 211.0f / 255.0f, 1.0f);

	this->target = target;
	this->target.visible = false;

	Emitter e = Emitter(10);
	e.Init(graphics, glm::vec3(side_lenght - 1 - GAP, 2.0f, side_lenght - 1 - GAP));

	this->e = e;
	this->e.placeholder.visible = false;

	for (int i = 0; i < this->side_lenght * 4 - 4; i++) {
		Cube cube;
		cube.Load();
		cube.boundingBox.Load();
		cube.mass = INFINITY;
		this->walls.push_back(cube);
	}
}

MazeManager::~MazeManager() {

}

void MazeManager::Init(Graphics graphics) {
	for (int i = 0; i < this->side_lenght * 4 - 4; i++) {
		// Bottom walls
		if (i < this->side_lenght) {
			walls[i].Translate(graphics, glm::vec3(0.0f + GAP * i, 1.0f, 0.0f));
			walls[i].fillColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		// Left walls
		else if (i < this->side_lenght * 2 - 2) {
			walls[i].Translate(graphics, glm::vec3(0.0f + (this->side_lenght - 1) * GAP, 1.0f, 0.0f + GAP * (i - this->side_lenght + 1)));
			walls[i].fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		// Top walls
		else if (i < this->side_lenght * 3 - 2) {
			walls[i].Translate(graphics, glm::vec3(0.0f + GAP * (i - this->side_lenght * 2 + 2), 1.0f, 0.0f + GAP * (this->side_lenght - 1)));
			walls[i].fillColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		// Right walls
		else {
			walls[i].Translate(graphics, glm::vec3(0.0f, 1.0f, 0.0f + GAP * (i - this->side_lenght * 3 + 3)));
			walls[i].fillColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		}
	}
}

void MazeManager::Refresh(Graphics graphics) {
	if (!this->maze_path.empty()) {
		//Uses and pops the last element of the vector
		agent.Translate(graphics, this->maze_path.back());
		this->maze_path.pop_back();
	}
	if (agent.position_memory[3][0] == target.position_memory[3][0] && agent.position_memory[3][2] == target.position_memory[3][2]) {
		e.Shoot(graphics);
	}
	agent.Refresh(graphics);
	target.Refresh(graphics);
	for (int i = 0; i < this->side_lenght * 4 - 4; i++) {
		walls[i].Refresh(graphics);
	}
	for (int i = 0; i < obstacles.size(); i++) {
		obstacles[i].Refresh(graphics);
	}
	this->e.Refresh(graphics);
}

void MazeManager::Draw() {
	if (agent.visible) {
		agent.Draw();
		//agent.boundingBox.Draw();
	}
	if (target.visible) {
		target.Draw();
		target.boundingBox.Draw();
	}
	for (int i = 0; i < this->side_lenght * 4 - 4; i++) {
		if (walls[i].visible) {
			walls[i].Draw();
			walls[i].boundingBox.Draw();
		}
	}
	for (int i = 0; i < obstacles.size(); i++) {
		if (obstacles[i].visible) {
			obstacles[i].Draw();
			obstacles[i].boundingBox.Draw();
		}
	}
	e.Draw();
}

void MazeManager::GenerateMaze(Graphics graphics) {
	obstacles.clear();
	Maze m;
	srand(time(0));
	vector<int> row(side_lenght - 2);
	vector<vector<int>> map;
	for (int i = 0; i < side_lenght - 2; ++i)
	{
		map.push_back(row);
	}
	m.generate(map);

	map[0][0] = 0;
	map[side_lenght - 3][side_lenght - 3] = 0;

	for (int i = 0; i < side_lenght - 2; ++i) {
		for (int j = 0; j < side_lenght - 2; ++j) {
			if (map[i][j] == 1) {
				Cube obstacle;
				obstacle.Load();
				obstacle.boundingBox.Load();
				obstacle.mass = INFINITY;
				obstacle.Translate(graphics, glm::vec3(i + GAP, 1.0f, j + GAP));
				obstacle.fillColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				this->obstacles.push_back(obstacle);
			}
		}
	}

	glm::mat4 agent_pos =
		glm::translate(glm::vec3(GAP, 3.0f, GAP)) *
		glm::scale(glm::vec3(0.5f, 1.0f, 0.5f)) *
		glm::mat4(1.0f);
	agent.position_memory = agent_pos;
	agent.visible = true;

	glm::mat4 target_pos =
		glm::translate(glm::vec3(side_lenght - 1 - GAP, 0.5f, side_lenght - 1 - GAP)) *
		glm::scale(glm::vec3(1.0f, 0.01f, 1.0f)) *
		glm::mat4(1.0f);
	target.position_memory = target_pos;
	target.visible = true;

	this->agent_position = Position(0, 0, 0, 0, NULL);
	this->map = map;
	this->target_position = Position(18, 18, 0, 0, NULL);
	this->maze_path.clear();
}

void MazeManager::SolveMaze() {
	vector<Position> a_star = A_Star();
	int i = this->agent_position.i;
	int j = this->agent_position.j;

	for (int k = 0; k < a_star.size(); ++k) {
		int delta_i = a_star[k].i - i;
		int delta_j = a_star[k].j - j;

		this->maze_path.push_back(glm::vec3(delta_i, 0.0f, delta_j));
		this->maze_path.push_back(glm::vec3(delta_i, 0.0f, delta_j));

		i = i + delta_i;
		j = j + delta_j;
	}
	//Reversing this because in the Refresh we take from the last element
	reverse(this->maze_path.begin(), this->maze_path.end());
}

vector<Position> MazeManager::A_Star() {
	vector<Position> closed;
	vector<Position> fringe;
	vector<Position> path;

	bool found = false;
	fringe.push_back(this->agent_position);

	while (!found && fringe.size() > 0) {
		Position* p = new Position(-1, -1, 0, 0, NULL);
		for (auto f : fringe) {
			if (((f.cost_g + f.cost_h) < ((*p).cost_g + (*p).cost_h)) || (*p).i < 0) {
				p->i = f.i;
				p->j = f.j;
				p->cost_g = f.cost_g;
				p->cost_h = f.cost_h;
				p->parent = f.parent;
			}
		}
		fringe.erase(remove(fringe.begin(), fringe.end(), *p), fringe.end()); // remove element from fringe

		found = Goal(*p);
		closed.push_back(*p);
		if (found) {
			Position* current = p;
			while (current != NULL) {
				path.push_back(*current);
				current = current->parent;
			}
			reverse(path.begin(), path.end());
			return path;
		}
		vector<Position> children;
		for (int i = 0; i < 4; ++i) {
			Position child = Position(p->i + moves[i].first, p->j + moves[i].second, 0, 0, p);
			if (child.i >= 0 && child.j >= 0 && child.i < side_lenght - 2 && child.j < side_lenght - 2) {
				if (map[child.i][child.j] == 0) {
					children.push_back(child);
				}
			}
		}
		for (int i = 0; i < children.size(); ++i) {

			//Check if child is not already in the closed list
			bool cl = false;
			for (int j = 0; j < closed.size(); ++j) {
				if (children[i] == closed[j]) {
					cl = true;
				}
			}
			if (!cl) {
				children[i].cost_g = children[i].parent->cost_g + 1;
				children[i].cost_h = Manhattan(children[i], this->target_position);

				//If the child is not already in the fringe I put it in the fringe
				bool already_in_fringe = false;
				for (int j = 0; j < fringe.size(); ++j) {
					if (children[i] == fringe[j] && children[i].cost_g > fringe[j].cost_g) {
						already_in_fringe = true;
					}
				}
				if (!already_in_fringe) {
					fringe.push_back(children[i]);
				}
			}
		}
	}
}

int MazeManager::Manhattan(Position p, Position g) {
	return g.i - p.i + g.j - p.j;
}

bool MazeManager::isClosed(vector<Position> closed, Position p) {
	for (int i = 0; i < closed.size(); ++i) {
		if (closed[i] == p) return true;
	}
	return false;
}

bool MazeManager::Goal(Position p) {
	return p.i == (side_lenght - 3) && p.j == (side_lenght - 3);
}

//https://medium.com/@nicholas.w.swift/easy-a-star-pathfinding-7e6689c7f7b2
