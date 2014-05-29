#ifndef CONNECTINFO_H
#define CONNECTINFO_H

class connectInfo {
	public:
		int c0, c1; // what are connected?
		bool ctd0, ctd1; // is c0 connected at bottom? is c1?

		float cx0, cy0, cx1, cy1; // center of arcs drawn

		connectInfo(int cc0, int cc1, bool ctdc0, bool ctdc1) {
			this->c0 = cc0;
			this->c1 = cc1;
			this->ctd0 = ctdc0;
			this->ctd1 = ctdc1;

			float r0 = (rand() % 180) * 3.14f, r1 = (rand() % 180) * 3.14f;
			this->cx0 = cos(r0) * 100.0f;
			this->cy0 = sin(r1) * 100.0f;
			this->cx1 = cos(r1) * 100.0f;
			this->cy1 = sin(r0) * 100.0f;
		}
};

#endif