#include "DxLib.h"

#define PI    3.1415926535897932384626433832795f

typedef enum MirrorDirection
{
	UpRight,//0 const int UpRight = 0;
	DownRight,//1
	DownLeft,//2
	UpLeft//3
}MirrorDirection;

typedef enum LightDirection
{
	Up,//0
	Right,//1
	Down,//2
	Left//3
}LightDirection;

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "LC1B: ライトメア";

// ウィンドウ横幅
const int WIN_WIDTH = 64 * 14;

// ウィンドウ縦幅
const int WIN_HEIGHT = 64 * 14;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
				   _In_ int nCmdShow) {
	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ウィンドウサイズを手動では変更させず、
	// かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// タイトルを変更
	SetMainWindowText(TITLE);

	// 画面サイズの最大サイズ、カラービット数を設定(モニターの解像度に合わせる)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// 画面サイズを設定(解像度との比率で設定)
	SetWindowSizeExtendRate(1.0);

	// 画面の背景色を設定する
	SetBackgroundColor(0x00, 0x00, 0x00);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像などのリソースデータの変数宣言と読み込み

	int mirrorGH = LoadGraph("mirror.png");
	int blackGH = LoadGraph("black.png");
	int whiteGH = LoadGraph("white.png");

	// ゲームループで使う変数の宣言

	//枠

	const int BLOCKSIZE = 64;
	int outX = BLOCKSIZE * 2;
	int outY = BLOCKSIZE * 2;

	int startX = outX;
	int startY = outY;

	int endX = WIN_WIDTH - outX;
	int endY = WIN_HEIGHT - outY;

	//自機

	int playerPosX = 0;
	int playerPosY = 0;
	int playerRadius = 32;
	int playerSpeed = 3;

	//鏡

	//鏡の最大値
	const int MIRROR_MAX = 5;
	int mirrorMax = 0;//ステージごとに変える用

	int mirrorPosX[MIRROR_MAX] = {};
	int mirrorPosY[MIRROR_MAX] = {};

	int mirrorRadius = 32;//鏡半径

	int mirrorDirection[MIRROR_MAX] = {};//0↗ 1↘ 2↙ 3↖
	float mirrorAngle[MIRROR_MAX] = {};//鏡角度

	//移動範囲

	int mirrorRangeUpX[MIRROR_MAX] = {};//移動範囲の左上の座標
	int mirrorRangeUpY[MIRROR_MAX] = {};

	int mirrorRangeDownX[MIRROR_MAX] = {};//移動範囲の右下の座標
	int mirrorRangeDownY[MIRROR_MAX] = {};

	//光

	//光の最大個数
	const int LIGHT_MAX = 5;
	int lightMax = 1;//今出てる光の最大値

	//現在の座標保存用

	int lightStartPosX[LIGHT_MAX] = {};
	int lightStartPosY[LIGHT_MAX] = {};

	int lightEndPosX[LIGHT_MAX] = {};
	int lightEndPosY[LIGHT_MAX] = {};

	//光の当たっていない時の保存用変数

	int lightOldStartPosX[LIGHT_MAX] = {};
	int lightOldStartPosY[LIGHT_MAX] = {};

	int lightOldEndPosX[LIGHT_MAX] = {};
	int lightOldEndPosY[LIGHT_MAX] = {};

	int lightRadius = 32;

	int lightDirection[MIRROR_MAX + 1] = {};//0↑　1→　2↓　3←
	int lightOldDirection[MIRROR_MAX + 1] = {};

	int reset[LIGHT_MAX] = {};

	int lightTouch[LIGHT_MAX] = {};//光が当たっているか
	int lightTouchMirror[LIGHT_MAX] = {};//光に今当たっている鏡

	int lightContact = 0;//当たっている鏡の数

	int isTouch[LIGHT_MAX] = {};

	//ステージ

	int stage = 0;
	int stageReset = 0;

	//ゴール

	int goalPosX = 0;
	int goalPosY = 0;

	int goalRadius = 32;
	int isGoal = 0;//0 ゴール不可能 ゴール可能

	// 最新のキーボード情報用
	char keys[256] = { 0 };

	// 1ループ(フレーム)前のキーボード情報
	char oldkeys[256] = { 0 };

	// ゲームループ
	while (true) {
		// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
		for (int i = 0; i < 256; ++i) {
			oldkeys[i] = keys[i];
		}
		// 最新のキーボード情報を取得
		GetHitKeyStateAll(keys);

		// 画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		// 更新処理

		//ステージ移動
		if (keys[KEY_INPUT_RETURN] == 1 && oldkeys[KEY_INPUT_RETURN] == 0) {
			stage++;
			stageReset = 0;
		}

		//ステージ移動時初期化
		if (stageReset == 0) {

			lightMax = 0;
			lightContact = 0;

			//ステージ1

			if (stage == 1) {

				stageReset = 1;

				//最初の光の座標
				lightStartPosX[0] = WIN_WIDTH / 2;
				lightStartPosY[0] = startY;
				lightEndPosX[0] = WIN_WIDTH / 2;
				lightEndPosY[0] = endY;

				//初期座標の保存
				lightOldStartPosX[0] = lightStartPosX[0];
				lightOldStartPosY[0] = lightStartPosY[0];
				lightOldEndPosX[0] = lightEndPosX[0];
				lightOldEndPosY[0] = lightEndPosY[0];

				//光初期向き

				lightDirection[0] = 2;

				//出てくる鏡の個数-1(1個なら0、2個なら1)
				mirrorMax = 1;

				//鏡の初期値+最初の光の向き+移動範囲

				mirrorPosX[0] = WIN_WIDTH / 2 - BLOCKSIZE * 2;
				mirrorPosY[0] = WIN_HEIGHT / 2;

				//mirrorRangeUpX[0] = mirrorPosX[0] - (BLOCKSIZE * 1);//移動範囲の左上の座標
				//mirrorRangeUpY[0] = mirrorPosY[0] - (BLOCKSIZE * 2);

				//mirrorRangeDownX[0] = mirrorPosX[0] + (BLOCKSIZE * 3);//移動範囲の右下の座標
				//mirrorRangeDownY[0] = mirrorPosY[0] + (BLOCKSIZE * 2);

				mirrorRangeUpX[0] = 0;//移動範囲の左上の座標
				mirrorRangeUpY[0] = 0;

				mirrorRangeDownX[0] = WIN_WIDTH;//移動範囲の右下の座標
				mirrorRangeDownY[0] = WIN_HEIGHT;

				mirrorDirection[0] = 2;



				mirrorPosX[1] = WIN_WIDTH / 2 + BLOCKSIZE * 2;
				mirrorPosY[1] = WIN_HEIGHT / 2;

				//mirrorRangeUpX[1] = mirrorPosX[1] - (BLOCKSIZE * 1);//移動範囲の左上の座標
				//mirrorRangeUpY[1] = mirrorPosY[1] - (BLOCKSIZE * 2);

				//mirrorRangeDownX[1] = mirrorPosX[1] + (BLOCKSIZE * 3);//移動範囲の右下の座標
				//mirrorRangeDownY[1] = mirrorPosY[1] + (BLOCKSIZE * 2);

				mirrorRangeUpX[1] = 0;//移動範囲の左上の座標
				mirrorRangeUpY[1] = 0;

				mirrorRangeDownX[1] = WIN_WIDTH;//移動範囲の右下の座標
				mirrorRangeDownY[1] = WIN_HEIGHT;

				mirrorDirection[1] = 0;



				//mirrorPosX[2] = WIN_WIDTH / 2 + BLOCKSIZE * 2;
				//mirrorPosY[2] = WIN_HEIGHT / 2 + BLOCKSIZE * 2;

				//mirrorRangeUpX[2] = 0;//移動範囲の左上の座標
				//mirrorRangeUpY[2] = 0;

				//mirrorRangeDownX[2] = WIN_WIDTH;//移動範囲の右下の座標
				//mirrorRangeDownY[2] = WIN_HEIGHT;

				//mirrorDirection[2] = 1;


				//ゴールの初期位置

				goalPosX = endX - goalRadius;
				goalPosY = endY - goalRadius;

				//プレイヤーの初期位置

				playerPosX = startX + playerRadius;
				playerPosY = startY + playerRadius;


			}
		}

		if (stage > 0) {

			if (keys[KEY_INPUT_W] == 1) {

				for (int i = 0; i <= mirrorMax; i++) {
					if (mirrorPosY[i] > mirrorRangeUpY[i] + (BLOCKSIZE / 2)) {
						mirrorPosY[i]--;
					}
				}
			}
			if (keys[KEY_INPUT_S] == 1) {
				for (int i = 0; i <= mirrorMax; i++) {
					if (mirrorPosY[i] < mirrorRangeDownY[i] - (BLOCKSIZE / 2)) {
						mirrorPosY[i]++;
					}
				}
			}

			if (keys[KEY_INPUT_A] == 1) {
				for (int i = 0; i <= mirrorMax; i++) {
					if (mirrorPosX[i] > mirrorRangeUpX[i] + (BLOCKSIZE / 2)) {
						mirrorPosX[i]--;
					}
				}
			}
			if (keys[KEY_INPUT_D] == 1) {
				for (int i = 0; i <= mirrorMax; i++) {
					if (mirrorPosX[i] < mirrorRangeDownX[i] - (BLOCKSIZE / 2)) {
						mirrorPosX[i]++;
					}
				}
			}

			if (keys[KEY_INPUT_UP] == 1) {

				if (playerPosY > startY + playerRadius) {
					playerPosY -= playerSpeed;
				}

			}
			if (keys[KEY_INPUT_DOWN] == 1) {

				if (playerPosY < endY - playerRadius) {
					playerPosY += playerSpeed;
				}


			}

			if (keys[KEY_INPUT_LEFT] == 1) {

				if (playerPosX > startX + playerRadius) {
					playerPosX -= playerSpeed;
				}

			}
			if (keys[KEY_INPUT_RIGHT] == 1) {

				if (playerPosX < endX - playerRadius) {
					playerPosX += playerSpeed;
				}

			}

			//----------鏡------------

			//角度変更

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {

				for (int i = 0; i <= mirrorMax; i++) {
					mirrorDirection[i]++;

					if (mirrorDirection[i] > 3) {
						mirrorDirection[i] = 0;
					}
				}

			}

			for (int i = 0; i <= mirrorMax; i++) {
				//角度を変数に保存
				if (mirrorDirection[i] == 0) {
					mirrorAngle[i] = 0.0f;
				}
				else if (mirrorDirection[i] == 1) {
					mirrorAngle[i] = PI / 2;
				}
				else if (mirrorDirection[i] == 2) {
					mirrorAngle[i] = PI;
				}
				else if (mirrorDirection[i] == 3) {
					mirrorAngle[i] = PI + (PI / 2);
				}
			}

			//-----------光-----------

			//光の反射

			//鏡と光の当たり判定

			for (int i = 0; i <= mirrorMax; i++) {
				for (int j = 0; j <= lightMax; j++) {


					DrawFormatString(0, 120, GetColor(255, 255, 255), "i = %d,j=%d", i, j, true);

					//光と鏡の当たり判定

					if (lightDirection[j] == 0) {

						if (mirrorPosX[i] - mirrorRadius < lightStartPosX[j] + lightRadius && lightEndPosX[j] - lightRadius < mirrorPosX[i] + mirrorRadius) {
							if (mirrorPosY[i] - mirrorRadius < lightStartPosY[j] && lightEndPosY[j] < mirrorPosY[i] + mirrorRadius) {

								isTouch[j] = 1;

							}
							else {
								isTouch[j] = 0;
							}
						}
						else {
							isTouch[j] = 0;
						}
					}
					if (lightDirection[j] == 1) {

						if (mirrorPosX[i] - mirrorRadius < lightEndPosX[j] && lightStartPosX[j] < mirrorPosX[i] + mirrorRadius) {
							if (mirrorPosY[i] - mirrorRadius < lightEndPosY[j] + lightRadius && lightStartPosY[j] - lightRadius < mirrorPosY[i] + mirrorRadius) {

								isTouch[j] = 1;

							}
							else {
								isTouch[j] = 0;
							}
						}
						else {
							isTouch[j] = 0;
						}

					}
					if (lightDirection[j] == 2) {

						if (mirrorPosX[i] - mirrorRadius < lightEndPosX[j] + lightRadius && lightStartPosX[j] - lightRadius < mirrorPosX[i] + mirrorRadius) {
							if (mirrorPosY[i] - mirrorRadius < lightEndPosY[j] && lightStartPosY[j] < mirrorPosY[i] + mirrorRadius) {

								isTouch[j] = 1;

							}
							else {
								isTouch[j] = 0;
							}
						}
						else {
							isTouch[j] = 0;
						}

					}
					if (lightDirection[j] == 3) {

						if (mirrorPosX[i] - mirrorRadius < lightStartPosX[j] && lightEndPosX[j] < mirrorPosX[i] + mirrorRadius) {
							if (mirrorPosY[i] - mirrorRadius < lightStartPosY[j] + lightRadius && lightEndPosY[j] - lightRadius < mirrorPosY[i] + mirrorRadius) {

								isTouch[j] = 1;

							}
							else {
								isTouch[j] = 0;
							}
						}
						else {
							isTouch[j] = 0;
						}

					}

					reset[j] = 1;
					
					if (isTouch[j] == 1) {

						DrawFormatString(0, 100, GetColor(255, 255, 255), "接触してるi = %d,j=%d", i, j, true);

						if (lightTouch[j] == 0) {
							lightContact++;
						}

						reset[j] = 0;

						//鏡に反射した後の向きを計測(鏡と光の角度から次の光の角度を設定)

						//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

						if (lightTouchMirror[j - 1] != i) {
							if (lightDirection[j] == Up) {
								if (mirrorDirection[i] == UpRight) {
									lightDirection[j + 1] = Up;
								}
								else if (mirrorDirection[i] == DownRight) {
									lightDirection[j + 1] = Right;
								}
								else if (mirrorDirection[i] == DownLeft) {
									lightDirection[j + 1] = Left;
								}
								else if (mirrorDirection[i] == UpLeft) {
									lightDirection[j + 1] = Right;
								}
							}

							else if (lightDirection[j] == Right) {
								if (mirrorDirection[i] == UpRight) {
									lightDirection[j + 1] = Right;
								}
								else if (mirrorDirection[i] == DownRight) {
									lightDirection[j + 1] = Right;
								}
								else if (mirrorDirection[i] == DownLeft) {
									lightDirection[j + 1] = Down;
								}
								else if (mirrorDirection[i] == UpLeft) {
									lightDirection[j + 1] = Up;
								}
							}

							else if (lightDirection[j] == Down) {
								if (mirrorDirection[i] == UpRight) {
									lightDirection[j + 1] = Right;
								}
								else if (mirrorDirection[i] == DownRight) {
									lightDirection[j + 1] = Down;
								}
								else if (mirrorDirection[i] == DownLeft) {
									lightDirection[j + 1] = Down;
								}
								else if (mirrorDirection[i] == UpLeft) {
									lightDirection[j + 1] = Left;
								}
							}

							else if (lightDirection[j] == Left) {
								if (mirrorDirection[i] == UpRight) {
									lightDirection[j + 1] = Up;
								}
								else if (mirrorDirection[i] == DownRight) {
									lightDirection[j + 1] = Down;
								}
								else if (mirrorDirection[i] == DownLeft) {
									lightDirection[j + 1] = Left;
								}
								else if (mirrorDirection[i] == UpLeft) {
									lightDirection[j + 1] = Left;
								}
							}
						}

						if (lightTouch[j] == 0) {
							lightTouchMirror[j] = i;
						}

						//---------1回目の反射-----------

						//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

						if (j == 0) {

							//DrawFormatString(200, 0, GetColor(255, 255, 255), "通ってる", true);

							//光の向きが縦か横で次の光の向きを計算

							if (lightDirection[j] % 2 == 0) {

								if (lightTouch[j] == 0) {
									lightOldEndPosY[j] = lightEndPosY[j];
									lightOldStartPosX[j + 1] = lightStartPosX[j + 1];
									lightOldStartPosX[j + 1] = lightStartPosY[j + 1];
								}

								lightEndPosY[j] = mirrorPosY[i];
								lightStartPosX[j + 1] = lightStartPosX[j];
								lightStartPosY[j + 1] = mirrorPosY[i];

							}
							else {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j] = lightEndPosX[j];
									lightOldStartPosX[j + 1] = lightStartPosX[j + 1];
									lightOldStartPosY[j + 1] = lightStartPosY[j + 1];
								}

								lightEndPosX[j] = mirrorPosX[i];
								lightStartPosX[j + 1] = mirrorPosX[i];
								lightStartPosY[j + 1] = lightEndPosY[j];

							}


							//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

							//次の光の向きから光を作成

							if (lightDirection[j + 1] == 0) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
								}

								lightEndPosX[j + 1] = lightStartPosX[j];
								lightEndPosY[j + 1] = startY;
							}
							else if (lightDirection[j + 1] == 1) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
								}
								lightEndPosX[j + 1] = endX;
								lightEndPosY[j + 1] = mirrorPosY[i];
							}
							else if (lightDirection[j + 1] == 2) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
								}

								lightEndPosX[j + 1] = lightStartPosX[j];
								lightEndPosY[j + 1] = endY;
							}
							else if (lightDirection[j + 1] == 3) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
									lightEndPosX[j + 1] = startX;
								}

								lightEndPosX[j + 1] = lightStartPosX[j + 2];
								lightEndPosY[j + 1] = mirrorPosY[i];
							}


							//DrawFormatString(200, 20, GetColor(255, 255, 255), "[0]sx%d,sy%d,ex%d,ey%d", lightStartPosX[0], lightStartPosY[0], lightEndPosX[0], lightEndPosY[0], true);
							//DrawFormatString(200, 40, GetColor(255, 255, 255), "[1]sx%d,sy%d,ex%d,ey%d", lightStartPosX[1], lightStartPosY[1], lightEndPosX[1], lightEndPosY[1], true);

							if (lightMax == j) {
								lightMax = j + 1;
							}

							lightTouchMirror[j] = i;

						}

						//------2回目以降の反射---------

						else if (j > 0 && j <= mirrorMax && lightTouchMirror[j - 1] != i) {

							DrawFormatString(0, 200, GetColor(255, 255, 255), "通ってる %d", j, true);

							//光の向きが縦か横で次の光の始点を計算

							if (lightDirection[j] % 2 == 0) {

								if (lightTouch[j] == 0) {
									lightOldEndPosY[j] = lightEndPosY[j];
									lightOldStartPosX[j + 1] = lightStartPosX[j + 1];
									lightOldStartPosX[j + 1] = lightStartPosY[j + 1];
								}

								lightEndPosY[j] = mirrorPosY[i];
								lightStartPosX[j + 1] = lightEndPosX[j];
								lightStartPosY[j + 1] = mirrorPosY[i];

							}
							else {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j] = lightEndPosX[j];
									lightOldStartPosX[j + 1] = lightStartPosX[j + 1];
									lightOldStartPosY[j + 1] = lightStartPosY[j + 1];
								}

								lightEndPosX[j] = mirrorPosX[i];
								lightStartPosX[j + 1] = mirrorPosX[i];
								lightStartPosY[j + 1] = lightEndPosY[j];

							}

							/*		DrawFormatString(200, 20, GetColor(255, 255, 255), "%d", lightDirection[j + 1], true);*/

									//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

									//次の光の向きから光を作成

							if (lightDirection[j + 1] == 0) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
								}

								lightEndPosX[j + 1] = mirrorPosX[i];
								lightEndPosY[j + 1] = startY;
							}
							else if (lightDirection[j + 1] == 1) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
								}
								lightEndPosX[j + 1] = endX;
								lightEndPosY[j + 1] = mirrorPosY[i];
							}
							else if (lightDirection[j + 1] == 2) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
								}

								lightEndPosX[j + 1] = lightEndPosX[j];
								lightEndPosY[j + 1] = endY;
							}
							else if (lightDirection[j + 1] == 3) {

								if (lightTouch[j] == 0) {
									lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
									lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
								}

								lightEndPosX[j + 1] = startX;
								lightEndPosY[j + 1] = mirrorPosY[i];
							}

							lightMax = j + 1;
							lightTouchMirror[j] = i;

						}

						lightTouch[j] = 1;

						//if (lightDirection[j] != lightOldDirection[j]) {

						//	lightTouch[j] = 0;

						//}

						DrawFormatString(0, 300, GetColor(255, 255, 255), "lightTouchMirror[1] %d", lightTouchMirror[2], true);

					}

					//入って出たときにリセットする

					if (reset[j] == 1 && lightTouch[j] == 1 && lightTouchMirror[j] == i) {

						lightStartPosX[j] = lightOldStartPosX[j];
						lightStartPosY[j] = lightOldStartPosY[j];

						lightEndPosX[j] = lightOldEndPosX[j];
						lightEndPosY[j] = lightOldEndPosY[j];

						lightStartPosX[j + 1] = lightOldStartPosX[j + 1];
						lightStartPosY[j + 1] = lightOldStartPosY[j + 1];

						lightEndPosX[j + 1] = lightOldEndPosX[j + 1];
						lightEndPosY[j + 1] = lightOldEndPosY[j + 1];

						lightTouch[j] = 0;

						lightContact--;

						lightMax = j;

						isTouch[j] = 0; 

						lightTouchMirror[j] = 100;

						DrawFormatString(0, 300, GetColor(255, 255, 255), "通ってる", true);

					}

				}

			}

			//↓後でマップチップ使う

			//光と自機の接触

			for (int j = 0; j <= lightMax; j++) {

				//if (lightDirection[j] % 2 == 0) {
				//	if (lightStartPosX[j] - lightRadius < playerPosX + playerRadius && playerPosX - playerRadius < lightEndPosX[j] + lightRadius) {
				//		if (lightStartPosY[j] < playerPosY + playerRadius && playerPosY - playerRadius < lightEndPosY[j]) {

				//			stageReset = 0;

				//		}
				//	}
				//}
				//else {
				//	if (lightStartPosX[j] < playerPosX + playerRadius && playerPosX - playerRadius < lightEndPosX[j]) {
				//		if (lightStartPosY[j] - lightRadius < playerPosY + playerRadius && playerPosY - playerRadius < lightEndPosY[j] + lightRadius) {

				//			stageReset = 0;

				//		}
				//	}
				//}

				if (lightDirection[j] == 0) {

					if (playerPosX - playerRadius < lightStartPosX[j] + lightRadius && lightEndPosX[j] - lightRadius < playerPosX + playerRadius) {
						if (playerPosY - playerRadius < lightStartPosY[j] && lightEndPosY[j] < playerPosY + playerRadius) {

							stageReset = 0;

						}
					}
				}
				if (lightDirection[j] == 1) {

					if (playerPosX - playerRadius < lightEndPosX[j] && lightStartPosX[j] < playerPosX + playerRadius) {
						if (playerPosY - playerRadius < lightEndPosY[j] + lightRadius && lightStartPosY[j] - lightRadius < playerPosY + playerRadius) {

							stageReset = 0;

						}
					}
				}
				if (lightDirection[j] == 2) {

					if (playerPosX - playerRadius < lightEndPosX[j] + lightRadius && lightStartPosX[j] - lightRadius < playerPosX + playerRadius) {
						if (playerPosY - playerRadius < lightEndPosY[j] && lightStartPosY[j] < playerPosY + playerRadius) {

							stageReset = 0;

						}
					}
				}
				if (lightDirection[j] == 3) {

					if (playerPosX - playerRadius < lightStartPosX[j] && lightEndPosX[j] < playerPosX + playerRadius) {
						if (playerPosY - playerRadius < lightStartPosY[j] + lightRadius && lightEndPosY[j] - lightRadius < playerPosY + playerRadius) {

							stageReset = 0;

						}
					}
				}
			}


			//ゴール接触時

			//光

			for (int j = 0; j <= lightMax; j++) {

				if (lightDirection[j] == 0) {

					if (goalPosX - goalRadius < lightStartPosX[j] + lightRadius && lightEndPosX[j] - lightRadius < goalPosX + goalRadius) {
						if (goalPosY - goalRadius < lightStartPosY[j] && lightEndPosY[j] < goalPosY + goalRadius) {

							isGoal = 1;

						}
						else {
							isGoal = 0;
						}
					}
					else {
						isGoal = 0;
					}
				}
				if (lightDirection[j] == 1) {

					if (goalPosX - goalRadius < lightEndPosX[j] && lightStartPosX[j] < goalPosX + goalRadius) {
						if (goalPosY - goalRadius < lightEndPosY[j] + lightRadius && lightStartPosY[j] - lightRadius < goalPosY + goalRadius) {

							isGoal = 1;

						}
						else {
							isGoal = 0;
						}
					}
					else {
						isGoal = 0;
					}

				}
				if (lightDirection[j] == 2) {

					if (goalPosX - goalRadius < lightEndPosX[j] + lightRadius && lightStartPosX[j] - lightRadius < goalPosX + goalRadius) {
						if (goalPosY - goalRadius < lightEndPosY[j] && lightStartPosY[j] < goalPosY + goalRadius) {

							isGoal = 1;

						}
						else {
							isGoal = 0;
						}
					}
					else {
						isGoal = 0;
					}

				}
				if (lightDirection[j] == 3) {

					if (goalPosX - goalRadius < lightStartPosX[j] && lightEndPosX[j] < goalPosX + goalRadius) {
						if (goalPosY - goalRadius < lightStartPosY[j] + lightRadius && lightEndPosY[j] - lightRadius < goalPosY + goalRadius) {

							isGoal = 1;

						}
						else {
							isGoal = 0;
						}
					}
					else {
						isGoal = 0;
					}

				}

			}

			//自機

			if (playerPosX - playerRadius < goalPosX + goalRadius && goalPosX - goalRadius < playerPosX + playerRadius) {
				if (playerPosY - playerRadius < goalPosY + goalRadius && goalPosY - goalRadius < playerPosY + playerRadius) {

					if (isGoal == 1) {
						stage = 0;
						//DrawFormatString(0, 600, GetColor(255, 255, 255), "通ってる", true);
					}

				}
			}
		}

		// 描画処理

		if (stage == 0) {
			DrawFormatString(WIN_WIDTH / 2, WIN_HEIGHT / 2, GetColor(255, 255, 255), "ENTER START", true);
		}
		else {

			//ゴール

			if (isGoal == 0) {
				DrawBox(goalPosX + goalRadius, goalPosY + goalRadius, goalPosX - goalRadius, goalPosY - goalRadius, GetColor(255, 0, 0), true);

			}

			//枠

			for (int i = 0; i <= WIN_HEIGHT - (outY * 2); i += BLOCKSIZE) {
				DrawLine(startX, startY + i, endX, startY + i, GetColor(255, 255, 255), true);
			}
			for (int i = 0; i <= WIN_WIDTH - (outX * 2); i += BLOCKSIZE) {
				DrawLine(startX + i, startY, startX + i, endY, GetColor(255, 255, 255), true);
			}

			//背景の明暗

			//if (isGoal == 0) {

			//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);
			//	DrawExtendGraph(0, 0, WIN_WIDTH, WIN_HEIGHT, blackGH, true);

			//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			//	DrawExtendGraph(playerPosX + (playerRadius * 2), playerPosY + (playerRadius * 2),
			//					playerPosX - (playerRadius * 2), playerPosY - (playerRadius * 2),
			//					whiteGH, true);
			//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			//}


			//光

			for (int i = 0; i <= lightMax; i++) {

				if (lightDirection[i] % 2 == 0) {
					DrawBox(lightStartPosX[i] - lightRadius, lightStartPosY[i], lightEndPosX[i] + lightRadius, lightEndPosY[i], GetColor(255, 255, 255), true);
				}
				else {
					DrawBox(lightStartPosX[i], lightStartPosY[i] - lightRadius, lightEndPosX[i], lightEndPosY[i] + lightRadius, GetColor(255, 255, 255), true);
				}

			}


			//移動範囲

			for (int i = 0; i <= mirrorMax; i++) {
				DrawBox(mirrorRangeUpX[i], mirrorRangeUpY[i], mirrorRangeDownX[i], mirrorRangeDownY[i], GetColor(255, 0, 0), false);
			}

			//鏡

			for (int i = 0; i <= mirrorMax; i++) {
				DrawRotaGraph(mirrorPosX[i], mirrorPosY[i], 1.0, mirrorAngle[i], mirrorGH, TRUE);
			}

			//DrawFormatString(0, 20, GetColor(255, 255, 255), "%d", mirrorDirection[0], true);
			//DrawFormatString(0, 40, GetColor(255, 255, 255), "%d", mirrorDirection[1], true);
			//DrawFormatString(60, 60, GetColor(255, 255, 255), "%d", lightTouch[0], true);

			for (int i = 0; i <= lightMax; i++) {
				DrawFormatString(lightStartPosX[i], lightStartPosY[i], GetColor(255, 0, 0), "%dS", i);
				DrawFormatString(lightEndPosX[i], lightEndPosY[i], GetColor(255, 0, 0), "%dE", i);
			}

			DrawFormatString(0, 0, GetColor(255, 255, 255), "見えないゴールに光を当ててプレイヤーを持っていけばクリア", true);
			DrawFormatString(0, 20, GetColor(255, 255, 255), "WASD :鏡移動 ↑→↓← : プレイヤー移動 SPASE : 鏡回転（時計回り）", true);
			DrawFormatString(0, 40, GetColor(255, 255, 255), "lightContact:%d", lightContact, true);
			DrawFormatString(0, 60, GetColor(255, 255, 255), "lightMax:%d", lightMax, true);

			//自機

			DrawCircle(playerPosX, playerPosY, playerRadius, GetColor(255, 255, 255), true);

		}

		if (isGoal != 0) {
			DrawBox(goalPosX + goalRadius, goalPosY + goalRadius, goalPosX - goalRadius, goalPosY - goalRadius, GetColor(0, 0, 255), true);
		}

		//---------  ここまでにプログラムを記述  ---------//
		// (ダブルバッファ)裏面
		ScreenFlip();

		// 20ミリ秒待機(疑似60FPS)
		WaitTimer(20);

		// Windowsシステムからくる情報を処理する
		if (ProcessMessage() == -1) {
			break;
		}

		// ESCキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}
