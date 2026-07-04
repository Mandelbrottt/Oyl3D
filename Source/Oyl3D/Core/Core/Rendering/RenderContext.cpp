#include "RenderContext.h"

namespace Oyl::Rendering
{
	struct RenderContext::Impl
	{

	};

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const RenderContextParams& a_params) noexcept
		: RenderContext()
	{
		Init(a_params);
	}

	RenderContext::RenderContext(RenderContext&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	RenderContext&
	RenderContext::operator=(RenderContext&& a_other) noexcept
	{
		std::swap(m_impl, a_other.m_impl);
		return *this;
	}

	RenderContext::~RenderContext() noexcept
	{
		if (!m_impl)
			return;

		Destroy();

		delete m_impl;
		std::memset(m_impl, 0, sizeof(Impl));
		m_impl = nullptr;
	}

	void
	RenderContext::Init(const RenderContextParams& a_params)
	{
	}

	void
	RenderContext::Update()
	{
	}

	void
	RenderContext::Destroy()
	{

	}
}
