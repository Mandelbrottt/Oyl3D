#pragma once

#include <Oyl3D.h>

class SoundLayer : public oyl::Layer
{
public:
	OYL_CTOR(SoundLayer, Layer);

	void onEnter()override;

	void onUpdate() override;

	bool onEvent(const oyl::Event& event) override;
};