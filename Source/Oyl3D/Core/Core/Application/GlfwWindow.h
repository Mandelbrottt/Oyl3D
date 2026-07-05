#pragma once

#include "Window.h"

#include <memory>

namespace Oyl
{
	class OYL_CORE_API GlfwWindow : public Window
	{
	public:
		GlfwWindow() noexcept;

		explicit
		GlfwWindow(const WindowParams& a_params) noexcept;

		GlfwWindow(GlfwWindow&& a_other) noexcept;
		GlfwWindow&
		operator=(GlfwWindow&& a_other) noexcept;

		virtual
		~GlfwWindow();

		void
		Init(const WindowParams& a_params) override;

		void
		Destroy() override;

		void
		Update() override;

		bool
		IsValid() const override;

		void
		SetEventCallback(EventDelegate a_delegate) override;

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
		void
		CreateGlfwWindow();

		void
		SetupGlfwWindowCallbacks();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}