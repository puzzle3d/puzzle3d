#pragma once

#include "OpenUtility\Utility\3D\3D.h"
#include "Shader.h"
#include "Piece.h"
#include "Statistique.h"
#include "OpenGLContext.h"

class Monde {

private:
	SPivotf pivot;
	Piece* pieceSelectionnee;
	 
	vector<Piece> pieces;
	vector<float> yPieces;

	int index;
	bool cameraHasMoved;
	bool pieceHasMoved;
	bool pieceIsMoving;
	bool collisionDetected;
	bool hasWon;

	OpenGLContext* contexte;
	Statistique* stat;

	// Test
	GLuint VBAbuffer, VBObuffer;

public:

	Monde(OpenGLContext* contexte, Statistique* stat);
	~Monde();

	void rotation(float theta, XYZf axe);
	void rotationPiece(float theta, XYZf axe);
	void translatePiece(XYZf vecteur);
	void changeSelectedPiece(int sens);
	void aimanter();

	void generateXValues();
	int getIndexNext();
	inline int getIndex() { return this->index; };
	int getIndexBefore();
	void printX();

	void checkCollision();
	inline bool getCollision(){return this->collisionDetected;}
	inline bool getHasWon(){return this->hasWon;}

	void dessiner();

	inline void gagneTest(){this->hasWon = true;}
};