#include "mirror.h"
#include "Dxlib.h"

void DrawMirror(int x, int y, int r, int t, int angle) {

	//鏡面
	int R = 228;
	int G = 240;
	int B = 248;

	//後ろ
	int R2 = 46;
	int G2 = 76;
	int B2 = 98;

	if (angle == 0) {
		//三角形1
		DrawTriangle(
			x - r, y - r,
			x - r, y - r + t,
			x + r, y + r,
			GetColor(R,G,B),
			true
		);
		//三角形2
		DrawTriangle(
			x - r, y - r + t,
			x + r - t, y + r,
			x + r, y + r,
			GetColor(R, G, B),
			true
		);

		//三角形3
		DrawTriangle(
			x - r, y - r + t,
			x - r, y - r + (2 * t),
			x + r - t, y + r,
			GetColor(R2, G2, B2),
			true
		);
		//三角形4
		DrawTriangle(
			x - r, y - r + (2 * t),
			x + r - (2 * t), y + r,
			x + r - t, y + r,
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 1) {
		//三角形1
		DrawTriangle(
			x - r, y + r,
			x + r, y - r,
			x - r, y + r - t,
			GetColor(R, G, B),
			true
		);
		//三角形2
		DrawTriangle(
			x - r, y + r - t,
			x + r, y - r,
			x + r - t, y - r,
			GetColor(R, G, B),
			true
		);

		//三角形3
		DrawTriangle(
			x - r, y + r - t,
			x - r, y + r - (2 * t),
			x + r - t, y - r,
			GetColor(R2, G2, B2),
			true
		);
		//三角形4
		DrawTriangle(
			x - r, y + r - (2 * t),
			x + r - (2 * t), y - r,
			x + r - t, y - r,
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 2) {
		//三角形1
		DrawTriangle(
			x - r, y - r,
			x + r, y + r,
			x - r + t, y - r,
			GetColor(R, G, B),
			true
		);
		//三角形2
		DrawTriangle(
			x - r + t, y - r,
			x + r, y + r,
			x + r, y + r - t,
			GetColor(R, G, B),
			true
		);

		//三角形3
		DrawTriangle(
			x - r + t, y - r,
			x + r, y + r - t,
			x - r + (2 * t), y - r,
			GetColor(R2, G2, B2),
			true
		);
		//三角形4
		DrawTriangle(
			x - r + (2 * t), y - r,
			x + r, y + r - t,
			x + r, y + r - (2 * t),
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 3) {
		//三角形1
		DrawTriangle(
			x - r, y + r,
			x + r, y - r,
			x - r + t, y + r,
			GetColor(R, G, B),
			true
		);
		//三角形2
		DrawTriangle(
			x - r + t, y + r,
			x + r, y - r,
			x + r, y - r + t,
			GetColor(R, G, B),
			true
		);

		//三角形3
		DrawTriangle(
			x - r + t, y + r,
			x - r + (2 * t), y + r,
			x + r, y - r + t,
			GetColor(R2, G2, B2),
			true
		);
		//三角形4
		DrawTriangle(
			x + r, y - r + (2 * t),
			x - r + (2 * t), y + r,
			x + r, y - r + t,
			GetColor(R2, G2, B2),
			true
		);
	}
}