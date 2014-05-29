#ifndef INTVARIABLEOBJ_H
#define INTVARIABLEOBJ_H

#include <string>
#include <vector>

#include "includer.h"
#include "definers.h"

class intVariableObj: public basicVariableObj {
public:
	int val; // the value of the integer
	// everything else depends on the type-specific variableObjs

	intVariableObj(float cx, float cy, std::string clbl, int czind) : basicVariableObj(cx, cy, clbl, czind) {
		this->type = "int";
		this->val = 0;
	}

	virtual void setEdit(char a, bool remove = false) {
		if (this->isEdited) {
			switch (this->editedItem) {
				case 0:
					if (remove) {
						if (this->lbl.size() > 0) {
							this->lbl.pop_back();
						}
					} else {
						this->lbl.push_back(a);
						if (this->lbl.size() > 0) {
							std::transform(this->lbl.begin(), this->lbl.end(), this->lbl.begin(), ::tolower);
						}
					}
					break;

				case 1:
					if (remove) {
						std::string tmp = std::to_string(this->val);
						if (tmp.size() > 0) {
							tmp.pop_back();
							if (tmp.size() == 0) {
								tmp = "0";
							}
						}
						this->val = std::stoi(tmp);
					} else {
						std::string tmp = std::to_string(this->val);
						tmp.push_back(a);
						if (tmp.size() > 0) {
							std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
						}
						this->val = std::stoi(tmp);
					}
					break;
			}
		}
	}

	virtual std::string getStringInfo() {
		return std::to_string(this->val);
	}

	virtual int inside(float tx, float ty) {
		int res = -1;

		if ((tx > x) &&
			(tx < x + w) &&
			(ty > y) &&
			(ty < y + h)) {
			res = 0;
		}
		else if ((tx > x) &&
			(tx < x + w) &&
			(ty > y + h + CTS * 3) &&
			(ty < y + h * 2 + CTS * 3)) {
			res = 1;
		}

		return res;
	}

	virtual void draw(wxDC &dc) {
		dc.SetPen(wxPen(wxColor(128, 0, 0)));
		dc.SetBrush(wxBrush(wxColor(255, 255, 255)));

		// int marker
		dc.DrawRectangle(this->x + this->w + CTS * 2, this->y - CTS, this->h + CTS * 2, this->h + CTS * 2);
		dc.DrawText(wxString(this->type), this->x + this->w + CTS * 3, this->y);

		// label
		dc.SetPen(wxPen(wxColor(128, 0, 0)));
		dc.DrawRectangle(this->x - this->h - CTS * 4, this->y - CTS, this->h + CTS * 2, this->h + CTS * 2);
		dc.DrawText(wxString("lbl"), this->x - this->h - CTS * 3, this->y);
		dc.SetPen(wxPen(wxColor(((this->editedItem == 0) ? 255 : 128), 0, 0)));
		dc.DrawRectangle(this->x - CTS, this->y - CTS, this->w + CTS * 2, this->h + CTS * 2);
		dc.DrawText(wxString(this->lbl), this->x, this->y);

		// val
		dc.SetPen(wxPen(wxColor(128, 0, 0)));
		dc.DrawRectangle(this->x - this->h - CTS * 4, this->y + this->h + CTS * 2, this->h + CTS * 2, this->h + CTS * 2);
		dc.DrawText(wxString("val"), this->x - this->h - CTS * 3, this->y + this->h + CTS * 3);
		dc.SetPen(wxPen(wxColor(((this->editedItem == 1) ? 255 : 128), 0, 0)));
		dc.DrawRectangle(this->x - CTS, this->y + this->h + CTS * 2, this->w + CTS * 2, this->h + CTS * 2);
		dc.DrawText(wxString(std::to_string(this->val)), this->x, this->y + this->h + CTS * 3);
	}
};

#endif