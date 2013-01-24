#include "Piece.h"


Piece::Piece(OpenGLContext* context, float x, float y, float z,float nscR, float nscG, float nscB) : 
	notSelectedColor(nscR, nscG, nscB), selectedColor(1.0, 0.0, 0.0)
{
	this->context = context;

	pivot = SPivotf(	XYZf(x, y, z),
						XYZf(1.0, 0.0, 0.0),
						XYZf(0.0, 0.0, 1.0));
	this->cubes.push_back( XYZf(0,0,0));
	this->isPlacedVar = false;
	this->inCube = false;
}


Piece::~Piece(void) {}

void Piece::addElement(XYZf posRelative){
	this->cubes.push_back(posRelative);
}

void Piece::rotation(float theta, XYZf axe){
	this->movePiece();
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

void Piece::translate(XYZf vecteur){
	this->movePiece();
	pivot.SetPos(pivot.GetPos()+vecteur);
}

void Piece::dessiner(Piece* p) {
	glUniformMatrix4fv(context->attribToShader("pivotObjet"), 1, false, this->pivot.GetMatrix());

	if(this == p) {
		glUniform3f(context->attribToShader("color"), selectedColor.x, selectedColor.y, selectedColor.z);
	} else {
		glUniform3f(context->attribToShader("color"), notSelectedColor.x,notSelectedColor.y,notSelectedColor.z);
	}

	for each(XYZf cube in cubes)
	{
		context->drawCube(cube);
	}
}

bool Piece::dansPerimetre(XYZf centre, int dist){
	return (abs(this->pivot.GetPos().x-centre.x) <= dist && abs(this->pivot.GetPos().y-centre.y) <= dist && abs(this->pivot.GetPos().z-centre.z) <= dist);
}

vector<XYZf> Piece::posCubes(){
	vector<XYZf> positions;
	for each (XYZf cube in this->cubes){
		XYZf coords;
		Matrix44Mul31f(&coords, this->pivot.GetMatrix(), &(cube*2));
		coords /= 2;
		//std::cout << "dview : " << pivot.GetdView() << ", dNorm : " << pivot.GetdNorm() << ", Pos : " << pivot.GetPos() << ", Coords : " << coords << std::endl;
		positions.push_back(coords);
	}
	return positions;
}