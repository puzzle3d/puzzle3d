#include "Statistique.h"
#include "Windows.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

Statistique::Statistique() {  
	intervalle first;
	first.begin = -1;
	first.end = -1;
	collision_number = 0;
	this->currentObsIntervalle = first;
	
	this->lastMouseMove = -1;

	this->collision_number = 0;
	this->error_number = 0;
	this->mouse_move_number = 0;

}

Statistique::~Statistique() {}

void Statistique::addMouseMove() {
	int cur_timestamp = (int)GetTickCount();

	int delta = 2000;

	if (this->lastMouseMove == -1) {
		this->mouse_move_number++;
		this->lastMouseMove = (int)GetTickCount();
	} else {
		if( (cur_timestamp - this->lastMouseMove) >= delta ) {
			this->mouse_move_number++;
		}
		this->lastMouseMove = cur_timestamp;
	}
}

void Statistique::addMovementTimestamp(int t, int index) {
	
	int delta = 2000; //En seconde

	if(this->currentObsIntervalle.begin == -1) {
		this->currentObsIntervalle.begin = t;
		this->currentObsIntervalle.index_piece = index;
	} else {
		if(this->currentObsIntervalle.end == -1) {
			this->currentObsIntervalle.end = t;
		} else {
			//cout << this->currentObsIntervalle.end << " - " << t << " - "<< t - this->currentObsIntervalle.end << " - " << delta << endl;
			if((t - this->currentObsIntervalle.end) >= delta) {
				
				intervalle r;
				r.begin = this->currentObsIntervalle.begin;
				r.end = this->currentObsIntervalle.end;
				r.index_piece = this->currentObsIntervalle.index_piece;

				this->observation.push_back(r);
				this->currentObsIntervalle.begin=t;
				this->currentObsIntervalle.end=-1;
			} else {
				this->currentObsIntervalle.end=t;
			}
		}
	}
}

void Statistique::changePiece(int t) {
	if(this->currentObsIntervalle.begin != -1) {
		intervalle r;

		r.begin = this->currentObsIntervalle.begin;
		r.end = t;
		r.index_piece = this->currentObsIntervalle.index_piece;

		this->observation.push_back(r);
		this->currentObsIntervalle.begin=-1;
		this->currentObsIntervalle.end=-1;
	}
}

void Statistique::endData() { 
	 if(this->currentObsIntervalle.begin != -1) {
		 //Timestamp de fin
		 this->endTimestamp  = (int)GetTickCount();
	
		 //Dernier intervalle
		 int delta = 2000;
		 if(this->currentObsIntervalle.end == -1) {
			 this->currentObsIntervalle.end = this->endTimestamp;
		 } else if ((this->endTimestamp - this->currentObsIntervalle.end) <= delta) {
			 this->currentObsIntervalle.end = this->endTimestamp;
		 }

		intervalle r;
		r.begin = this->currentObsIntervalle.begin;
		r.end = this->currentObsIntervalle.end;
		r.index_piece = this->currentObsIntervalle.index_piece;
		this->observation.push_back(r);
	 }

	this->printObservation();
 }

void Statistique::printObservation() {
	FILE* fp;
	std::ostringstream oss;
	time_t date;
	time(&(date));
	oss << (int)date;
	string path = "./"+oss.str()+".xml";
	
	if ((fp = fopen(path.c_str(),"w")) == NULL){
		std::cout << "Impossible de créer le fichier" << std::endl;
	} else {
		fprintf(fp, "<?xml version=\"1.0\"?>\n");
		//Temps de début, de fin et durée
		fprintf(fp, "   <begin>%d</begin>\n",  this->beginTimestamp);
		fprintf(fp, "   <end>%d</end>\n", this->endTimestamp);
		fprintf(fp, "   <duration>%d</duration>\n", this->endTimestamp - this->beginTimestamp);
		fprintf(fp, "   <collision_number>%d</collision_number>\n", this->collision_number);
        fprintf(fp, "   <error_number>%d</error_number>\n", this->error_number);
		fprintf(fp, "   <mouse_move_number>%d</mouse_move_number>\n", this->mouse_move_number);

		//Intervalle de déplacements de la souris
		fprintf(fp, "   <intervals>\n");
		unsigned int size = this->observation.size();
		for(unsigned int i = 0; i < size; i++) {
			intervalle inter =  this->observation.at(i);
			fprintf(fp, "<interval>");
			fprintf(fp, "<piece_index>%d</piece_index>", inter.index_piece);
			fprintf(fp, "<begin>%d</begin>", inter.begin - this->beginTimestamp);
			fprintf(fp, "<end>%d</end>", inter.end -  this->beginTimestamp);
			fprintf(fp, "<duration>%d</duration>", inter.end - inter.begin);
			fprintf(fp, "</interval>\n");
		}

		fprintf(fp, "   </intervals>\n");
		fclose(fp);
	}
}
