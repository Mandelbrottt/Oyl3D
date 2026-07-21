#pragma once

#include <memory>

#include "Rendering/Window.h"

namespace Oyl::Glfw
{
	class OYL_RENDERING_API Window : public Oyl::Window
	{
	public:
		Window() noexcept;

		explicit
		Window(const WindowParams& a_params) noexcept;

		Window(Window&& a_other) noexcept;
		Window&
		operator=(Window&& a_other) noexcept;

		virtual
		~Window();

		void
		Init(const WindowParams& a_params) override;

		void
		Destroy() override;

		void
		Update() override;

		bool
		IsValid() const override;

		void
		SetPostEventCallback(PostEventDelegate a_delegate) override;

		Vector2i
		GetSize() const override;

		void
		SetSize(Vector2i a_size) override;

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
