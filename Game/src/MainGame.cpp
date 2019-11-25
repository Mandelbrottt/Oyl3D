#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "PlayerInteractionValidation.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
	OYL_CTOR(MainLayer, oyl::Layer)

	bool isCameraActive = false;

	void onEnter() override
	{
		addToCategoryMask(CategoryKeyboard);
		addToCategoryMask(CategoryMouse);

		scheduleSystemUpdate<PlayerSystem>();
		scheduleSystemUpdate<CannonSystem>();
		scheduleSystemUpdate<PlayerInteractionValidationSystem>();

		auto lightShader = Shader::get(LIGHTING_SHADER_ALIAS);

		Material::cache(lightShader, nullptr, "monkeyMat");

		auto textureShader = Shader::get(TEXTURE_SHADER_ALIAS);
		auto uv = Texture2D::get(UV_TEXTURE_ALIAS);

		Material::cache(textureShader, uv, "cubeMat");

		{
			auto e = registry->create();
			registry->assign<component::Transform>(e);
			auto& camera = registry->assign<component::PlayerCamera>(e);
			camera.player = 0;
			camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
			auto& so = registry->assign<component::SceneObject>(e);
			so.name = "Player Camera";
		}
	}

	void onUpdate(Timestep dt) override
	{
		auto view = registry->view<Player, component::Transform>();
		for (auto& entity : view)
		{
			auto& player = registry->get<Player>(entity);
			auto& playerTransform = registry->get<component::Transform>(entity);

			glm::vec3 desiredMoveDirection = glm::vec3(0.0f);

			if (Input::isKeyPressed(Key_W))
				desiredMoveDirection += playerTransform.getForward();

			if (Input::isKeyPressed(Key_S))
				desiredMoveDirection += -playerTransform.getForward();

			if (Input::isKeyPressed(Key_A))
				desiredMoveDirection += -playerTransform.getRight();

			if (Input::isKeyPressed(Key_D))
				desiredMoveDirection += playerTransform.getRight();

		    //check if it's 0 because if we normalize a vector with 0 magnitude it breaks
		    if (desiredMoveDirection == glm::vec3(0.0f))
			    player.moveDirection = desiredMoveDirection;
			else
				player.moveDirection = glm::normalize(desiredMoveDirection);
		}
	}

	bool onEvent(Ref<Event> event) override
	{
		switch (event->type)
		{
		case TypeKeyReleased:
		{
			Window& window = oyl::Application::get().getWindow();

			auto e = (oyl::KeyReleasedEvent) * event;
			if (e.keycode == oyl::Key_F11)
			{
				// TODO: Make Event Request
				if (window.getFullscreenType() == oyl::Windowed)
					window.setFullscreenType(oyl::Fullscreen);
				else
					window.setFullscreenType(oyl::Windowed);
			}
			else if (e.keycode == oyl::Key_F7)
			{
				window.setVsync(!window.isVsync());
			}
			else if (e.keycode == oyl::Key_C)
			{
				isCameraActive = !isCameraActive;

				CursorStateRequestEvent cursorRequest;
				cursorRequest.state = isCameraActive ? Cursor_Disabled : Cursor_Enabled;
				postEvent(Event::create(cursorRequest));
			}

			break;
		}
		case TypeKeyPressed:
		{
			Window& window = oyl::Application::get().getWindow();

			auto evt = (oyl::KeyPressedEvent) * event;

			switch (evt.keycode)
			{
			case oyl::Key_E:
			{
				auto playerView = registry->view<Player>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.player = playerEntity;
					postEvent(Event::create(playerInteractionRequest));
				}

				break;
			}
			/*case oyl::Key_W:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = playerView.get<component::Transform>(playerEntity).getForward();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_A:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = -playerView.get<component::Transform>(playerEntity).getRight();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_S:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = -playerView.get<component::Transform>(playerEntity).getForward();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_D:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = playerView.get<component::Transform>(playerEntity).getRight();
					postEvent(Event::create(playerMove));
				}

				break;
			}*/
			}
		}
		case TypeMouseMoved:
		{
			if (isCameraActive)
			{
				auto evt = (MouseMovedEvent)* event;

				auto playerView = registry->view<Player, component::Transform>();
				for (auto& entity : playerView)
				{
					component::Transform& playerTransform = registry->get<component::Transform>(entity);

					playerTransform.setRotationEulerY(playerTransform.getRotationEulerY() - evt.dx * 0.5f);
				}

				auto playerCameraView = registry->view<component::PlayerCamera, component::Transform>();
				for (auto& entity : playerCameraView)
				{
					component::Transform& cameraTransform = registry->get<component::Transform>(entity);

					cameraTransform.setRotationEulerX(cameraTransform.getRotationEulerX() - evt.dy * 0.5f);
				}
			}

			break;
		}
		}
		return false;
	}
};

class MainScene : public oyl::Scene
{
public:
	OYL_CTOR(MainScene, oyl::Scene)

		virtual void onEnter() override
	{
		pushLayer(MainLayer::create());
		pushLayer(SandboxLayer::create());
	}
};

class Game : public oyl::Application
{
public:
	Game()
	{
		pushScene(MainScene::create());
	}

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
	return new Game();
}