﻿#pragma once
#include "Sprite.h"
#include <vector>
#include "./../../Library/Helper/LoadJson.h"

class Stage final
{
private: //シングルトン化
	Stage();
	Stage(const Stage&) = delete;
	~Stage();
	Stage& operator=(const Stage&) = delete;
public:
	static Stage* GetInstance()
	{
		static Stage instance;
		return &instance;
	}

public: //サブクラス
	// 道のタイプ
	enum RoadType
	{
		ROAD,  //道
		START, //スタート
		GOAL,  //ゴール
	};

	class Road
	{
	public:
		SpriteData sprite;

		Vec2 pos;
		Vec2 size;
		RoadType type;

	public:
		Road();
		Road(const Vec2& pos, const Vec2& size);
		void BoxInit();
	};

	struct JsonData
	{
		std::vector<Road> objects = {};
	};

private: //メンバ変数
	std::vector<Road> boxes;

public: //メンバ関数
	// 初期化
	void Init();
	// 描画
	void Draw(float offsetX = 0.0f, float offsetY = 0.0f);

	// ステージの書き込み
	void WriteStage(const std::string& stageName);
private:
	// ステージの読み込み
	JsonData* LoadStage(const std::string& jsonFile);

};
