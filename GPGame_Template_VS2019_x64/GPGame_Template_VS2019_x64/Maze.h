#pragma once
#include "string"
#include<vector>
#include "unordered_map"
#include "queue"
#include "cstdlib"
#include "ctime"
#include <iostream>
#include "algorithm"
#include "stack"

using namespace std;

class Maze {
public:
	void generate(vector<vector<int>>& map);
	void showMaze(vector<vector<int>>& map);
private:
	void _generate(vector<vector<int>>& map, int i, int j);
	int countVisitedNeighbor(vector<vector<int>>& map, int i, int j);
	void shuffle(int a[], int n);
	void swap(int& a, int& b);
};
