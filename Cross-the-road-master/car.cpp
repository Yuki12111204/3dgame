#include "car.h"

// モデルハンドル
int car::model_regular;
int car::model_rora;

car::car() {
	// モデルハンドルの読み込み
	model_regular = MV1LoadModel("./resorces/Car8.mv1");
	MV1SetScale(model_regular, VGet(100.0f, 100.0f, 100.0f));

	// 初期状態では回転させない
	MV1SetRotationXYZ(model_regular, VGet(0.0f, 0.0f, 0.0f));

	model_rora = MV1LoadModel("./resorces/rora.mv1");

	initialize(CAR_TYPE_REGULAR, 900.0f, VGet(2000.0f, 100.0f, -1350.0f), -50.0f);
}

car::car(int type, VECTOR pos, float v) {
	car_type = type;

	switch (type) {
	case CAR_TYPE_REGULAR:
		initialize(CAR_TYPE_REGULAR, 1.0f, pos, v);
		break;

	case CAR_TYPE_RORA:
		initialize(CAR_TYPE_RORA, 3.0f, pos, v);
		model_rotation = VGet(DX_PI_F / 2, 0.0f, 0.0f);
		break;
	}
}

void car::initialize(int type, float extendf, VECTOR pos, float v) {
	// 3Dモデルの読み込み(複製)
	switch (type) {
	case CAR_TYPE_REGULAR:
		model_handle = MV1DuplicateModel(model_regular);
		break;

	case CAR_TYPE_RORA:
		model_handle = MV1DuplicateModel(model_rora);
		break;
	}

	model_extend = VGet(extendf, extendf, extendf);
	flag = true;

	speed = v;

	/* ----- 3Dモデルの設定変更 ----- */

	// 3Dモデルの拡大縮小
	MV1SetScale(model_handle, model_extend);

	// 3Dモデルの輪郭線の修正
	int MaterialNum = MV1GetMaterialNum(model_handle);

	for (int i = 0; i < MaterialNum; i++) {
		float dotwidth = MV1GetMaterialOutLineDotWidth(model_handle, i);

		MV1SetMaterialOutLineDotWidth(
			model_handle,
			i,
			dotwidth / 50.0f
		);
	}

	/* ----- 3Dモデルの配置 ----- */

	model_position = VGet(
		pos.x,
		-300.0f + pos.y,
		pos.z
	);

	// ================================
	// 車の初期回転
	// ================================
	if (car_type == CAR_TYPE_REGULAR) {

		if (speed > 0.0f) {
			// X軸プラス方向へ進む
			model_rotation = VGet(
				0.0f,
				0.0f,
				0.0f
			);
		}
		else {
			// X軸マイナス方向へ進む
			model_rotation = VGet(
				0.0f,
				DX_PI_F,
				0.0f
			);
		}
	}
	else {
		model_rotation = VGet(
			0.0f,
			0.0f,
			0.0f
		);
	}

	MV1SetPosition(model_handle, model_position);
	MV1SetRotationXYZ(model_handle, model_rotation);
}

void car::update() {
	// ================================
	// 移動量計算
	// ================================
	VECTOR value = VGet(
		speed * 2.0f,
		0.0f,
		0.0f
	);

	// 移動量の加算
	model_position = VAdd(
		model_position,
		value
	);

	// ================================
	// 上限・下限に到達したとき
	// ================================
	if (model_position.x > ROAD_LIMIT_LEFT ||
		ROAD_LIMIT_RIGHT > model_position.x) {

		flag = false;
	}

	// ================================
	// 各車タイプごとの処理
	// ================================
	switch (car_type) {

	case CAR_TYPE_REGULAR:

		// ------------------------------
		// 進行方向に車の前面を向ける
		// ------------------------------
		if (speed > 0.0f) {

			// X軸プラス方向
			model_rotation = VGet(
				0.0f,
				-DX_PI_F / 2.0f,
				0.0f
			);
		}
		else {

			// X軸マイナス方向
			model_rotation = VGet(
				0.0f,
				DX_PI_F / 2.0f,
				0.0f
			);
		}

		break;

	case CAR_TYPE_RORA:

		model_rotation.z -= speed / 1000.0f;

		break;
	}

	// ================================
	// 移動後の座標・回転を設定
	// ================================
	MV1SetPosition(
		model_handle,
		model_position
	);

	MV1SetRotationXYZ(
		model_handle,
		model_rotation
	);
}

void car::draw() {
	// 3Dモデルを描画
	MV1DrawModel(model_handle);
}

void car::draw_log() {
	printfDx(
		"car_type[REGULAR] _handle[%d]\n",
		model_handle
	);
}

void car::finalize() {
	// モデルハンドルの削除
	MV1DeleteModel(model_handle);
}

// 車の座標を VECTOR 型で取得する
VECTOR car::get_position() {
	return model_position;
}

// 車の移動量を VECTOR 型で取得する
VECTOR car::get_move_vector() {
	return VGet(
		speed,
		0.0f,
		0.0f
	);
}