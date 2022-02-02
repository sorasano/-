#include "DxLib.h"
#include "mirror.h"

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

float easeOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1) {
		return n1 * x * x;
	}
	else if (x < 2 / d1) {
		return n1 * (x -= 1.5 / d1) * x + 0.75;
	}
	else if (x < 2.5 / d1) {
		return n1 * (x -= 2.25 / d1) * x + 0.9375;
	}
	else {
		return n1 * (x -= 2.625 / d1) * x + 0.984375;
	}
}

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

	int clearGH = LoadGraph("clear.png");
	int stageSerectGH = LoadGraph("stageSerect.png");
	int goalGH = LoadGraph("goal.png");

	int titlegh = LoadGraph("title2.png");
	int backgh = LoadGraph("back.png");
	int space[6];
	LoadDivGraph("space.png", 6, 6, 1, 896, 896, space);

	int tutorial1Gh[2];
	LoadDivGraph("tutorial1.png", 2, 2, 1, 800, 128, tutorial1Gh);

	int tutorial2Gh[8];
	LoadDivGraph("tutorial2.png", 8, 8, 1, 800, 128, tutorial2Gh);

	int tutorial3Gh[9];
	LoadDivGraph("tutorial3.png", 9, 9, 1, 800, 128, tutorial3Gh);

	// ゲームループで使う変数の宣言

	//マップチップ

	int map[10][10] = {

	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},
	{ 1,1,1,1,1,1,1,1,1,1},

	};

	int playerMapX = 0;
	int playerMapY = 0;

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
	int playerRadius = 28;
	int playerSpeed = 3;
	int playerOldPosX = 0;
	int playerOldPosY = 0;

	//鏡

	//鏡の最大値
	const int MIRROR_MAX = 15;
	int mirrorMax = 0;//ステージごとに変える用

	int mirrorPosX[MIRROR_MAX] = {};
	int mirrorPosY[MIRROR_MAX] = {};

	int mirrorOldPosX[MIRROR_MAX] = {};
	int mirrorOldPosY[MIRROR_MAX] = {};

	int mirrorRadius = 32;//鏡半径

	int mirrorDirection[MIRROR_MAX] = {};//0↗ 1↘ 2↙ 3↖

	//移動範囲

	int mirrorRangeUpX[MIRROR_MAX] = {};//移動範囲の左上の座標
	int mirrorRangeUpY[MIRROR_MAX] = {};

	int mirrorRangeDownX[MIRROR_MAX] = {};//移動範囲の右下の座標
	int mirrorRangeDownY[MIRROR_MAX] = {};

	//光

	//光の最大個数
	const int LIGHT_MAX = 10;
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

	int stage = 0;//0,なし123,チュートリアル,45678,メインステージ
	int stageReset = 0;
	int stagePick = 1;

	int scene = 0;//0タイトル,1選択画面,2ゲーム画面

	const int STAGE_MAX = 3;

	int stagePosX[STAGE_MAX][STAGE_MAX] = {};
	int stagePosY[STAGE_MAX][STAGE_MAX] = {};

	int stageRadiusX = 100;
	int stageRadiusY = 100;

	for (int y = 0; y < STAGE_MAX; y++) {
		for (int x = 0; x < STAGE_MAX; x++) {
			stagePosX[x][y] = (800 / 3 * x) + 140;
			stagePosY[x][y] = (800 / 3 * y) + 140;
		}
	}

	//ゴール

	int goalPosX = 0;
	int goalPosY = 0;

	int goalRadius = 32;
	int isGoal = 0;//0 ゴール不可能 ゴール可能

	//キャラチェン

	int charaChange = 0;

	//ブロック

	const int BLOCK_MAX = 100;
	int blockMax = 1;

	int blockPosX[BLOCK_MAX] = {};
	int blockPosY[BLOCK_MAX] = {};
	int blockRadius = 32;

	//タイトル

		//タイトル
	int spaceMove = 0;
	int ismove = 1;
	float frame = 0.0f;
	float endframe = 250.0f;
	float start = -386.0f;
	float end = 100.0f;
	int titleX = 56;
	int titleY = 0;

	//光変更
	int changeMode = 0;
	int changeBack = 0;
	int changeCount = 0;
	int alpha = 200;
	int changeX = WIN_WIDTH / 2;
	int changeY = WIN_HEIGHT / 2;
	int changeR = 0;
	int changeCoolTime = 0;
	int isChange = 0;

	//チュートリアル

	int tutorialPage = 0;
	const int TUTORIALTIME = 100;
	int tutorialTimer = 0;
	int tutorialTouchLight = 0;

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

		//タイトル
		if (scene == 0) {

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				if (changeCount == 0) {
					changeCount = 1;
				}
			}

			if (frame == endframe) {
				ismove = 0;
			}
			if (ismove == 1) {
				frame += 1;
			}
			if (spaceMove < 50) {
				spaceMove += 1;
			}
			if (spaceMove == 50) {
				spaceMove = 0;
			}
			//イージング
			titleY = start + (end - start) * easeOutBounce(frame / endframe);

			//だんだん大きくなる
			if (changeCount == 1 && changeBack == 0) {
				changeR += 10;
			}

			//戻る
			if (changeR >= 1000 && changeBack == 0) {
				//ここでシーン切り替え
				scene = 1;
				stagePick = 1;
				stageReset = 0;
				changeBack = 1;
				scene = 1;
				////////////////////
			}

		}

		//ステージ選択
		if (scene == 1) {

			//タイトル
			if (changeBack == 1) {
				changeR -= 10;
			}
			if (changeR <= 0 && changeBack == 1) {
				changeBack = 0;
				changeCount = 0;
				changeR = 0;
				isChange = 1;
			}

			//ステージ選択
			if (stagePick == 1) {
				if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 2;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 4;
				}
			}

			else if (stagePick == 2) {
				if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 1;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 5;
				}
				else if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 3;
				}
			}

			else if (stagePick == 3) {
				if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 2;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 6;
				}
			}

			else if (stagePick == 4) {

				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 1;
				}
				if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 5;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 7;
				}
			}

			else if (stagePick == 5) {

				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 2;
				}
				else if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 4;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 8;
				}
				else if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 6;
				}
			}

			else if (stagePick == 6) {
				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 3;
				}
				else if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 5;
				}
				else if (keys[KEY_INPUT_S] == 1 && oldkeys[KEY_INPUT_S] == 0) {
					stagePick = 9;
				}
			}

			else if (stagePick == 7) {

				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 4;
				}
				if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 8;
				}

			}

			else if (stagePick == 8) {

				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 5;
				}
				else if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 7;
				}
				else if (keys[KEY_INPUT_D] == 1 && oldkeys[KEY_INPUT_D] == 0) {
					stagePick = 9;
				}
			}
			else if (stagePick == 9) {
				if (keys[KEY_INPUT_W] == 1 && oldkeys[KEY_INPUT_W] == 0) {
					stagePick = 6;
				}
				else if (keys[KEY_INPUT_A] == 1 && oldkeys[KEY_INPUT_A] == 0) {
					stagePick = 8;
				}

			}

			//決定

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {

				if (stagePick == 9) {
					scene = 0;
				}
				else if (stagePick != 9 && stagePick != 0) {
					stage = stagePick;
					stageReset = 0;
					scene = 2;
				}
				else {
					stagePick = 1;
				}

			}

		}

		if (scene == 2) {

			//ステージ移動時初期化
			if (stageReset == 0) {

				lightMax = 0;
				mirrorMax = 0;
				blockMax = 0;

				lightContact = 0;
				charaChange = 0;

				//ステージ1

				if (stage == 1) {

					stageReset = 1;
					tutorialPage = 0;
					tutorialTimer = 0;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = 672;
					lightStartPosY[0] = startY;
					lightEndPosX[0] = 672;
					lightEndPosY[0] = endY;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 2;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 0;
					//鏡の初期値
					mirrorPosX[0] = 0;
					mirrorPosY[0] = 0;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius;
					//最初の鏡の向き
					mirrorDirection[0] = 1;

					//-------ゴールの初期位置------
					goalPosX = 672;
					goalPosY = 672;

					//-----プレイヤーの初期位置------
					playerPosX = 224;
					playerPosY = 224;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 0;
					blockPosX[0] = 0;
					blockPosY[0] = 0;

				}
				else if (stage == 2) {

					stageReset = 1;
					tutorialPage = 0;
					tutorialTimer = 0;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = 672;
					lightStartPosY[0] = startY;
					lightEndPosX[0] = 672;
					lightEndPosY[0] = endY;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 2;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 0;
					//鏡の初期値
					mirrorPosX[0] = 352;
					mirrorPosY[0] = 352;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius + (BLOCKSIZE * 5);//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius + (BLOCKSIZE * 2);
					//最初の鏡の向き
					mirrorDirection[0] = 3;

					//-------ゴールの初期位置------
					goalPosX = 672;
					goalPosY = 672;

					//-----プレイヤーの初期位置------
					playerPosX = 224;
					playerPosY = 224;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 0;
					blockPosX[0] = 0;
					blockPosY[0] = 0;

				}
				else if (stage == 3) {

					stageReset = 1;
					tutorialPage = 0;
					tutorialTimer = 0;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = startX;
					lightStartPosY[0] = 416;
					lightEndPosX[0] = endX;
					lightEndPosY[0] = 416;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 1;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 0;
					//鏡の初期値
					mirrorPosX[0] = 288;
					mirrorPosY[0] = 416;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius + (BLOCKSIZE * 6);//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius;
					//最初の鏡の向き
					mirrorDirection[0] = 1;

					//-------ゴールの初期位置------
					goalPosX = 672;
					goalPosY = 672;

					//-----プレイヤーの初期位置------
					playerPosX = 224;
					playerPosY = 224;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 0;
					blockPosX[0] = 0;
					blockPosY[0] = 0;
				}

				else if (stage == 4) {

					stageReset = 1;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = endX;
					lightStartPosY[0] = endY - 32;
					lightEndPosX[0] = startX;
					lightEndPosY[0] = endY - 32;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 3;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 0;
					//鏡の初期値
					mirrorPosX[0] = 416;
					mirrorPosY[0] = 736;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius;
					//最初の鏡の向き
					mirrorDirection[0] = 1;

					//-------ゴールの初期位置------
					goalPosX = 416;
					goalPosY = 160;

					//-----プレイヤーの初期位置------
					playerPosX = 160;
					playerPosY = 672;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 0;
					blockPosX[0] = 0;
					blockPosY[0] = 0;

				}
				else if (stage == 5) {

					stageReset = 1;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
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

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 1;
					//鏡の初期値
					mirrorPosX[0] = WIN_WIDTH / 2 - BLOCKSIZE * 2;
					mirrorPosY[0] = WIN_HEIGHT / 2;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - (BLOCKSIZE * 1);//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - (BLOCKSIZE * 2);
					mirrorRangeDownX[0] = mirrorPosX[0] + (BLOCKSIZE * 3);//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + (BLOCKSIZE * 2);
					//最初の鏡の向き
					mirrorDirection[0] = 2;

					//鏡[1]
					mirrorPosX[1] = WIN_WIDTH / 2 + BLOCKSIZE * 2;
					mirrorPosY[1] = WIN_HEIGHT / 2;

					mirrorRangeUpX[1] = mirrorPosX[1] - (BLOCKSIZE * 1);//移動範囲の左上の座標
					mirrorRangeUpY[1] = mirrorPosY[1] - (BLOCKSIZE * 2);

					mirrorRangeDownX[1] = mirrorPosX[1] + (BLOCKSIZE * 3);//移動範囲の右下の座標
					mirrorRangeDownY[1] = mirrorPosY[1] + (BLOCKSIZE * 2);

					mirrorDirection[1] = 0;

					//-------ゴールの初期位置------
					goalPosX = endX - goalRadius;
					goalPosY = endY - goalRadius;

					//-----プレイヤーの初期位置------
					playerPosX = startX + playerRadius;
					playerPosY = startY + playerRadius;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 2;
					blockPosX[0] = 608;
					blockPosY[0] = 608;
					blockPosX[1] = 608;
					blockPosY[1] = 608 + 64;
					blockPosX[2] = 608;
					blockPosY[2] = 608 + 128;

				}
				else if (stage == 6) {

					stageReset = 1;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = 736;
					lightStartPosY[0] = startY;
					lightEndPosX[0] = 736;
					lightEndPosY[0] = endY;

					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 2;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 0;
					//鏡の初期値
					mirrorPosX[0] = 736;
					mirrorPosY[0] = 736;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius - (BLOCKSIZE * 9);
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius;
					//最初の鏡の向き
					mirrorDirection[0] = 3;

					//-------ゴールの初期位置------
					goalPosX = 480;
					goalPosY = 288;

					//-----プレイヤーの初期位置------
					playerPosX = 224;
					playerPosY = 672;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 10;
					blockPosX[0] = 160;
					blockPosY[0] = 480;
					blockPosX[1] = 224;
					blockPosY[1] = 480;
					blockPosX[2] = 288;
					blockPosY[2] = 480;
					blockPosX[3] = 352;
					blockPosY[3] = 480;
					blockPosX[4] = 416;
					blockPosY[4] = 480;
					blockPosX[5] = 480;
					blockPosY[5] = 480;
					blockPosX[6] = 480;
					blockPosY[6] = 416;
					blockPosX[7] = 480;
					blockPosY[7] = 352;

				}
				else if (stage == 7) {

					stageReset = 1;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる
					//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = 416;
					lightStartPosY[0] = startY;
					lightEndPosX[0] = 416;
					lightEndPosY[0] = endY;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 2;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 9;
					//---鏡の初期値-----
					mirrorPosX[1] = 160;
					mirrorPosY[1] = 352;
					//移動範囲
					mirrorRangeUpX[1] = mirrorPosX[1] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[1] = mirrorPosY[1] - mirrorRadius;
					mirrorRangeDownX[1] = mirrorPosX[1] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[1] = mirrorPosY[1] + mirrorRadius;
					//最初の鏡の向き
					mirrorDirection[1] = 2;

					//----鏡[2]-----
					mirrorPosX[2] = 160;
					mirrorPosY[2] = 736;

					mirrorRangeUpX[2] = mirrorPosX[2] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[2] = mirrorPosY[2] - mirrorRadius;

					mirrorRangeDownX[2] = mirrorPosX[2] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[2] = mirrorPosY[2] + mirrorRadius;

					mirrorDirection[2] = 1;

					//----鏡[3]-----
					mirrorPosX[3] = 288;
					mirrorPosY[3] = 736;

					mirrorRangeUpX[3] = mirrorPosX[3] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[3] = mirrorPosY[3] - mirrorRadius;

					mirrorRangeDownX[3] = mirrorPosX[3] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[3] = mirrorPosY[3] + mirrorRadius;

					mirrorDirection[3] = 0;

					//----鏡[4]-----
					mirrorPosX[4] = 288;
					mirrorPosY[4] = 544;

					mirrorRangeUpX[4] = mirrorPosX[4] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[4] = mirrorPosY[4] - mirrorRadius;

					mirrorRangeDownX[4] = mirrorPosX[4] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[4] = mirrorPosY[4] + mirrorRadius;

					mirrorDirection[4] = 2;

					//----鏡[8]-----
					mirrorPosX[8] = 544;
					mirrorPosY[8] = 480;

					mirrorRangeUpX[8] = mirrorPosX[8] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[8] = mirrorPosY[8] - mirrorRadius;

					mirrorRangeDownX[8] = mirrorPosX[8] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[8] = mirrorPosY[8] + mirrorRadius;

					mirrorDirection[8] = 2;

					//----鏡[7]-----
					mirrorPosX[7] = 544;
					mirrorPosY[7] = 736;

					mirrorRangeUpX[7] = mirrorPosX[7] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[7] = mirrorPosY[7] - mirrorRadius;

					mirrorRangeDownX[7] = mirrorPosX[7] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[7] = mirrorPosY[7] + mirrorRadius;

					mirrorDirection[7] = 0;

					//----鏡[6]-----

					mirrorPosX[6] = 736;
					mirrorPosY[6] = 736;

					mirrorRangeUpX[6] = mirrorPosX[6] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[6] = mirrorPosY[6] - mirrorRadius;

					mirrorRangeDownX[6] = mirrorPosX[6] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[6] = mirrorPosY[6] + mirrorRadius;

					mirrorDirection[6] = 3;

					//----鏡[5]-----

					mirrorPosX[5] = 736;
					mirrorPosY[5] = 352;

					mirrorRangeUpX[5] = mirrorPosX[5] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[5] = mirrorPosY[5] - mirrorRadius;

					mirrorRangeDownX[5] = mirrorPosX[5] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[5] = mirrorPosY[5] + mirrorRadius;

					mirrorDirection[5] = 2;

					//----鏡[0]-----

					mirrorPosX[0] = 352;
					mirrorPosY[0] = 352;

					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;

					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius + BLOCKSIZE;//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius;

					mirrorDirection[0] = 0;

					//----鏡[9]-----

					mirrorPosX[9] = 352;
					mirrorPosY[9] = 544;

					mirrorRangeUpX[9] = mirrorPosX[9] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[9] = mirrorPosY[9] - mirrorRadius;

					mirrorRangeDownX[9] = mirrorPosX[9] + mirrorRadius + BLOCKSIZE;//移動範囲の右下の座標
					mirrorRangeDownY[9] = mirrorPosY[9] + mirrorRadius;

					mirrorDirection[9] = 0;

					//-------ゴールの初期位置------
					goalPosX = 416;
					goalPosY = 480;

					//-----プレイヤーの初期位置------
					playerPosX = 416;
					playerPosY = 736;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 8;
					blockPosX[0] = 352;
					blockPosY[0] = 416;
					blockPosX[1] = 416;
					blockPosY[1] = 416;
					blockPosX[2] = 480;
					blockPosY[2] = 416;
					blockPosX[3] = 352;
					blockPosY[3] = 736;
					blockPosX[4] = 352;
					blockPosY[4] = 672;
					blockPosX[5] = 352;
					blockPosY[5] = 608;
					blockPosX[6] = 480;
					blockPosY[6] = 736;
					blockPosX[7] = 480;
					blockPosY[7] = 672;
					blockPosX[8] = 480;
					blockPosY[8] = 608;
				}
				else if (stage == 8) {
					stageReset = 1;

					//初期位置計算方法

					//左上を0ブロック目として設置したい場所をxとすると,64 * x + 160 を縦横それぞれやる
					//0↗ 1↘ 2↙ 3↖(mirror)//0↑　1→　2↓　3←(light)

					//----------光--------------------
					//最初の光の座標
					lightStartPosX[0] = 160;
					lightStartPosY[0] = startY;
					lightEndPosX[0] = 160;
					lightEndPosY[0] = endY;
					//初期座標の保存
					lightOldStartPosX[0] = lightStartPosX[0];
					lightOldStartPosY[0] = lightStartPosY[0];
					lightOldEndPosX[0] = lightEndPosX[0];
					lightOldEndPosY[0] = lightEndPosY[0];
					//光初期向き
					lightDirection[0] = 2;

					//----------鏡--------------
					//出てくる鏡の個数-1(1個なら0、2個なら1)
					mirrorMax = 5;
					//---鏡の初期値-----
					mirrorPosX[0] = 160;
					mirrorPosY[0] = 480;
					//移動範囲
					mirrorRangeUpX[0] = mirrorPosX[0] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[0] = mirrorPosY[0] - mirrorRadius;
					mirrorRangeDownX[0] = mirrorPosX[0] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[0] = mirrorPosY[0] + mirrorRadius + (BLOCKSIZE * 2);
					//最初の鏡の向き
					mirrorDirection[0] = 0;

					//----鏡[1]-----
					mirrorPosX[1] = 608;
					mirrorPosY[1] = 480;

					mirrorRangeUpX[1] = mirrorPosX[1] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[1] = mirrorPosY[1] - mirrorRadius;

					mirrorRangeDownX[1] = mirrorPosX[1] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[1] = mirrorPosY[1] + mirrorRadius + (BLOCKSIZE * 2);

					mirrorDirection[1] = 3;

					//----鏡[2]-----
					mirrorPosX[2] = 608;
					mirrorPosY[2] = 352;

					mirrorRangeUpX[2] = mirrorPosX[2] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[2] = mirrorPosY[2] - mirrorRadius;

					mirrorRangeDownX[2] = mirrorPosX[2] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[2] = mirrorPosY[2] + mirrorRadius + (BLOCKSIZE * 2);

					mirrorDirection[2] = 2;

					//----鏡[3]-----
					mirrorPosX[3] = 224;
					mirrorPosY[3] = 352;

					mirrorRangeUpX[3] = mirrorPosX[3] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[3] = mirrorPosY[3] - mirrorRadius;

					mirrorRangeDownX[3] = mirrorPosX[3] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[3] = mirrorPosY[3] + mirrorRadius + (BLOCKSIZE * 2);

					mirrorDirection[3] = 1;

					//----鏡[4]-----
					mirrorPosX[4] = 416;
					mirrorPosY[4] = 608;

					mirrorRangeUpX[4] = mirrorPosX[4] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[4] = mirrorPosY[4] - mirrorRadius;

					mirrorRangeDownX[4] = mirrorPosX[4] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[4] = mirrorPosY[4] + mirrorRadius;

					mirrorDirection[4] = 2;

					//----鏡[5]-----
					mirrorPosX[5] = 416;
					mirrorPosY[5] = 736;

					mirrorRangeUpX[5] = mirrorPosX[5] - mirrorRadius;//移動範囲の左上の座標
					mirrorRangeUpY[5] = mirrorPosY[5] - mirrorRadius;

					mirrorRangeDownX[5] = mirrorPosX[5] + mirrorRadius;//移動範囲の右下の座標
					mirrorRangeDownY[5] = mirrorPosY[5] + mirrorRadius;

					mirrorDirection[5] = 0;

					//-------ゴールの初期位置------
					goalPosX = 736;
					goalPosY = 736;

					//-----プレイヤーの初期位置------
					playerPosX = 416;
					playerPosY = 416;

					//-----------ブロックの位置----------
					//出てくるブロックの個数 - 1(1個なら0、2個なら1)
					blockMax = 6;
					blockPosX[0] = 736;
					blockPosY[0] = 288;
					blockPosX[1] = 672;
					blockPosY[1] = 288;
					blockPosX[2] = 608;
					blockPosY[2] = 288;
					blockPosX[3] = 544;
					blockPosY[3] = 288;
					blockPosX[4] = 480;
					blockPosY[4] = 288;
					blockPosX[5] = 416;
					blockPosY[5] = 288;
					blockPosX[6] = 352;
					blockPosY[6] = 288;

				}

			}

			if (scene == 2) {

				if (keys[KEY_INPUT_R] == 1 && oldkeys[KEY_INPUT_R] == 0) {
					scene = 1;
					stagePick = 1;
					stage = 0;
					stageReset = 0;
				}

				if (keys[KEY_INPUT_C] == 1 && oldkeys[KEY_INPUT_C] == 0) {

					if (charaChange == 0) {
						charaChange = 1;
					}
					else {
						charaChange = 0;
					}

				}


				if (charaChange == 1) {

					for (int i = 0; i <= mirrorMax; i++) {
						mirrorOldPosX[i] = mirrorPosX[i];
						mirrorOldPosY[i] = mirrorPosY[i];

					}

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
				}
				else {

					playerOldPosX = playerPosX;
					playerOldPosY = playerPosY;

					if (keys[KEY_INPUT_W] == 1) {

						if (playerPosY > startY + playerRadius) {
							playerPosY -= playerSpeed;
						}

					}
					if (keys[KEY_INPUT_S] == 1) {

						if (playerPosY < endY - playerRadius) {
							playerPosY += playerSpeed;
						}


					}

					if (keys[KEY_INPUT_A] == 1) {

						if (playerPosX > startX + playerRadius) {
							playerPosX -= playerSpeed;
						}

					}
					if (keys[KEY_INPUT_D] == 1) {

						if (playerPosX < endX - playerRadius) {
							playerPosX += playerSpeed;
						}

					}

					playerMapX = (playerPosX - startX) / BLOCKSIZE;
					playerMapY = (playerPosY - startY) / BLOCKSIZE;

				}


				//----------鏡------------

				//角度変更

				if (charaChange == 1) {
					if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {

						for (int i = 0; i <= mirrorMax; i++) {
							mirrorDirection[i]++;

							if (mirrorDirection[i] > 3) {
								mirrorDirection[i] = 0;
							}
						}

					}
				}

				//-------チュートリアル-------

				if (stage == 1) {

					//キャラチェン出来ない
					if (keys[KEY_INPUT_C] == 1) {
						charaChange = 0;
					}

					//wasd入力で次
					if (tutorialPage == 0) {
						if (keys[KEY_INPUT_W] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_A] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_S] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_D] == 1) {
							tutorialPage++;
						}
					}

				}
				else if (stage == 2) {

					if (tutorialPage == 0) {
						if (keys[KEY_INPUT_C] == 1) {
							tutorialPage++;
						}
					}
					else if (tutorialPage == 1) {
						if (keys[KEY_INPUT_W] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_A] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_S] == 1) {
							tutorialPage++;
						}
						else if (keys[KEY_INPUT_D] == 1) {
							tutorialPage++;
						}
					}
					else if (tutorialPage == 2) {

						tutorialTimer++;
						if (tutorialTimer > TUTORIALTIME) {
							tutorialTimer = 0;
							tutorialPage++;
						}

					}
					else if (tutorialPage == 3) {

						if (tutorialTouchLight == 1) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 4) {

						if (keys[KEY_INPUT_SPACE] == 1) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 5) {

						if (isGoal == 1) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 6) {

						if (isGoal == 0) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 7) {

						if (isGoal == 1) {
							tutorialPage = 6;
						}

					}

				}
				else if (stage == 3) {

					if (tutorialPage == 0) {

						tutorialTimer++;
						if (tutorialTimer > TUTORIALTIME) {
							tutorialTimer = 0;
							tutorialPage++;
						}

					}
					else if (tutorialPage == 1) {

						if (isGoal == 1) {
							tutorialPage++;
						}

						tutorialTimer++;
						if (tutorialTimer > TUTORIALTIME) {
							tutorialTimer = 0;
							tutorialPage = 5;
						}

					}
					else if (tutorialPage == 2) {

						tutorialTimer++;

						if (tutorialTimer > TUTORIALTIME) {
							tutorialTimer = 0;
							tutorialPage++;
						}

					}
					else if (tutorialPage == 3) {

						if (isGoal == 0) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 4) {

						if (isGoal == 1) {
							tutorialPage = 3;
						}

					}
					else if (tutorialPage == 5) {

						if (isGoal == 1) {
							tutorialPage = 3;
						}

						if (keys[KEY_INPUT_F] == 1) {
							tutorialPage++;
						}

					}
					else if (tutorialPage == 6) {

						if (keys[KEY_INPUT_C] == 1) {
							tutorialPage++;
						}
						else {
							if (isChange == 1) {
								tutorialPage++;
							}
						}
						if (isGoal == 1) {
							tutorialPage = 5;
						}
					}
					else if (tutorialPage == 7) {

						if (mirrorPosX[0] >= goalPosX) {
							tutorialPage++;
						}
						if (isGoal == 1) {
							tutorialPage = 5;
						}
					}
					else if (tutorialPage == 8) {
						if (isGoal == 1) {
							tutorialPage = 5;
						}
					}

				}



				//-----------光-----------

				//光の反射

				//鏡と光の当たり判定

				for (int i = 0; i <= mirrorMax; i++) {
					for (int j = 0; j <= lightMax; j++) {


						//DrawFormatString(0, 120, GetColor(255, 255, 255), "i = %d,j=%d", i, j, true);

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

						if (stage == 2) {
							if (isTouch[j] == 1) {
								tutorialTouchLight = 1;
							}
							else {
								tutorialTouchLight = 0;
							}
						}

						if (isTouch[j] == 1) {

							//DrawFormatString(0, 100, GetColor(255, 255, 255), "接触してるi = %d,j=%d", i, j, true);

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
										lightDirection[j + 1] = Up;
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

									lightEndPosX[j + 1] = lightStartPosX[j + 1];
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

									lightEndPosX[j + 1] = lightStartPosX[j + 1];
									lightEndPosY[j + 1] = endY;
								}
								else if (lightDirection[j + 1] == 3) {

									if (lightTouch[j] == 0) {
										lightOldEndPosX[j + 1] = lightEndPosX[j + 1];
										lightOldEndPosY[j + 1] = lightEndPosY[j + 1];
										lightEndPosX[j + 1] = startX;
									}

									lightEndPosX[j + 1] = startX;
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

								//DrawFormatString(0, 200, GetColor(255, 255, 255), "通ってる %d", j, true);

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

							/*DrawFormatString(0, 300, GetColor(255, 255, 255), "lightTouchMirror[1] %d", lightTouchMirror[2], true);*/
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

							/*					DrawFormatString(0, 300, GetColor(255, 255, 255), "通ってる", true);*/

						}

					}

				}

				//--------------ゴール接触時--------------

				//光

				for (int j = 0; j <= lightMax; j++) {

					if (lightDirection[j] == 0) {

						if (goalPosX - goalRadius < lightStartPosX[j] + lightRadius && lightEndPosX[j] - lightRadius < goalPosX + goalRadius) {
							if (goalPosY - goalRadius < lightStartPosY[j] && lightEndPosY[j] < goalPosY + goalRadius) {

								lightEndPosY[j] = goalPosY + goalRadius - 1;
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

								lightEndPosX[j] = goalPosX - goalRadius;
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

								lightEndPosY[j] = goalPosY - goalRadius + 1;
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

								lightEndPosX[j] = goalPosX + goalRadius;
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
							scene = 3;
							//DrawFormatString(0, 600, GetColor(255, 255, 255), "通ってる", true);
						}
						else {
							playerPosX = playerOldPosX;
							playerPosY = playerOldPosY;
						}

					}
				}

				//鏡


				for (int j = 0; j <= mirrorMax; j++) {
					if (mirrorPosX[j] - mirrorRadius < goalPosX + goalRadius && goalPosX - goalRadius < mirrorPosX[j] + mirrorRadius) {
						if (mirrorPosY[j] - mirrorRadius < goalPosY + goalRadius && goalPosY - goalRadius < mirrorPosY[j] + mirrorRadius) {

							mirrorPosX[j] = mirrorOldPosX[j];
							mirrorPosY[j] = mirrorOldPosY[j];

						}
					}
				}

			}

			//-----------ブロックの当たり判定--------------

			//光

			for (int i = 0; i <= blockMax; i++) {
				for (int j = 0; j <= lightMax; j++) {

					if (lightDirection[j] == 0) {

						if (blockPosX[i] - blockRadius < lightStartPosX[j] + lightRadius && lightEndPosX[j] - lightRadius < blockPosX[i] + blockRadius) {
							if (blockPosY[i] - blockRadius < lightStartPosY[j] && lightEndPosY[j] < blockPosY[i] + blockRadius) {

								lightEndPosY[j] = blockPosY[i] + blockRadius;
								lightMax = j;
							}
							else {

							}
						}
						else {

						}
					}
					if (lightDirection[j] == 1) {

						if (blockPosX[i] - blockRadius < lightEndPosX[j] && lightStartPosX[j] < blockPosX[i] + blockRadius) {
							if (blockPosY[i] - blockRadius < lightEndPosY[j] + lightRadius && lightStartPosY[j] - lightRadius < blockPosY[i] + blockRadius) {

								lightEndPosX[j] = blockPosX[i] - blockRadius;
								lightMax = j;
							}
							else {

							}
						}
						else {

						}

					}
					if (lightDirection[j] == 2) {

						if (blockPosX[i] - blockRadius < lightEndPosX[j] + lightRadius && lightStartPosX[j] - lightRadius < blockPosX[i] + blockRadius) {
							if (blockPosY[i] - blockRadius < lightEndPosY[j] && lightStartPosY[j] < blockPosY[i] + blockRadius) {

								lightEndPosY[j] = blockPosY[i] - blockRadius;
								lightMax = j;

							}
							else {

							}
						}
						else {

						}

					}
					if (lightDirection[j] == 3) {

						if (blockPosX[i] - blockRadius < lightStartPosX[j] && lightEndPosX[j] < blockPosX[i] + blockRadius) {
							if (blockPosY[i] - blockRadius < lightStartPosY[j] + lightRadius && lightEndPosY[j] - lightRadius < blockPosY[i] + blockRadius) {

								lightEndPosX[j] = blockPosX[i] + blockRadius;
								lightMax = j;


							}
							else {

							}
						}
						else {

						}

					}
				}
			}

			//自機

			for (int i = 0; i <= blockMax; i++) {
				if (playerPosX - playerRadius < blockPosX[i] + blockRadius && blockPosX[i] - blockRadius < playerPosX + playerRadius) {
					if (playerPosY - playerRadius < blockPosY[i] + blockRadius && blockPosY[i] - blockRadius < playerPosY + playerRadius) {

						playerPosX = playerOldPosX;
						playerPosY = playerOldPosY;

					}
				}
			}

			//鏡

			for (int i = 0; i <= blockMax; i++) {
				for (int j = 0; j <= mirrorMax; j++) {
					if (mirrorPosX[j] - mirrorRadius < blockPosX[i] + blockRadius && blockPosX[i] - blockRadius < mirrorPosX[j] + mirrorRadius) {
						if (mirrorPosY[j] - mirrorRadius < blockPosY[i] + blockRadius && blockPosY[i] - blockRadius < mirrorPosY[j] + mirrorRadius) {

							mirrorPosX[j] = mirrorOldPosX[j];
							mirrorPosY[j] = mirrorOldPosY[j];

						}
					}
				}
			}


			//-----------光と自機の接触---------------

			for (int j = 0; j <= lightMax; j++) {

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
		}

		if (scene == 3) {

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				scene = 1;
				stagePick = 1;
				stage = 0;
				stageReset = 0;
			}

		}


		// 描画処理

		DrawFormatString(0, 0, GetColor(255, 255, 255), "scene%d", scene);
		DrawFormatString(0, 10, GetColor(255, 255, 255), "stage%d", stage);
		DrawFormatString(0, 20, GetColor(255, 255, 255), "stagePick%d", stagePick);



		if (scene == 0) {

			DrawGraph(0, 0, backgh, true);
			DrawGraph(titleX, titleY, titlegh, true);
			DrawGraph(0, 0, space[spaceMove / 10], true);
			if (changeCount == 1) {
				DrawCircle(changeX, changeY, changeR, GetColor(255, 255, 255), true);

			}
		}
		else if (scene == 1) {

			if (changeCount == 1) {
				DrawCircle(changeX, changeY, changeR, GetColor(255, 255, 255), true);
			}

			DrawGraph(0, 0, stageSerectGH, true);

			//ステージ選択

			for (int y = 0; y < STAGE_MAX; y++) {
				for (int x = 0; x < STAGE_MAX; x++) {

					if (stagePick == (y * 3) + (x + 1)) {
						DrawBox(stagePosX[x][y], stagePosY[x][y], stagePosX[x][y] + stageRadiusX, stagePosY[x][y] + stageRadiusY, GetColor(255, 0, 0), true);
					}
					else {
						DrawBox(stagePosX[x][y], stagePosY[x][y], stagePosX[x][y] + stageRadiusX, stagePosY[x][y] + stageRadiusY, GetColor(255, 255, 255), true);
					}
				}
			}

		}
		else if (scene == 2) {


			//ブロック

			for (int i = 0; i <= blockMax; i++) {
				DrawBox(blockPosX[i] + blockRadius, blockPosY[i] + blockRadius, blockPosX[i] - blockRadius, blockPosY[i] - blockRadius, GetColor(100, 100, 100), true);
			}

			//ゴール

			if (isGoal == 0) {
				DrawBox(goalPosX + goalRadius, goalPosY + goalRadius, goalPosX - goalRadius, goalPosY - goalRadius, GetColor(0, 204, 255), true);
			}

			//枠

			for (int i = 0; i <= WIN_HEIGHT - (outY * 2); i += BLOCKSIZE) {
				DrawLine(startX, startY + i, endX, startY + i, GetColor(255, 255, 255), true);
			}
			for (int i = 0; i <= WIN_WIDTH - (outX * 2); i += BLOCKSIZE) {
				DrawLine(startX + i, startY, startX + i, endY, GetColor(255, 255, 255), true);
			}

			//背景の明暗

			if (isGoal == 0) {

				for (int y = 0; y < 10; y++) {
					for (int x = 0; x < 10; x++) {
						if (map[y][x] == 0) {
							map[y][x] = 1;
						}
					}
				}

				//自機の周りのみうっすらと見えるようにする
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);
				for (int y = -1; y < 2; y++) {
					for (int x = -1; x < 2; x++) {
						if (map[playerMapY - y][playerMapX - x] == 1) {

							if (playerMapX == 0 && x == 1) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
								DrawBox((playerMapX - x) * BLOCKSIZE + startX, (playerMapY - y) * BLOCKSIZE + startY,
										(playerMapX - x) * BLOCKSIZE + startX + BLOCKSIZE, (playerMapY - y) * BLOCKSIZE + startY + BLOCKSIZE,
										GetColor(0, 0, 0), true);

								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);
							}
							else if (playerMapX == 9 && x == -1) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
								DrawBox((playerMapX - x) * BLOCKSIZE + startX, (playerMapY - y) * BLOCKSIZE + startY,
										(playerMapX - x) * BLOCKSIZE + startX + BLOCKSIZE, (playerMapY - y) * BLOCKSIZE + startY + BLOCKSIZE,
										GetColor(0, 0, 0), true);

								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);

							}
							else {
								DrawBox((playerMapX - x) * BLOCKSIZE + startX, (playerMapY - y) * BLOCKSIZE + startY,
										(playerMapX - x) * BLOCKSIZE + startX + BLOCKSIZE, (playerMapY - y) * BLOCKSIZE + startY + BLOCKSIZE,
										GetColor(0, 0, 0), true);

								map[playerMapY - y][playerMapX - x] = 0;
							}

						}
					}
				}

				//暗闇にする
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				for (int y = 0; y < 10; y++) {
					for (int x = 0; x < 10; x++) {
						if (map[y][x] == 1) {

							DrawBox(x * BLOCKSIZE + startX, y * BLOCKSIZE + startY,
									x * BLOCKSIZE + startX + BLOCKSIZE, y * BLOCKSIZE + startY + BLOCKSIZE,
									GetColor(0, 0, 0), true);

						}
						else {

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
							DrawLine(x * BLOCKSIZE + startX + BLOCKSIZE - 1, y * BLOCKSIZE + startY,
									 x * BLOCKSIZE + startX + BLOCKSIZE - 1, y * BLOCKSIZE + startY + BLOCKSIZE,
									 GetColor(255, 255, 255), true);
							DrawLine(x * BLOCKSIZE + startX, y * BLOCKSIZE + startY + BLOCKSIZE - 1,
									 x * BLOCKSIZE + startX + BLOCKSIZE, y * BLOCKSIZE + startY + BLOCKSIZE - 1,
									 GetColor(255, 255, 255), true);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

						}
					}
				}

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			}


			//光

			for (int i = 0; i <= lightMax; i++) {

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
				DrawLine(lightStartPosX[i], lightStartPosY[i], lightEndPosX[i], lightEndPosY[i], GetColor(128, 128, 128), 64);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				DrawLine(lightStartPosX[i], lightStartPosY[i], lightEndPosX[i], lightEndPosY[i], GetColor(100, 100, 100), 40);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				DrawLine(lightStartPosX[i], lightStartPosY[i], lightEndPosX[i], lightEndPosY[i], GetColor(255, 255, 255), 20);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);

			}


			//移動範囲

			if (charaChange == 1) {
				for (int i = 0; i <= mirrorMax; i++) {
					DrawBox(mirrorRangeUpX[i], mirrorRangeUpY[i], mirrorRangeDownX[i], mirrorRangeDownY[i], GetColor(255, 0, 0), false);
				}
			}

			//鏡

			for (int i = 0; i <= mirrorMax; i++) {
				DrawMirror(mirrorPosX[i], mirrorPosY[i], mirrorRadius, 15, mirrorDirection[i]);
			}

			//DrawFormatString(0, 20, GetColor(255, 255, 255), "%d", mirrorDirection[0], true);
			//DrawFormatString(0, 40, GetColor(255, 255, 255), "%d", mirrorDirection[1], true);
			//DrawFormatString(60, 60, GetColor(255, 255, 255), "%d", lightTouch[0], true);

			//for (int i = 0; i <= lightMax; i++) {
			//	DrawFormatString(lightStartPosX[i], lightStartPosY[i], GetColor(255, 0, 0), "%dS", i);
			//	DrawFormatString(lightEndPosX[i], lightEndPosY[i], GetColor(255, 0, 0), "%dE", i);
			//}

			//DrawFormatString(0, 0, GetColor(255, 255, 255), "見えないゴールに光を当ててプレイヤーを持っていけばクリア", true);
			//DrawFormatString(0, 20, GetColor(255, 255, 255), "WASD :鏡移動 ↑→↓← : プレイヤー移動 SPASE : 鏡回転（時計回り）", true);
			//DrawFormatString(0, 40, GetColor(255, 255, 255), "lightContact:%d", lightContact, true);
			//DrawFormatString(0, 60, GetColor(255, 255, 255), "lightMax:%d", lightMax, true);

			//自機

			DrawCircle(playerPosX, playerPosY, playerRadius, GetColor(255, 255, 255), true);

			if (isGoal != 0) {
				DrawExtendGraph(goalPosX + goalRadius, goalPosY + goalRadius, goalPosX - goalRadius, goalPosY - goalRadius, goalGH, true);
			}

			int lineR = 10;

			DrawLine(startX, startY - (lineR / 2), endX, startY - (lineR / 2), GetColor(255, 255, 255), lineR);
			DrawLine(startX, endY + (lineR / 2), endX, endY + (lineR / 2), GetColor(255, 255, 255), lineR);

			DrawLine(endX + (lineR / 2), startY, endX + (lineR / 2), endY, GetColor(255, 255, 255), lineR);
			DrawLine(startX - (lineR / 2), startY, startX - (lineR / 2), endY, GetColor(255, 255, 255), lineR);

			DrawBox(0, 0, WIN_WIDTH, startY - lineR, GetColor(0, 0, 0), true);

			if (stage == 1) {
				DrawGraph(0, 416, tutorial1Gh[tutorialPage], true);
			}
			else if (stage == 2) {
				DrawGraph(0, 416, tutorial2Gh[tutorialPage], true);
			}
			else if (stage == 3) {
				DrawGraph(0, 416, tutorial3Gh[tutorialPage], true);
			}

		}
		else {

			DrawFormatString(400, 400, GetColor(255, 255, 255), "clear PRESS SPACE", true);
			DrawGraph(0, 0, clearGH, true);
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
