#pragma once
#include "Scene/SceneComponent.h"

ENGINE_USING

class TitleScene : public SceneComponent
{
public:
	TitleScene();
	~TitleScene();

public:
	virtual bool Init();

private:
	void CreatePrototype();
	void SceneChangeButton(float _fTime);
};

