#include "Input.h"

void Input::Initialize()
{
	//キーボード初期化
	m_keybord.Initialize();

	//コントローラー初期化
	m_controller.InitController();

	//マウス初期化
	m_mouse.Init();
}

void Input::Update()
{
	//キーボード更新	
	m_keybord.Update();
	//コントローラー更新
	m_controller.UpdateInput();
	//マウス更新
	m_mouse.Update();
}


bool Input::KeybordPush(BYTE keyNumber)
{
	return m_keybord.PushKey(keyNumber);
}

bool Input::KeybordTrigger(BYTE keyNumber)
{
	return m_keybord.TriggerKey(keyNumber);
}

bool Input::KeybordReleaseTheHand(BYTE keyNumber)
{
	return m_keybord.ReleaseTheHand(keyNumber);;
}

bool Input::KeybordInputArrow()
{
	if (KeybordPush(DIK_UP) || KeybordPush(DIK_DOWN)
		|| KeybordPush(DIK_LEFT) || KeybordPush(DIK_RIGHT))
	{
		return true;
	}
	return false;
}

bool Input::ControllerPush(ButtonKind button)
{
	return m_controller.IsButtonPush(button);
}

bool Input::ControllerUp(ButtonKind button)
{
	return m_controller.IsButtonUp(button);
}

bool Input::ControllerDown(ButtonKind button)
{
	return m_controller.IsButtonDown(button);
}

float Input::GetLeftAngle()
{
	return m_controller.GetLeftAngle();
}

float Input::GetRightAngle()
{
	return m_controller.GetRightAngle();
}

bool Input::ConRightInput()
{
	if (ControllerPush(RButtonUp) == false &&
		ControllerPush(RButtonDown) == false &&
		ControllerPush(RButtonLeft) == false &&
		ControllerPush(RButtonRight) == false)
	{
		return true;
	}
	return false;
}

bool Input::ConLeftInputS()
{
	if (ControllerPush(LButtonUpS) == true ||
		ControllerPush(LButtonDownS) == true ||
		ControllerPush(LButtonLeftS) == true ||
		ControllerPush(LButtonRightS) == true)
	{
		return true;
	}
	return false;
}

bool Input::ConLeftInput()
{
	if (ControllerPush(LButtonUp) == true ||
		ControllerPush(LButtonDown) == true ||
		ControllerPush(LButtonLeft) == true ||
		ControllerPush(LButtonRight) == true)
	{
		return true;
	}
	return false;
}

bool Input::MousePushLeft()
{
	return m_mouse.PushMouseLeft();
}

bool Input::MousePushMiddle()
{
	return m_mouse.PushMouseMiddle();
}

bool Input::MouseTriggerLeft()
{
	return m_mouse.TriggerMouseLeft();
}

bool Input::MouseTriggerMiddle()
{
	return m_mouse.TriggerMouseMiddle();
}

bool Input::MouseReleaseLeft()
{
	return  m_mouse.ReleseMouseLeft();
}

Mouse::MouseMove Input::GetMouseMove()
{
	return m_mouse.GetMouseMove();
}

Vec2 Input::GetMousePos()
{
	return m_mouse.GetMousePos();
}

void Input::SetCursor(bool flag)
{
	ShowCursor(flag);
}

