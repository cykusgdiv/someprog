#ifndef VARINFO_H
#define VARINFO_H

#include "includer.h"

class varInfo {
public:
	int c, v, ve; // what are connected?
	float yr; // the y-ratio (which parameter / which output? e.g. 1st of 5 parameters = 0.2)
	bool fromc, fromv; // is the source of information coming from c or from v?

	float cx0, cy0, cx1, cy1; // center of arcs drawn

	varInfo(int cc, int cv, int cve, bool cfc, bool cfv, float cyr = -0.5) {
		this->c = cc;
		this->v = cv;
		this->ve = cve;
		this->yr = cyr;
		this->fromc = cfc;
		this->fromv = cfv;

		float r0 = (rand() % 180) * 3.14f, r1 = (rand() % 180) * 3.14f;
		this->cx0 = cos(r0) * 100.0f;
		this->cy0 = sin(r1) * 100.0f;
		this->cx1 = cos(r1) * 100.0f;
		this->cy1 = sin(r0) * 100.0f;
	}
};

#endif