#pragma once

#include <memory>

#include "Rendering/Window.h"

namespace Oyl::Glfw
{
	class OYL_RENDERING_API Window : public IWindow
	{
	public:
		Window() noexcept;

		Window(std::nullptr_t);

		struct CreateParams
		{
			Vector2i position = Vector2i(-1, -1);
			Vector2i size;

			std::string title;

			WindowStateFlags windowState;
			CursorState cursorState;

			PostEventDelegate postEventCallback;

			// Requested Default Graphics API
		};

		explicit
		Window(const CreateParams& a_params) noexcept;

		Window(Window&& a_other) noexcept;
		Window&
		operator=(Window&& a_other) noexcept;

		virtual
		~Window();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Update() override;

		void
		SetPostEventCallback(PostEventDelegate a_delegate) override;

		Vector2i
		GetSize() const override;

		void
		SetSize(Vector2i a_size) override;

		Vector2i
		GetFrameBufferSize() const override;

		Vector2i
		GetPosition() const override;

		void
		SetPosition(Vector2i a_position) override;

		std::string_view
		GetTitle() const override;

		void
		SetTitle(std::string_view a_title) override;

		WindowStateFlags
		GetWindowStateFlags() const override;

		void
		SetWindowStateFlags(WindowStateFlags a_flags) override;

		CursorState
		GetCursorStateFlags() const override;

		void
		SetCursorStateFlags(CursorState a_state) override;

		void*
		GetNativeWindowHandle() const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
