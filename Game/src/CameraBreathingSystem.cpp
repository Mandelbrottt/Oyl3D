#include "CameraBreathingSystem.h"

void CameraBreathingSystem::onEnter()
{

}

void CameraBreathingSystem::onExit()
{

}

void CameraBreathingSystem::onUpdate()
{
	auto playerCameraView = registry->view<component::Camera, CameraBreathing, component::Transform>();
	for (auto& cameraEntity : playerCameraView)
	{
		auto& cameraTransform = registry->get<component::Transform>(cameraEntity);
		auto& cameraBreathing = registry->get<CameraBreathing>(cameraEntity);

		if (cameraBreathing.isMovingUp)
		{
			cameraBreathing.interpolationParam += cameraBreathing.speed * Time::deltaTime();
			
			//check for max height
			if (cameraBreathing.interpolationParam >= 1.0f) 
				cameraBreathing.isMovingUp = !cameraBreathing.isMovingUp;
		}
		else // !isMovingUp
		{
			cameraBreathing.interpolationParam -= cameraBreathing.speed * Time::deltaTime();

			//check for min height
			if (cameraBreathing.interpolationParam <= 0.0f) 
				cameraBreathing.isMovingUp = !cameraBreathing.isMovingUp;
		}

		float newPosY = glm::smoothstep(0.0f, 1.0f, cameraBreathing.interpolationParam);

		newPosY *= cameraBreathing.cameraHeightVariance;

		cameraTransform.setPositionY(cameraBreathing.startPosY + newPosY);
	}
}

bool CameraBreathingSystem::onEvent(const Event& event)
{
	return false;
}