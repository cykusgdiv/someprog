#ifndef CONNECTOBJ_H
#define CONNECTOBJ_H

#include <string>
#include <vector>

#include "includer.h"

class connectObj {
	public:
		float x, y; // the position of the connectObj
		float w, h; // the width and the height
		std::string lbl; // the label of the connectObj
		
		// for dragging
		int zind; // the z-index of the connectObj, used for determining which connectObj to drag
		float dx, dy; // the "offset" position of the connectObj when dragging starts
		bool isDragged; // is it currently being dragged?

		bool isEdited; // is it currently being edited?

		// input from left, output to right
		// receive signal from top, send signal from bottom
		int ttcons, tbcons, tlcons, trcons; // total number of connections
		std::vector<int> tcons, bcons, lcons, rcons; // all the indices of the connectObjs connected TO THIS connectObj's top, bottom, left and right
		int nlcons, nrcons; // number of parameters required / output

		bool isFunc; // is this executable?
		bool isExecuted; // is this executed? if false, turn to true and execute whatever

		connectObj(float cx, float cy, std::string clbl, int czind) {
			this->x = cx;
			this->y = cy;
			this->w = 50;
			this->h = TEXT_SIZE;
			this->lbl = clbl;
			this->zind = czind;
			this->isDragged = false;
			this->isEdited = false;
			this->ttcons = 0;
			this->tbcons = 0;
			this->tlcons = 0;
			this->trcons = 0;
			this->nlcons = 0;
			this->nrcons = 0;

			this->dx = 0;
			this->dy = 0;

			this->isFunc = false;
			this->isExecuted = true;
		}

		void tcon(int i) {
			tcons.push_back(i);
		}
		void bcon(int i) {
			bcons.push_back(i);
		}
		void lcon(int i) {
			lcons.push_back(i);
		}
		void rcon(int i) {
			rcons.push_back(i);
		}

		bool inside(float tx, float ty) {
			return (tx > x - CONNECTOBJ_TEXT_SPACE) && (tx < x + w + CONNECTOBJ_TEXT_SPACE) && (ty > y - CONNECTOBJ_TEXT_SPACE) && (ty < y + h + CONNECTOBJ_TEXT_SPACE);
		}

		bool tdinside(float tx, float ty) {
			return (tx > x - CONNECTOBJ_TEXT_SPACE) && (tx < x + w + CONNECTOBJ_TEXT_SPACE) && (ty > y + TEXT_SIZE / 2) && (ty < y + h + CONNECTOBJ_TEXT_SPACE);
		}

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

		void checkExecutable() {
			this->nlcons = 0;
			this->nrcons = 0;
			if (this->lbl == "exec" ||
				this->lbl == "popmsg" ||
				this->lbl == "pass"
				) {
				this->isFunc = true;
				if (this->lbl == "popmsg") {
					this->nlcons = 2;
				}
			} else {
				this->isFunc = false;
			}
		}
};

#endif