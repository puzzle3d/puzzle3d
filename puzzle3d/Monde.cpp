#include "Monde.h"
#include "Statistique.h"
#include <list>
#include<math.h>

Monde::Monde(OpenGLContext* context, Statistique* stat){
	this->contexte = context;
	this->stat = stat;
	pivot = SPivotf(	XYZf(0.0, 0.0, 0.0),
						XYZf(1.0, 0.0, 0.0),
						XYZf(0.0, 0.0, 1.0));

	//Jaune
	Piece temp = Piece(context, 5.0f, 5.0f, 5.0f, 1.0f,1.0f,0.65f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(0.0,-1.0,0.0));
	temp.addElement(XYZf(0.0,0.0,1.0));
	this->pieces.push_back(temp);

	//Vert
	temp = Piece(context, -5.0f, -5.0f, -5.0f, 0.6f,1.0f,0.6f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(0.0,-1.0,0.0));
	temp.addElement(XYZf(0.0,1.0,1.0));
	this->pieces.push_back(temp);

	//Bleu
	temp = Piece(context, 5.0f, -5.0f, -5.0f, 0.72f,0.78f,1.0f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(0.0,-1.0,1.0));
	temp.addElement(XYZf(0.0,0.0,1.0));
	this->pieces.push_back(temp);

	//Rouge
	temp = Piece(context, -5.0f, 5.0f, -5.0f, 1.0f,0.68f,0.68f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(1.0,0.0,0.0));
	temp.addElement(XYZf(0.0,0.0,1.0));
	this->pieces.push_back(temp);


	//Rose
	temp = Piece(context, -5.0f, 5.0f, 5.0f, 1.0f,0.75f,1.0f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(1.0,0.0,1.0));
	temp.addElement(XYZf(0.0,0.0,1.0));
	this->pieces.push_back(temp);


	//Marron
	temp = Piece(context, 5.0f, 5.0f, -5.0f,1.0f,0.69f,0.38f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(1.0,0.0,0.0));
	temp.addElement(XYZf(0.0,1.0,-1.0));
	this->pieces.push_back(temp);


	//Cyan
	temp = Piece(context, -5.0f, -5.0f, 5.0f, 0.87f,1.0f,1.0f);
	temp.addElement(XYZf(0.0,1.0,0.0));
	temp.addElement(XYZf(1.0,0.0,0.0));
	this->pieces.push_back(temp);

	this->index = 0;
	this->pieceSelectionnee = &(this->pieces[this->index]);

	this->cameraHasMoved = false;
	this->pieceHasMoved = false;
	this->generateXValues();
	this->collisionDetected = false;
	this->hasWon = false;
}

Monde::~Monde(){
}

void Monde::rotation(float theta, XYZf axe){

	this->cameraHasMoved = true;

	// Normalisation du vecteur directeur de l'axe
	axe = axe / v3fModule(axe);

	float passage[16];
	XYZf axe2;
	Invert44f(passage, pivot.GetRotMatrix());
	Matrix44Mul31f(&axe2, passage, &axe);

	// Axes à modifier
	XYZf view = this->pivot.GetdView();
	XYZf normale = this->pivot.GetdNorm();

	// Rotation des axes
	v3fRotate(view, axe2, theta);
	v3fRotate(normale, axe2, theta);

	// Nouvelle orientation
	this->pivot.SetOrientation(view, normale);
}


void Monde::rotationPiece(float theta, XYZf axe){

	this->pieceHasMoved = true;
	this->pieceIsMoving = true;

	// Normalisation du vecteur directeur de l'axe
	axe = axe / v3fModule(axe);

	float passage[16];
	XYZf axe2;
	Invert44f(passage, pivot.GetRotMatrix());
	Matrix44Mul31f(&axe2, passage, &axe);

	pieceSelectionnee->rotation(theta, axe2);
}

void Monde::changeSelectedPiece(int sens){
	// -1 => vers la gauche
	// +1 => vers la droite

	if(sens == -1) this->index = this->getIndexBefore();
	else if (sens == 1) this->index = this->getIndexNext();
	
	//this->printY();

	this->pieceSelectionnee = &(this->pieces[this->index]);
}

void Monde::printX() {
	printf("\n\n   Jaune : %f\n   Vert :  %f\n   Bleu :  %f\n   Rouge :  %f\n   Rose :  %f\n   Marron :  %f\n   Cyan :  %f\n", 
		this->yPieces[0], this->yPieces[1], this->yPieces[2], this->yPieces[3], this->yPieces[4], this->yPieces[5], this->yPieces[6]);
}

void Monde::generateXValues() {
	int taille = this->pieces.size();
	this->yPieces.clear();

	for(int i=0; i < taille ; i++) {
		Piece p = this->pieces[i];
		XYZf res, pivotPiece = p.getPivot().GetPos();

		Matrix44Mul31f(&res, this->pivot.GetRotMatrix(), &pivotPiece);

		this->yPieces.push_back(res.x);
		this->cameraHasMoved = false;
		this->pieceHasMoved = false;
	}
}

int Monde::getIndexBefore() {
	
	if(this->pieceHasMoved || this->cameraHasMoved) this->generateXValues();
	
	int res = -1;
	int taille = this->yPieces.size();
	float y_for_index = this->yPieces[this->index];
	float y_to_compare = -FLT_MAX;

	//On y insère une copie des pièces où les pivots sont déjà multiplié
	for(int i = 0; i < taille; i++) {
		if (i != this->index) {
			if (this->yPieces[i] <= y_for_index && this->yPieces[i] > y_to_compare) {
				res = i;
				y_to_compare = this->yPieces[i];
			}
		}
	}

	if (res == -1) return this->index;

	return res;
}

int Monde::getIndexNext() {
	if(this->pieceHasMoved || this->cameraHasMoved) this->generateXValues();

	int res = -1;
	int taille = this->yPieces.size();
	float y_for_index = this->yPieces[this->index];
	float y_to_compare = FLT_MAX;

	//On y insère une copie des pièces où les pivots sont déjà multiplié
	for(int i = 0; i < taille; i++) {
		if (i != this->index) {
			if (this->yPieces[i] >= y_for_index && this->yPieces[i] < y_to_compare) {
				res = i;
				y_to_compare = this->yPieces[i];
			}
		}
	}

	if (res == -1) return this->index;

	return res;
}

void Monde::translatePiece(XYZf vecteur){

	this->pieceHasMoved = true;
	this->pieceIsMoving = true;

	float passage[16];
	XYZf vecteur2;
	Invert44f(passage, pivot.GetRotMatrix());
	Matrix44Mul31f(&vecteur2, passage, &vecteur);

	pieceSelectionnee->translate(vecteur2);
}

void Monde::dessiner() {

	glUniformMatrix4fv(contexte->attribToShader("pivot"), 1, false, this->pivot.GetRotMatrix());
	int taille = this->pieces.size();

	for(int i =0; i < taille; i++){
		pieces[i].dessiner(this->pieceSelectionnee);
	}

	float id[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glUniformMatrix4fv(contexte->attribToShader("translObjet"), 1, false, id);
	glUniformMatrix4fv(contexte->attribToShader("pivotObjet"), 1, false, id);
	contexte->drawLines(3);
}

void Monde::aimanter(){
	bool previous_in_cube = this->pieceSelectionnee->getInCube();

	if(pieceIsMoving) {
		if(this->pieceSelectionnee->dansPerimetre(this->pivot.GetPos(), 3)) {

			this->pieceSelectionnee->setTrueToInCube();

			XYZf vx(1.0, 0.0, 0.0), vy(0.0, 1.0, 0.0), vz(0.0, 0.0, 1.0);
			double scalvx = v3fScal(this->pieceSelectionnee->getPivot().GetdView(),vx);
			double scalvy = v3fScal(this->pieceSelectionnee->getPivot().GetdView(),vy);
			double scalvz = v3fScal(this->pieceSelectionnee->getPivot().GetdView(),vz);
			double scalnx = v3fScal(this->pieceSelectionnee->getPivot().GetdNorm(),vx);
			double scalny = v3fScal(this->pieceSelectionnee->getPivot().GetdNorm(),vy);
			double scalnz = v3fScal(this->pieceSelectionnee->getPivot().GetdNorm(),vz);
			XYZf nView, nNorm;

			double old_x, old_y, old_z;

		
			float seuil = sqrt(3.0)/2.0;

			if(abs(scalvx) > seuil || abs(scalvy) > seuil || abs(scalvz) > seuil){
				// Initialisation de nView et nNorm
				(scalvx<0)?nView = -vx:nView = vx;
				(scalnz<0)?nNorm = -vz:nNorm = vz;

				if(abs(scalvy) > abs(v3fScal(this->pieceSelectionnee->getPivot().GetdView(), nView))) {
					(scalvy<0)?nView = -vy:nView = vy;
				}
				if(abs(scalvz) > abs(v3fScal(this->pieceSelectionnee->getPivot().GetdView(), nView))) {
					(scalvz<0)?nView = -vz:nView = vz;
				}
				if(abs(scalny) > abs(v3fScal(this->pieceSelectionnee->getPivot().GetdNorm(), nNorm))) {
					if(nView != vy)
						(scalny<0)?nNorm = -vy:nNorm = vy;
				}
				if(abs(scalnx) > abs(v3fScal(this->pieceSelectionnee->getPivot().GetdNorm(), nNorm))) {
					if(nView != vx)
						(scalnx<0)?nNorm = -vx:nNorm = vx;
				}

				this->pieceSelectionnee->changePosPivot(floor(this->pieceSelectionnee->getPivot().GetPos().x/2+0.5)*2,
												floor(this->pieceSelectionnee->getPivot().GetPos().y/2+0.5)*2,
												floor(this->pieceSelectionnee->getPivot().GetPos().z/2+0.5)*2);

				this->pieceSelectionnee->changeAnglePivot(nView, nNorm);
				this->pieceSelectionnee->placePiece();
				this->checkCollision();
			}

			
		} else {
			if(previous_in_cube) {
				(*stat).addError();
			}
		}
	pieceIsMoving = false;
	}
}

void Monde::checkCollision(){
	int casesCubesOccupation[27];
	this->collisionDetected = false;
	bool collision = false;
	int occupants = 0;
	// Initialisation des cases à 0 occupants
	for(int i=0; i<27; i++)
		casesCubesOccupation[i] = 0;

	// On rempli notre cube en ajoutant chaque occupant
	for each(Piece piece in this->pieces){
		if(piece.isPlaced()){
			for each(XYZf cube in piece.posCubes()){
				int index = (floor(cube.x)+1)+3*(floor(cube.y)+1)+9*(floor(cube.z)+1);
				casesCubesOccupation[index]++;
				occupants++;
			}
		}
	}

	// On verifie les collisions
	for(int i=0; i<27; i++){
		if (casesCubesOccupation[i] > 1) {
			collision = true;
		}
	}

	if (collision) { (*stat).addCollision(); }
	this->collisionDetected = collision;

	if(occupants == 27 && !collision)
		this->hasWon = true;
}