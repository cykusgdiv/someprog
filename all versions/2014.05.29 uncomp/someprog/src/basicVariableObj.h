#ifndef BASICVARIABLEOBJ_H
#define BASICVARIABLEOBJ_H

#include <string>
#include <vector>

#include "includer.h"
#include "definers.h"

class basicVariableObj { // the basic variableObj every other one has to extend. use dynamic_cast to cast it down
public:
	float x, y; // the position of the variableObj
	float w, h; // the width and the height
	std::string lbl; // the label of the variableObj
	std::string type; // the type of the variableObj
	// everything else depends on the type-specific variableObjs

	// for dragging
	int zind; // the z-index of the variableObj, used for determining which variableObj to drag
	float dx, dy; // the "offset" position of the variableObj when dragging starts
	bool isDragged; // is it currently being dragged?
	
	bool isEdited; // is it currently being edited?
	int editedItem; // indicates the item being edited; -1 = nothing is edited, 0 = lbl, 1, 2, 3, 4, 5.... = other properties; corresponds to inside()

	// input from left, output to right
	// receive signal from top, send signal from bottom
	int tlcons, trcons; // total number of connections
	std::vector<int> lcons, rcons; // all the indices of the connectObjs connected TO THIS connectObj's top, bottom, left and right

	basicVariableObj(float cx, float cy, std::string clbl, int czind) {
		this->x = cx;
		this->y = cy;
		this->w = 50;
		this->h = TEXT_SIZE;
		this->lbl = clbl;
		this->zind = czind;
		this->isDragged = false;
		this->isEdited = false;
		this->editedItem = -1;
		this->tlcons = 0;
		this->trcons = 0;

		this->dx = 0;
		this->dy = 0;
	}

	virtual void setEdit(char a, bool remove = false) = 0; // do something in the derived classes

	virtual std::string getStringInfo() { // IMPORTANT: used when the variable is being output
		return "";
	}

	void lcon(int i) {
		lcons.push_back(i);
	}
	void rcon(int i) {
		rcons.push_back(i);
	}

	virtual int inside(float tx, float ty) = 0; // do something in the derived classes please

	void startDrag(float mx, float my) {
		if (!this->isDragged) {
			this->dx = mx - this->x;
			this->dy = my - this->y;
			this->isDragged = true;
		}
	}

	void keepDrag(float mx, float my) {
		if (this->isDragged) {
			this->x = mx - this->dx;
			this->y = my - this->dy;
		}
	}

	void stopDrag() {
		if (this->isDragged) {
			this->isDragged = false;
			this->dx = 0;
			this->dy = 0;
		}
	}

	virtual void draw(wxDC &dc) {}
};

#endif