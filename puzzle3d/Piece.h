#pragma once
#include <vector>
#include "OpenGLContext.h"

using namespace std;

class Piece
{
private:
	OpenGLContext* context;
	SPivotf pivot;
	vector<XYZf> cubes;
	XYZf selectedColor;
	XYZf notSelectedColor;
	bool isPlacedVar;
	bool inCube;

public:
	Piece(OpenGLContext* context, float x, float y, float z,float nscR, float nscG, float nscB);
	~Piece(void);

	SPivotf inline getPivot() { return this->pivot; }
	bool inline getInCube() { return this->inCube; }
	void inline setTrueToInCube() { this->inCube=true; }
	void inline changePosPivot(int x, int y, int z) { this->pivot.SetPos(x, y, z); }
	void inline changeAnglePivot(XYZf view, XYZf norm) { this->pivot.SetOrientation(
															view.x, view.y, view.z,
															norm.x, norm.y, norm.z);}

	void addElement(XYZf posRelative);
	void rotation(float theta, XYZf axe);
	void translate(XYZf vecteur);
	void dessiner(Piece* p);

	bool dansPerimetre(XYZf centre, int dist);
	vector<XYZf> posCubes();


	inline bool isPlaced(){return this->isPlacedVar;}
	inline void placePiece(){this->isPlacedVar = true;}
	inline void movePiece(){this->isPlacedVar = false;}


	inline bool operator==(Piece p) { return (this->selectedColor == p.selectedColor && this->notSelectedColor == p.notSelectedColor); }
};