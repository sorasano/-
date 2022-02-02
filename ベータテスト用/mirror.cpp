#include "mirror.h"
#include "Dxlib.h"

void DrawMirror(int x, int y, int r, int t, int angle) {

	//����
	int R = 228;
	int G = 240;
	int B = 248;

	//���
	int R2 = 46;
	int G2 = 76;
	int B2 = 98;

	if (angle == 0) {
		//�O�p�`1
		DrawTriangle(
			x - r, y - r,
			x - r, y - r + t,
			x + r, y + r,
			GetColor(R,G,B),
			true
		);
		//�O�p�`2
		DrawTriangle(
			x - r, y - r + t,
			x + r - t, y + r,
			x + r, y + r,
			GetColor(R, G, B),
			true
		);

		//�O�p�`3
		DrawTriangle(
			x - r, y - r + t,
			x - r, y - r + (2 * t),
			x + r - t, y + r,
			GetColor(R2, G2, B2),
			true
		);
		//�O�p�`4
		DrawTriangle(
			x - r, y - r + (2 * t),
			x + r - (2 * t), y + r,
			x + r - t, y + r,
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 1) {
		//�O�p�`1
		DrawTriangle(
			x - r, y + r,
			x + r, y - r,
			x - r, y + r - t,
			GetColor(R, G, B),
			true
		);
		//�O�p�`2
		DrawTriangle(
			x - r, y + r - t,
			x + r, y - r,
			x + r - t, y - r,
			GetColor(R, G, B),
			true
		);

		//�O�p�`3
		DrawTriangle(
			x - r, y + r - t,
			x - r, y + r - (2 * t),
			x + r - t, y - r,
			GetColor(R2, G2, B2),
			true
		);
		//�O�p�`4
		DrawTriangle(
			x - r, y + r - (2 * t),
			x + r - (2 * t), y - r,
			x + r - t, y - r,
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 2) {
		//�O�p�`1
		DrawTriangle(
			x - r, y - r,
			x + r, y + r,
			x - r + t, y - r,
			GetColor(R, G, B),
			true
		);
		//�O�p�`2
		DrawTriangle(
			x - r + t, y - r,
			x + r, y + r,
			x + r, y + r - t,
			GetColor(R, G, B),
			true
		);

		//�O�p�`3
		DrawTriangle(
			x - r + t, y - r,
			x + r, y + r - t,
			x - r + (2 * t), y - r,
			GetColor(R2, G2, B2),
			true
		);
		//�O�p�`4
		DrawTriangle(
			x - r + (2 * t), y - r,
			x + r, y + r - t,
			x + r, y + r - (2 * t),
			GetColor(R2, G2, B2),
			true
		);
	}
	else if (angle == 3) {
		//�O�p�`1
		DrawTriangle(
			x - r, y + r,
			x + r, y - r,
			x - r + t, y + r,
			GetColor(R, G, B),
			true
		);
		//�O�p�`2
		DrawTriangle(
			x - r + t, y + r,
			x + r, y - r,
			x + r, y - r + t,
			GetColor(R, G, B),
			true
		);

		//�O�p�`3
		DrawTriangle(
			x - r + t, y + r,
			x - r + (2 * t), y + r,
			x + r, y - r + t,
			GetColor(R2, G2, B2),
			true
		);
		//�O�p�`4
		DrawTriangle(
			x + r, y - r + (2 * t),
			x - r + (2 * t), y + r,
			x + r, y - r + t,
			GetColor(R2, G2, B2),
			true
		);
	}
}