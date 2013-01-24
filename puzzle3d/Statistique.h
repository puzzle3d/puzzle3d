#pragma once

#include <time.h>
#include <vector>
#include <Windows.h>

class intervalle {
	public:
		int begin;
		int end;
		int index_piece;
};

class Statistique {


private:
	int beginTimestamp;
	int endTimestamp;

	int lastMouseMove;

	int collision_number;
	int error_number;
	int mouse_move_number;

	std::vector<intervalle> observation;
	intervalle currentObsIntervalle;

public:
	Statistique();
	~Statistique();
	inline void saveBeginTime() { this->beginTimestamp = (int)GetTickCount(); }
	void endData();
	void addMouseMove();
	inline void addCollision() { this->collision_number++; }
	inline void addError() { this->error_number++; }
	void addMovementTimestamp(int milliseconde, int index);
	void printObservation();
	void changePiece(int t);
};