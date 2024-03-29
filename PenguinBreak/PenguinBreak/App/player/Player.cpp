﻿#include "Player.h"
#include <time.h>
#include <cassert>
#include "Input.h"
#include"Debugtext.h"
#include"Texture.h"

using namespace DirectX;

Player::Player()
{}

Player::~Player()
{
	audio->SoundUnload(&deathSound);
}

void Player::Initialize()
{
	//スプライト作成の仕方
	player = Sprite::Get()->SpriteCreate(L"Resources/uma.png");
	hand_p = Sprite::Get()->SpriteCreate(L"Resources/hand_pa.png");
	hand_g = Sprite::Get()->SpriteCreate(L"Resources/hand_g.png");
	moveParticle = std::make_unique <ParticleManager>();
	moveParticle->Initialize();
	p_Texture = Texture::Get()->LoadTexture(L"Resources/Particle/particle3.png");
	for (int i = 0; i < DEATH_MAX; i++)
	{
		death[i] = Sprite::Get()->SpriteCreate(L"Resources/death.png");
	}
	deathSound = Audio::SoundLoadWave("Resources/Sound/death.wav");
}

void Player::Init(Stage* stage)
{
	position = {
	stage->GetInstance()->GetStartPos().x,
	stage->GetInstance()->GetStartPos().y
	};
	oldPos = position;
	flipFlag = false;
	goalFlag = false;
	deathTime = 20;
	respawn = false;
	effect = false;
	deathCount = 0;
	for (int i = 0; i < DEATH_MAX; i++)
	{
		deathPos[i] = { 0,0 };
		isDeathDraw[i] = false;
	}
	size = Vec2(width, height) / static_cast<float>(stage->GetScale());
	out = Vec2(outX, outY) / static_cast<float>(stage->GetScale());
	SetSize(size, out);
	move = false;
}

void Player::stageInit(int stageNo)
{
	//初期ポジション
	//if(stageNo==){
	//position={};
	//}
}

void Player::Update(Stage* stage)
{
	//スクリーン座標からワールド座標に変換する処理
	ConvertParticlePos();
	//移動
	Move();
	collide2Stage(stage);
	Input::Get()->SetCursor(false);
}

void Player::Move()
{
	if (Input::Get()->MousePushLeft() && !effect) {
		const Vec2 mouseSize = { 32,32 };
		oldPos = position;
		if (Collision::BoxCollision(Input::Get()->GetMousePos(), position, mouseSize, radius) && !goalFlag) {
			//position = Input::Get()->GetMousePos();
			move = true;

			//プレイヤーの画像によってはいらない処理
			if (static_cast<float>(Input::Get()->GetMouseMove().lX) > 0) {
				flipFlag = true;
			}
			else {
				flipFlag = false;
			}
		}
		//パーティクルだす
		//手のspを表示するか
		isDraw = true;
		moveParticle->ParticleAdd2(particlePos, color, color2);
	}
	else {
		isDraw = false;
		move = false;
	}
	if (move == true) {
		position = Input::Get()->GetMousePos();
	}
	if (stageNum == 1) {
		color = { 1, 1, 0.5, 1 };
		color2 = { 1, 1, 0.5, 1 };
	}
	else if (stageNum == 2) {
		color = { 1, 0, 0, 1 };
		color2 = { 1, 0, 1, 1 };
	}
	else {
		color = { 0, 0.8f, 1, 1 };
		color2 = { 1, 0.5f,0.8f, 1 };
	}
}

void Player::ConvertParticlePos()
{
	//ビューポート行列
	XMMATRIX mvp = XMMatrixIdentity();
	mvp.r[0].m128_f32[0] = 1280.0f / 2.0f;
	mvp.r[1].m128_f32[1] = -720.0f / 2.0f;
	mvp.r[3].m128_f32[0] = 1280.0f / 2.0f;
	mvp.r[3].m128_f32[1] = 720.0f / 2.0f;
	//ビュープロジェクションビューポート合成行列
	XMMATRIX mvpv = moveParticle->GetMat() * mvp;
	//上記の行列の逆行列
	XMMATRIX mvpvInv = XMMatrixInverse(nullptr, mvpv);
	//スクリーン座標
	Vec3 posNear = Vec3(position.x, position.y, 0);
	Vec3 posFar = Vec3(position.x, position.y, 1);
	XMVECTOR posNearV = XMLoadFloat3(&posNear);
	XMVECTOR posFarV = XMLoadFloat3(&posFar);
	//スクリーン座標系からワールド座標系へ
	posNearV = XMVector3TransformCoord(posNearV, mvpvInv);//座標に行列をかけてwを除算
	posFarV = XMVector3TransformCoord(posFarV, mvpvInv);
	//レイの方向
	XMVECTOR direction = posFarV - posNearV;
	//ベクトルの正規化
	direction = XMVector3Normalize(direction);
	const float distance = 0.0f;

	particlePos.x = posNearV.m128_f32[0] - direction.m128_f32[0] * distance;
	particlePos.y = posNearV.m128_f32[1] - direction.m128_f32[1] * distance;
	particlePos.z = posNearV.m128_f32[2] - direction.m128_f32[2] * distance;

	moveParticle->Update();
}

void Player::collide2Stage(Stage* stage)
{
	//矩形同士の当たり判定か点と矩形の当たり判定のカウントが道の数と同じになったら死亡演出
	//矩形同士はステージとの当たり判定用
	//点と矩形はワープした時用
	if (stage->GetRoadCount() == CollisionCount(stage) || PointCollisionCount(stage) == stage->GetRoadCount())
	{
		//エフェクトだす
		effect = true;
	}
	else if (!effect)
	{
		respawn = false;
	}
	//死亡時演出後の処理
	if (effect)
	{
		deathTime--;
		//一定時間に達したらリスポーンする
		if (deathTime <= 0) {
			deathTime = 20;
			respawn = true;
		}
		else if (deathTime >= 19) {
			audio->SoundSEPlayWave(deathSound);
		}
		if (respawn == true) {
			effect = false;
			//死のカウントをプラス
			deathCount++;
			//死んだ場所の位置コピー
			deathPos[deathCount - 1] = position;
			//死んだ場所に描画するスプライトのフラグをture
			isDeathDraw[deathCount - 1] = true;
			//ロード外に出たらスタート位置に戻す
			position = {
				stage->GetPos(stage->GetRestart()).x,
				stage->GetPos(stage->GetRestart()).y
			};
			oldPos = position;
			stage->AllPlayerFlagTrue();

			if (deathCount >= DEATH_MAX)
			{
				deathCount = 0;
			}
		}
		else if (respawn == false) {
			effect = true;
		}
	}
	//ゴールの判定
	if (!OutStage(position, stage, static_cast<int>(stage->GetGoal()))) {
		goalFlag = true;
	}
	//ゴール後、死亡演出出さない
	if (goalFlag)
	{
		effect = false;
	}
}

int Player::CollisionCount(Stage* stage)
{
	int count = 0;
	for (int i = 0; i < stage->GetBoxSize(); i++)
	{
		//i番目のステージが壁の場合
		if (stage->GetType(i) == Road::RoadType::WALL)
		{
			//全てのステージと当たってなくても初期位置に戻す
			if (!OutStage(position, stage, i))
			{
				count = static_cast<int>(stage->GetRoadCount());
				break;
			}
		}
		//i番目のステージが背景だった場合数えない
		else if (stage->GetType(i) == Road::RoadType::BACK)
		{
			continue;
		}
		else
		{
			//道に入っていなかったらカウント
			if (OutStage(position, stage, i))
			{
				count++;
			}
			else
			{
				if (stage->GetType(i) == Road::RoadType::START || stage->GetType(i) == Road::RoadType::SAVE)
				{
					stage->ChangeRestart(i);
				}
			}
		}
	}
	//
	return count;
}

bool Player::OutStage(Vec2 position, Stage* stage, int num)
{
	//ステージスプライトの中心座標
	Vec2 stageCenter = {
		stage->GetInstance()->GetPos(num).x,
		stage->GetInstance()->GetPos(num).y
	};
	//X軸、Y軸の距離を算出
	distance =
	{
		stageCenter.x - position.x,
		stageCenter.y - position.y
	};
	//絶対値にするため結果が負なら正にする
	if (distance.x < 0.0f) { distance.x *= -1.0f; }
	if (distance.y < 0.0f) { distance.y *= -1.0f; }
	//2つの矩形の和を算出
	const Vec2 outSize = { 9.0f,12.0f };
	size_num =
	{
		((stage->GetInstance()->GetSize(num).x) / 2.0f) - radius.x + outX,
		((stage->GetInstance()->GetSize(num).y) / 2.0f) - radius.y + outY
	};
	//距離がサイズの和より小さいor以下
	if (distance.x <= size_num.x && distance.y <= size_num.y)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Player::Point2Box(Stage* stage, Vec2 point, int num)
{
	//ステージの左上座標
	Vec2 leftTop = {
		stage->GetPos(num).x - (stage->GetSize(num).x / 2),
		stage->GetPos(num).y - (stage->GetSize(num).y / 2)
	};
	//ステージの右下座標
	Vec2 rightBottom = {
		stage->GetPos(num).x + (stage->GetSize(num).x / 2),
		stage->GetPos(num).y + (stage->GetSize(num).y / 2)
	};
	//点の座標が左辺よりも大きかったらtrue
	bool left = false;
	if (point.x >= leftTop.x)
	{
		left = true;
	}
	//点の座標が右辺よりも小さかったらtrue
	bool right = false;
	if (point.x <= rightBottom.x)
	{
		right = true;
	}
	//点の座標が上辺よりも大きかったらtrue
	bool top = false;
	if (point.y >= leftTop.y)
	{
		top = true;
	}
	//点の座標が下辺よりも小さかったらtrue
	bool bottom = false;
	if (point.y <= rightBottom.y)
	{
		bottom = true;
	}
	//矩形の中に入ってなかったらtrue
	if (left && right && top && bottom)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Player::Old2Now(Vec2 oldPos, Vec2 position, Stage* stage, int num)
{
	int count = 0;
	//現在は前フレームより大きかったらfor文を前フレームから進める
	if (position.x >= oldPos.x)
	{
		count = Point2BoxCount(oldPos, position, stage, num);
	}
	else
	{
		count = Point2BoxCount(position, oldPos, stage, num);
	}
	//カウントが0以外だったらtrue
	if (count > 0)
	{
		return true;
	}
	else {
		return false;
	}
}

int Player::Point2BoxCount(Vec2 point1, Vec2 point2, Stage* stage, int num)
{
	int count = 0;
	//前フレームの座標から現在までの座標が全てステージに乗っているか調べる
	for (float i = point1.x; i <= point2.x; i++)
	{
		//点2の方が大きかったらfor文を点1から始める
		if (point2.y >= point1.y)
		{
			for (float j = point1.y; j <= point2.y; j++)
			{
				//点の座標がステージに入ってなかったらカウント
				if (Point2Box(stage, { i,j }, num))
				{
					count++;
				}
			}
		}
		else
		{
			for (float j = point2.y; j <= point1.y; j++)
			{
				//点の座標がステージに入ってなかったらカウント
				if (Point2Box(stage, { i,j }, num))
				{
					count++;
				}
			}
		}
	}

	return count;
}

int Player::PointCollisionCount(Stage* stage)
{
	int count = 0;
	for (int i = 0; i < stage->GetBoxSize(); i++)
	{
		//i番目のステージが背景だったら数えない
		if (stage->GetType(i) == Road::RoadType::BACK)
		{
			continue;
		}

		stage->SetPlayerFlag(i, Old2Now(oldPos, position, stage, i) == false);

		//i番目のステージが壁だったら
		if (stage->GetType(i) == Road::RoadType::WALL)
		{
			//点がステージに当たってても初期位置に戻す
			if (stage->GetPlayerFlag(i))
			{
				count = static_cast<int>(stage->GetRoadCount());
				break;
			}
		}
		else
		{
			//i番目のステージに点が入っていなかったらカウント
			if (stage->GetPlayerFlag(i))
			{
				//i番目のステージがスタート位置かセーブポイントだったらリスタート地点を変える
				if (stage->GetType(i) == Road::RoadType::START || stage->GetType(i) == Road::RoadType::SAVE)
				{
					stage->ChangeRestart(i);
				}
				//i番目のステージがスイッチだったらギミックを切り替える
				if (stage->GetType(i) == Road::RoadType::SWITCH)
				{
					stage->SwitchCount(i);
				}
			}
			else
			{
				count++;
			}
		}
	}

	return count;
}

void Player::Draw()
{
	//2D描画
	for (int i = 0; i < DEATH_MAX; i++)
	{
		if (isDeathDraw[i])
		{
			Sprite::Get()->Draw(death[i], deathPos[i], 32, 32, { 0.5f,0.5f });
		}
	}
	moveParticle->Draw(p_Texture);
	Sprite::Get()->Draw(player, position, width, height, { 0.5f,0.5f }, { 1,1,1,1 }, flipFlag);

	if (isDraw)
	{
		Vec2 hPos = { Input::Get()->GetMousePos().x,Input::Get()->GetMousePos().y };
		Sprite::Get()->Draw(hand_g, hPos, 32, 32, { 0.5f,0.5f });
	}
	else
	{
		Vec2 hPos = { Input::Get()->GetMousePos().x,Input::Get()->GetMousePos().y };
		Sprite::Get()->Draw(hand_p, hPos, 32, 32, { 0.5f,0.5f });
	}

}

void Player::SetSize(const Vec2& size, const Vec2& out)
{
	width = size.x;
	height = size.y;
	outX = out.x;
	outY = out.y;
	radius = (size / 2) - Vec2(4.0f, 4.0f);
}
