#include "src\includer.h"
#include "src\definers.h"
#include "src\connectObj.h"
#include "src\connectInfo.h"

#include <string>
#include <vector>
#include <algorithm>

enum {
	ID_ADDCONNECTOBJ = wxID_HIGHEST + 1,
	ID_EDITLABEL = wxID_HIGHEST + 2,
	ID_CONNECT = wxID_HIGHEST + 3,
	ID_ADDPAR = wxID_HIGHEST + 4,

	ID_ADDINTVARIABLEOBJ = wxID_HIGHEST + 5,

	ID_LINESNOTARCS = wxID_HIGHEST + 6
};

class someprogFrame : public wxFrame {
public:
	someprogFrame();
	~someprogFrame() {};
	void OnClose(wxCloseEvent& event);
	
	void OnAddConnectObj(wxCommandEvent& event);
	void OnEditLabel(wxCommandEvent& event);
	void OnConnect(wxCommandEvent& event);
	void OnAddPar(wxCommandEvent& event);

	void OnAddIntVariableObj(wxCommandEvent& event);

	void OnLinesNotArcs(wxCommandEvent& event);

	wxMenuBar *menuBar;
	wxMenu *connectObjMenu, *variableObjMenu, *bunchOfOptionsMenu;

	DECLARE_EVENT_TABLE()
};

class someprogPanel : public wxPanel {
public:
	someprogPanel(wxFrame* parent) : wxPanel(parent) {
		this->SetDoubleBuffered(true);
	};

	void paintEvent(wxPaintEvent& event);
	void paintNow();

	void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnMotion(wxMouseEvent& event);

	void OnKeyDown(wxKeyEvent& event);

	void render(wxDC& dc);
	void cubicBezier(wxDC& dc, float x0, float y0, float x1, float y1, float cx0, float cy0, float cx1, float cy1, int res);

	void drawIntVariableObj(wxDC& dc, intVariableObj ivo);

	//

	DECLARE_EVENT_TABLE()
};

class someprogApp : public wxApp {
public:
	float mouseX, mouseY;

	someprogFrame* frame;
	someprogPanel* panel;

	std::vector<connectObj> conobjs;
	std::vector<connectInfo> coninfos;
	std::vector<std::unique_ptr<basicVariableObj>> varobjs;
	std::vector<varInfo> varinfos;
	int czind; // current zind
	int cdcoi; // the index of the connectObj that is currently being dragged
	int cdvoi; // the index of the variableObj that is currently being dragged

	bool isEdit; // (not) in edit label mode
	int cecoi; // the index of the edited connectObj
	int cevoi; // the index of the edited variableObj

	bool isCon; // (not) in connection mode
	int cccoi0, cccoi1; // the index of the 1st and 2nd connectObjs
	bool cccoi0d, cccoi1d; // the 1st and the 2nd connectObjs are (not) connected at their lower halves

	bool isPar; // (not) in parameter mode
	int cpcoi, cpvoi, cpvoei; // the index of the connectObj and the variableObj, and the selected item of the variableObj
	bool fromc, fromv; // the 1st selected is connectObj / variableObj

	void resetEdit(bool);
	void resetCon(bool);
	void resetPar(bool);

	void process(); // check if execution signal is sent
	void execute(int coi); // execute that particular connectObj

	bool onRenderLoop;

	bool OnInit();
	void OnIdle(wxIdleEvent& event);
	void activateRenderLoop(bool on);
	// void onExit(wxCommandEvent& event) {};

	// bunch of options
	bool linesNotArcs;
};

IMPLEMENT_APP(someprogApp)

bool someprogApp::OnInit() {
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	frame = new someprogFrame();
	panel = new someprogPanel((wxFrame*)frame);

	sizer->Add(panel, 1, wxEXPAND);

	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);
	frame->Show();

	onRenderLoop = false;
	activateRenderLoop(true);

	czind = 0;
	cdcoi = -1;
	cdvoi = -1;

	isEdit = false;
	cecoi = -1;
	cevoi = -1;

	isCon = false;
	cccoi0 = -1;
	cccoi1 = -1;
	cccoi0d = false;
	cccoi1d = false;

	isPar = false;
	cpcoi = -1;
	cpvoi = -1;
	cpvoei = -1;
	fromc = false;
	fromv = false;

	linesNotArcs = true;

	return true;
}

void someprogApp::activateRenderLoop(bool on) {
	if (on && !onRenderLoop) {
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(someprogApp::OnIdle));
		onRenderLoop = true;
	}
	else if (!on && onRenderLoop) {
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(someprogApp::OnIdle));
		onRenderLoop = false;
	}
}

void someprogApp::OnIdle(wxIdleEvent& event) {
	if (onRenderLoop) {
		panel->paintNow();
		event.RequestMore();
	}
	process();
}

void someprogApp::process() {
	for (int i = 0; i < conobjs.size(); i++) {
		conobjs[i].checkExecutable();
		if (conobjs[i].isFunc && !conobjs[i].isExecuted) {
			conobjs[i].isExecuted = true;
			execute(i);
		}
	}
}

void someprogApp::execute(int coi) {
	connectObj co = conobjs[coi];

	if (co.lbl == "exec" || co.lbl == "pass") {
		// send
		for (int i = 0; i < co.bcons.size(); i++) {
			if (conobjs[co.bcons[i]].isFunc) {
				conobjs[co.bcons[i]].isExecuted = false;
			}
		}
	} else if (co.lbl == "popmsg") {
		// the alert command
		// 1st input = message (default is "message"), 2nd input = caption (default is "caption")
		wxMessageBox(wxString("message"), wxString("caption"));

		// send
		for (int i = 0; i < co.bcons.size(); i++) {
			if (conobjs[co.bcons[i]].isFunc) {
				conobjs[co.bcons[i]].isExecuted = false;
			}
		}
	}
}

void someprogApp::resetEdit(bool set = false) {
	this->isEdit = set;
	this->cecoi = -1;
	this->cevoi = -1;
}

void someprogApp::resetCon(bool set = false) {
	this->isCon = set;
	this->cccoi0 = -1;
	this->cccoi1 = -1;
	this->cccoi0d = false;
	this->cccoi1d = false;
}

void someprogApp::resetPar(bool set = false) {
	this->isPar = set;
	this->cpcoi = -1;
	this->cpvoi = -1;
	this->cpvoei = -1;
	this->fromc = false;
	this->fromv = false;
}

BEGIN_EVENT_TABLE(someprogFrame, wxFrame)
EVT_CLOSE(someprogFrame::OnClose)
EVT_MENU(ID_ADDCONNECTOBJ, someprogFrame::OnAddConnectObj)
EVT_MENU(ID_EDITLABEL, someprogFrame::OnEditLabel)
EVT_MENU(ID_CONNECT, someprogFrame::OnConnect)
EVT_MENU(ID_ADDPAR, someprogFrame::OnAddPar)

EVT_MENU(ID_ADDINTVARIABLEOBJ, someprogFrame::OnAddIntVariableObj)

EVT_MENU(ID_LINESNOTARCS, someprogFrame::OnLinesNotArcs)
END_EVENT_TABLE()

someprogFrame::someprogFrame() : wxFrame((wxFrame*)NULL, -1 ,wxT("some program"), wxPoint(50, 50), wxSize(800, 600)) {
	menuBar = new wxMenuBar();

	connectObjMenu = new wxMenu();
	connectObjMenu->Append(ID_ADDCONNECTOBJ, _T("add connectObj"), _T("adds a new connectObj to the program."));
	connectObjMenu->Append(ID_EDITLABEL, _T("toggle edit label"), _T("enables / disables you to edit the label of an existing connectObj. click on the connectObj to be edited."));
	connectObjMenu->Append(ID_CONNECT, _T("toggle connect"), _T("enables / disables you to connect connectObjs. click on the connectObjs to be connected."));
	connectObjMenu->Append(ID_ADDPAR, _T("toggle parameter"), _T("enables / disables you to connect a variableObj to / from a connectObj."));
	menuBar->Append(connectObjMenu, _T("connectObj"));

	variableObjMenu = new wxMenu();
	variableObjMenu->Append(ID_ADDINTVARIABLEOBJ, _T("add intVariableObj"), _T("adds a new intVariableObj to the program."));
	menuBar->Append(variableObjMenu, _T("variableObj"));

	bunchOfOptionsMenu = new wxMenu();
	bunchOfOptionsMenu->Append(ID_LINESNOTARCS, _T("lines or arcs"), _T("sets whether connectObjs are connected with lines / arcs."));
	menuBar->Append(bunchOfOptionsMenu, _T("bunch of options"));

	SetMenuBar(menuBar);

	CreateStatusBar(1);
	SetStatusText(_T("connection"), 0);
}

void someprogFrame::OnClose(wxCloseEvent& event) {
	wxGetApp().activateRenderLoop(false);
	event.Skip();
}

void someprogFrame::OnAddConnectObj(wxCommandEvent& event) {
	wxGetApp().conobjs.push_back(connectObj(300, 300, "", ++wxGetApp().czind));
}

void someprogFrame::OnEditLabel(wxCommandEvent& event) {
	if (wxGetApp().isEdit) {
		wxGetApp().isEdit = false;
		if (wxGetApp().cecoi != -1) {
			wxGetApp().conobjs[wxGetApp().cecoi].isEdited = false;
		}
		if (wxGetApp().cevoi != -1) {
			wxGetApp().varobjs[wxGetApp().cevoi]->isEdited = false;
		}
	}
	else {
		wxGetApp().isEdit = true;
		wxGetApp().resetCon();
		wxGetApp().resetPar();
	}
	wxGetApp().cecoi = -1;
	wxGetApp().cevoi = -1;
}

void someprogFrame::OnConnect(wxCommandEvent& event) {
	if (wxGetApp().isCon) {
		wxGetApp().isCon = false;
	} else {
		wxGetApp().isCon = true;
		wxGetApp().resetEdit();
		wxGetApp().resetPar();
	}
	wxGetApp().cccoi0 = -1;
	wxGetApp().cccoi1 = -1;
	wxGetApp().cccoi0d = false;
	wxGetApp().cccoi1d = false;
}

void someprogFrame::OnAddPar(wxCommandEvent& event) {
	if (wxGetApp().isPar) {
		wxGetApp().isPar = false;
	} else {
		wxGetApp().isPar = true;
		wxGetApp().resetCon();
		wxGetApp().resetEdit();
	}
	wxGetApp().cpcoi = -1;
	wxGetApp().cpvoi = -1;
	wxGetApp().cpvoei = -1;
	wxGetApp().fromc = false;
	wxGetApp().fromv = false;
}

void someprogFrame::OnAddIntVariableObj(wxCommandEvent& event) {
	wxGetApp().varobjs.push_back(std::unique_ptr<intVariableObj>(new intVariableObj(300, 300, "", ++wxGetApp().czind)));
}

void someprogFrame::OnLinesNotArcs(wxCommandEvent& event) {
	wxGetApp().linesNotArcs = !wxGetApp().linesNotArcs;
}

BEGIN_EVENT_TABLE(someprogPanel, wxPanel)
EVT_PAINT(someprogPanel::paintEvent)
EVT_LEFT_DOWN(someprogPanel::OnLeftDown)
EVT_LEFT_UP(someprogPanel::OnLeftUp)
EVT_MOTION(someprogPanel::OnMotion)
EVT_KEY_DOWN(someprogPanel::OnKeyDown)
END_EVENT_TABLE()

void someprogPanel::OnLeftDown(wxMouseEvent& event) {
	// button pressed?
	// in any connection mode?
	// if not then start dragging...

	char *str = "%1$i, %2$i, %3$i, %4$i";
	printf(str, event.GetX(), event.GetY(), wxGetMousePosition().x, wxGetMousePosition().y);
	// wxLogMessage(str);

	int aai = -1; // after all index;
	for (int i = 0; i < wxGetApp().conobjs.size(); i++) {
		connectObj co = wxGetApp().conobjs[i];

		if (co.inside(event.GetX(), event.GetY())) {
			if (aai == -1) {
				aai = i;
			}
			else {
				if (co.zind > wxGetApp().conobjs[aai].zind) {
					aai = i;
				}
			}
		}
	}

	if (aai != -1) { // something is actually selected
		if (wxGetApp().isEdit) { // in edit label mode
			if (wxGetApp().cecoi != -1) {
				wxGetApp().conobjs[wxGetApp().cecoi].isEdited = false;
			}
			wxGetApp().cecoi = aai;
			wxGetApp().conobjs[aai].isEdited = true;
		}
		else if (wxGetApp().isCon) { // in connect mode
			if (wxGetApp().cccoi0 == -1) { // if the 1st is not set
				wxGetApp().cccoi0 = aai; // set the 1st
				wxGetApp().cccoi0d = wxGetApp().conobjs[aai].tdinside(event.GetX(), event.GetY());
			}
			else { // if the 1st is set
				wxGetApp().cccoi1 = aai; // set the 2nd
				wxGetApp().cccoi1d = wxGetApp().conobjs[aai].tdinside(event.GetX(), event.GetY());

				if (wxGetApp().cccoi0d != wxGetApp().cccoi1d) { // top-top, bottom-bottom connections are not allowed
					wxGetApp().coninfos.push_back(connectInfo(wxGetApp().cccoi0, wxGetApp().cccoi1, wxGetApp().cccoi0d, wxGetApp().cccoi1d));
					if (wxGetApp().cccoi0d) {
						wxGetApp().conobjs[wxGetApp().cccoi0].bcon(wxGetApp().cccoi1);
						wxGetApp().conobjs[wxGetApp().cccoi1].tcon(wxGetApp().cccoi0);
					}
					else {
						wxGetApp().conobjs[wxGetApp().cccoi0].tcon(wxGetApp().cccoi1);
						wxGetApp().conobjs[wxGetApp().cccoi1].bcon(wxGetApp().cccoi0);
					}
				}

				// reset everything!
				wxGetApp().resetCon(true);
			}
		}
		else if (wxGetApp().isPar) {
			if (wxGetApp().cpcoi == -1) {
				wxGetApp().cpcoi = aai;
				if (!wxGetApp().fromv) { // then the cObj is selected the 1st
					wxGetApp().fromc = true;
				}
				else { // then the vObj is the 1st
					if (wxGetApp().cpvoi != -1 && wxGetApp().fromv != wxGetApp().fromc) {
						float yr = 0;
						if (wxGetApp().fromv) {
							(float)wxGetApp().conobjs[wxGetApp().cpcoi].lcons.size() / (float)wxGetApp().conobjs[wxGetApp().cpcoi].nlcons;
						}
						else {
							(float)wxGetApp().conobjs[wxGetApp().cpcoi].rcons.size() / (float)wxGetApp().conobjs[wxGetApp().cpcoi].nrcons;
						}
						wxGetApp().varinfos.push_back(varInfo(wxGetApp().cpcoi, wxGetApp().cpvoi, wxGetApp().cpvoei, wxGetApp().fromc, wxGetApp().fromv, yr));
						wxGetApp().resetPar(true);
					}
				}
			}
		} else { // just drag
			wxGetApp().conobjs[aai].startDrag(event.GetX(), event.GetY());
			wxGetApp().cdcoi = aai;
		}
	}
	else { // if nothing was
		for (int i = 0; i < wxGetApp().varobjs.size(); i++) { // try with variableObjs
			if (wxGetApp().varobjs[i]->inside(event.GetX(), event.GetY()) != -1) {
				if (aai == -1) {
					aai = i;
				}
				else {
					if (wxGetApp().varobjs[i]->zind > wxGetApp().varobjs[aai]->zind) {
						aai = i;
					}
				}
			}
		}

		if (aai != -1) {
			if (wxGetApp().isEdit) { // in edit mode
				if (wxGetApp().cevoi != -1) {
					wxGetApp().varobjs[wxGetApp().cevoi]->isEdited = false;
					wxGetApp().varobjs[wxGetApp().cevoi]->editedItem = -1;
				}
				wxGetApp().cevoi = aai;
				wxGetApp().varobjs[aai]->isEdited = true;
				wxGetApp().varobjs[aai]->editedItem = wxGetApp().varobjs[aai]->inside(event.GetX(), event.GetY());
				// wxLogMessage(wxString(std::to_string(wxGetApp().varobjs[aai]->editedItem)));
			}
			else if (wxGetApp().isPar) {
				if (wxGetApp().cpvoi == -1) {
					wxGetApp().cpvoi = aai;
					wxGetApp().cpvoei = wxGetApp().varobjs[aai]->inside(event.GetX(), event.GetY());
					if (!wxGetApp().fromc) { // then this vObj is the 1st selected
						wxGetApp().fromv = true;
					} else {
						if (wxGetApp().cpcoi != -1 && wxGetApp().fromv != wxGetApp().fromc) {
							float yr = 0;
							if (wxGetApp().fromv) {
								(float)wxGetApp().conobjs[wxGetApp().cpcoi].lcons.size() / (float)wxGetApp().conobjs[wxGetApp().cpcoi].nlcons;
							}
							else {
								(float)wxGetApp().conobjs[wxGetApp().cpcoi].rcons.size() / (float)wxGetApp().conobjs[wxGetApp().cpcoi].nrcons;
							}
							wxGetApp().varinfos.push_back(varInfo(wxGetApp().cpcoi, wxGetApp().cpvoi, wxGetApp().cpvoei, wxGetApp().fromc, wxGetApp().fromv, yr));
						}
						wxGetApp().resetPar(true);
					}
				}
			} else { // just drag
				wxGetApp().varobjs[aai]->startDrag(event.GetX(), event.GetY());
				wxGetApp().cdvoi = aai;
			}
		}
		else { // really nothing was selected
			if (wxGetApp().isEdit) { // if in edit mode
				if (wxGetApp().cecoi != -1) {
					wxGetApp().conobjs[wxGetApp().cecoi].isEdited = false;
				}
				wxGetApp().cecoi = -1; // get rid of the edit focus
				if (wxGetApp().cevoi != -1) {
					wxGetApp().varobjs[wxGetApp().cevoi]->isEdited = false;
					wxGetApp().varobjs[wxGetApp().cevoi]->editedItem = -1;
				}
				wxGetApp().cevoi = -1;
			}
			else if (wxGetApp().isCon) {
				wxGetApp().resetCon(true);
			}
		}
	}
}

void someprogPanel::OnLeftUp(wxMouseEvent& event) {
	if (wxGetApp().cdcoi != -1) {
		wxGetApp().conobjs[wxGetApp().cdcoi].stopDrag();

		// is exec clicked?
		if (wxGetApp().conobjs[wxGetApp().cdcoi].lbl == "exec") {
			wxGetApp().conobjs[wxGetApp().cdcoi].isExecuted = false;
		}

		wxGetApp().cdcoi = -1;
	}
	if (wxGetApp().cdvoi != -1) {
		wxGetApp().varobjs[wxGetApp().cdvoi]->stopDrag();
		wxGetApp().cdvoi = -1;
	}
}

void someprogPanel::OnMotion(wxMouseEvent& event) {
	if (wxGetApp().cdcoi != -1) {
		wxGetApp().conobjs[wxGetApp().cdcoi].keepDrag(event.GetX(), event.GetY());
	}
	if (wxGetApp().cdvoi != -1) {
		wxGetApp().varobjs[wxGetApp().cdvoi]->keepDrag(event.GetX(), event.GetY());
	}
}

void someprogPanel::OnKeyDown(wxKeyEvent& event) {
	if (wxGetApp().isEdit) {
		wxChar uk = event.GetKeyCode();
		if (uk == WXK_BACK) {
			if (wxGetApp().cecoi != -1) {
				if (wxGetApp().conobjs[wxGetApp().cecoi].lbl.size() > 0) {
					wxGetApp().conobjs[wxGetApp().cecoi].lbl.pop_back();
				}
			}
			else if (wxGetApp().cevoi != -1) {
				wxGetApp().varobjs[wxGetApp().cevoi]->setEdit('a', true);
			}
		}
		else {
			if (event.GetUnicodeKey() != WXK_NONE) {
				if (wxGetApp().cecoi != -1) {
					wxGetApp().conobjs[wxGetApp().cecoi].lbl.push_back(uk);
					std::transform(wxGetApp().conobjs[wxGetApp().cecoi].lbl.begin(), wxGetApp().conobjs[wxGetApp().cecoi].lbl.end(), wxGetApp().conobjs[wxGetApp().cecoi].lbl.begin(), ::tolower);
				}
				else if (wxGetApp().cevoi != -1) {/*
					wxGetApp().varobjs[wxGetApp().cevoi]->lbl.push_back(uk);
					std::transform(wxGetApp().varobjs[wxGetApp().cevoi]->lbl.begin(), wxGetApp().varobjs[wxGetApp().cevoi]->lbl.end(), wxGetApp().varobjs[wxGetApp().cevoi]->lbl.begin(), ::tolower);*/
					wxGetApp().varobjs[wxGetApp().cevoi]->setEdit(uk, false);
				}
			}
		}
	}
}

void someprogPanel::paintEvent(wxPaintEvent& event) {
	wxPaintDC dc(this);
	render(dc);
}

void someprogPanel::paintNow() {/*
								wxClientDC dc(this);
								render(dc);*/
	Refresh();
}

void someprogPanel::render(wxDC& dc) {
	for (int i = 0; i < wxGetApp().conobjs.size(); i++) {
		connectObj co = wxGetApp().conobjs[i];

		dc.SetPen(wxPen(wxColor((co.isEdited ? 255 : 0), 0, (co.isFunc ? (co.isEdited ? 0 : 255) : 0))));
		dc.SetBrush(wxBrush(wxColor(255, 255, 255)));
		dc.DrawRectangle(co.x - CTS, co.y - CTS, co.w + CTS * 2, co.h + CTS * 2);
		dc.DrawText(wxString(co.lbl), co.x, co.y);

		dc.SetBrush(wxBrush(wxColor((co.isEdited ? 255 : 0), 0, (co.isFunc ? (co.isEdited ? 0 : 255) : 0))));
		dc.DrawRectangle(co.x - CTS, co.y - CTS - CS, co.w + CTS * 2, CS);
		dc.DrawRectangle(co.x - CTS, co.y + co.h + CTS, co.w + CTS * 2, CS);

		// if it is partly top-connected / bottom-connected
		if (wxGetApp().isCon) {
			if (wxGetApp().cccoi0 == i) {
				dc.SetPen(wxPen(wxColor(0, 128, 0)));
				dc.DrawLine(wxGetMousePosition().x - GetScreenPosition().x, wxGetMousePosition().y - GetScreenPosition().y, co.x - CTS + wxGetApp().cccoi0d * (co.w + CTS * 2), co.y - CTS - CS + wxGetApp().cccoi0d * (co.h + CTS * 2 + CS * 2) - 1);
			}
		}
	}

	for (int i = 0; i < wxGetApp().varobjs.size(); i++) {
		std::unique_ptr<basicVariableObj> bvo = std::move(wxGetApp().varobjs[i]);
		
		if (bvo->type == "int") {
			intVariableObj *ivo = dynamic_cast<intVariableObj*>(bvo.get());
			if (ivo) {
				ivo->draw(dc);
			}
			else {
				dc.DrawText("!!!", 500, 500);
			}
		}

		wxGetApp().varobjs[i] = std::move(bvo);
		bvo.reset();

		/*
		// if it is partly top-connected / bottom-connected
		if (wxGetApp().isCon) {
		if (wxGetApp().cccoi0 == i) {
		dc.SetPen(wxPen(wxColor(0, 128, 0)));
		dc.DrawLine(wxGetMousePosition().x - GetScreenPosition().x, wxGetMousePosition().y - GetScreenPosition().y, co.x - CTS + wxGetApp().cccoi0d * (co.w + CTS * 2), co.y - CTS - CS + wxGetApp().cccoi0d * (co.h + CTS * 2 + CS * 2) - 1);
		}
		}*/
	}

	for (int i = 0; i < wxGetApp().coninfos.size(); i++) {
		connectInfo ci = wxGetApp().coninfos[i];
		connectObj c0 = wxGetApp().conobjs[ci.c0], c1 = wxGetApp().conobjs[ci.c1];

		dc.SetPen(wxPen(wxColor(0, 0, 0)));
		dc.SetBrush(wxNullBrush);
		if (wxGetApp().linesNotArcs) {
			dc.DrawLine(c0.x - CTS + ci.ctd0 * (c0.w + CTS * 2), c0.y - CTS - CS + ci.ctd0 * (c0.h + CTS * 2 + CS * 2) - 1, c1.x - CTS + ci.ctd1 * (c1.w + CTS * 2), c1.y - CTS - CS + ci.ctd1 * (c1.h + CTS * 2 + CS * 2) - 1);
		}
		else {
			cubicBezier(dc, c0.x - CTS + ci.ctd0 * (c0.w + CTS * 2), c0.y - CTS - CS + ci.ctd0 * (c0.h + CTS * 2 + CS * 2) - 1, c1.x - CTS + ci.ctd1 * (c1.w + CTS * 2), c1.y - CTS - CS + ci.ctd1 * (c1.h + CTS * 2 + CS * 2) - 1, (c0.x + c1.x) / 2.0f + ci.cx0, (c0.y + c1.y) / 2.0f + ci.cy0, (c0.x + c1.x) / 2.0f + ci.cx1, (c0.y + c1.y) / 2.0f + ci.cy1, 100);
		}
	}

	for (int i = 0; i < wxGetApp().varinfos.size(); i++) {
		varInfo vi = wxGetApp().varinfos[i];
		connectObj c = wxGetApp().conobjs[vi.c];
		std::unique_ptr<basicVariableObj> v = std::move(wxGetApp().varobjs[vi.v]);

		dc.SetPen(wxPen(wxColor(0, 0, 0)));
		dc.SetBrush(wxBrush(wxColor(255, 255, 255)));
		if (wxGetApp().linesNotArcs) {
			dc.DrawLine(c.x - CTS + (vi.fromc) * (c.w + CTS * 2), c.y - CTS - CS + abs(vi.yr) * (c.h + CTS * 2 + CS * 2) - 1, v->x - CTS + (vi.fromv) * (v->w + CTS * 2), v->y - CTS - CS + 0.5f * (v->h + CTS * 2 + CS * 2) - 1);
		}
		else {
			cubicBezier(dc, c.x - CTS + (vi.fromc) * (c.w + CTS * 2), c.y - CTS - CS + abs(vi.yr) * (c.h + CTS * 2 + CS * 2) - 1, v->x - CTS + (vi.fromv) * (v->w + CTS * 2), v->y - CTS - CS + 0.5f * (v->h + CTS * 2 + CS * 2) - 1, (c.x + v->x) / 2.0f + vi.cx0, (c.y + v->y) / 2.0f + vi.cy0, (c.x + v->x) / 2.0f + vi.cx1, (c.y + v->y) / 2.0f + vi.cy1, 100);
		}
		dc.DrawEllipse(vi.fromc ? c.x - CTS + (c.w + CTS * 2) : v->x - CTS + (v->w + CTS * 2), vi.fromc ? c.y - CTS - CS + abs(vi.yr) * (c.h + CTS * 2 + CS * 2) - 1 : v->y - CTS - CS + 0.5f * (v->h + CTS * 2 + CS * 2) - 1, 5, 5);

		wxGetApp().varobjs[vi.v] = std::move(v);
	}

	char info[150];
	char *fmt = "# of connectObjs: %d\nedit label mode is: %s\nconnect mode is: %s\nparam mode is: %s\n\noptions\nlines or arcs: %s";
	sprintf(info, fmt, int(wxGetApp().conobjs.size()), (wxGetApp().isEdit ? "ON" : "OFF"), (wxGetApp().isCon ? "ON" : "OFF"), (wxGetApp().isPar ? "ON" : "OFF"), (wxGetApp().linesNotArcs ? "LINES" : "ARCS"));
	dc.DrawText(wxString(info), 10, 10);
}

// developed according to nice and clear explanations on http://ruiueyama.tumblr.com/post/11197882224
void someprogPanel::cubicBezier(wxDC& dc, float x0, float y0, float x1, float y1, float cx0, float cy0, float cx1, float cy1, int res) {
	float pbx = 0, pby = 0;
	for (int iter = 0; iter < res; iter++) {
		float t = (float)iter / (float)(res - 1);

		float mx0 = x0 + (cx0 - x0) * t;
		float my0 = y0 + (cy0 - y0) * t;
		float mx1 = cx0 + (cx1 - cx0) * t;
		float my1 = cy0 + (cy1 - cy0) * t;
		float mx2 = cx1 + (x1 - cx1) * t;
		float my2 = cy1 + (y1 - cy1) * t;
		float bx0 = mx0 + (mx1 - mx0) * t;
		float by0 = my0 + (my1 - my0) * t;
		float bx1 = mx1 + (mx2 - mx1) * t;
		float by1 = my1 + (my2 - my1) * t;
		float bx = bx0 + (bx1 - bx0) * t;
		float by = by0 + (by1 - by0) * t;

		if (iter > 0) {
			dc.DrawLine(pbx, pby, bx, by);
		}

		pbx = bx;
		pby = by;
	}
}