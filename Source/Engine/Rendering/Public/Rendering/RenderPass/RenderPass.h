#pragma once

#include <Core/Containers/ArraySet.h>
#include <Core/Containers/Set.h>

#include "Rendering/CommandBuffer.h"
#include "Rendering/RenderTarget.h"

namespace Oyl
{
	class IRenderPass
	{
	public:
		virtual
		~IRenderPass() = 0;

		virtual
		void
		Execute(const ICommandBuffer& a_commandBuffer)
		{
			Begin(a_commandBuffer);
			Render(a_commandBuffer);
			End(a_commandBuffer);
		}

	protected:
		virtual
		void
		Begin(const ICommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Render(const ICommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		End(const ICommandBuffer& a_commandBuffer) = 0;

	public:
		virtual
		StringView
		GetName() const = 0;

		virtual
		const RenderTarget&
		GetRenderTarget() const = 0;

		virtual
		const ISet<String>&
		GetDependencies() const = 0;

		virtual
		void
		AddDependency(StringView a_name) = 0;

		virtual
		void
		AddDependency(IRenderPass& a_renderPass) { AddDependency(a_renderPass.GetName()); };

		virtual
		void
		RemoveDependency(StringView a_name) = 0;

		virtual
		void
		RemoveDependency(IRenderPass& a_renderPass) { RemoveDependency(a_renderPass.GetName()); };
	};

	inline
	IRenderPass::~IRenderPass() {}

	class RENDERING_EXPORT RenderPassBase : public IRenderPass
	{
	protected:
		explicit
		RenderPassBase(StringView a_name)
			: m_name(a_name) {}

		DEFAULT_MOVE(RenderPassBase);

	public:
		NO_COPY(RenderPassBase);

		virtual
		~RenderPassBase() = 0;

		StringView
		GetName() const override { return m_name; }

		const RenderTarget&
		GetRenderTarget() const override { return m_renderTarget; }

		const ISet<String>&
		GetDependencies() const override
		{
			return m_dependencies;
		}

		void
		AddDependency(StringView a_name) override
		{
			if (!m_dependencies.Contains(a_name))
				m_dependencies.Add(String(a_name));
		}

		void
		RemoveDependency(StringView a_name) override
		{
			if (!m_dependencies.Contains(a_name))
				m_dependencies.Erase(String(a_name));
		}

	protected:
		RenderTarget m_renderTarget;

	private:
		String m_name;
		ArraySet<String> m_dependencies;
	};

	inline
	RenderPassBase::~RenderPassBase() {}

	template<typename TRenderPass = void>
	class RenderPass : public RenderPassBase
	{
		static_assert(Traits::Same<TRenderPass, void> || Traits::ConvertibleTo<TRenderPass*, IRenderPass*>);

	protected:
		template<typename = std::enable_if_t<!Traits::Same<TRenderPass, void>>>
		RenderPass()
			: RenderPassBase(nameof(TRenderPass)) {}

		explicit
		RenderPass(StringView a_name)
			: RenderPassBase(a_name) {}

		virtual
		~RenderPass() = 0;
	};

	template<typename TRenderPass>
	RenderPass<TRenderPass>::~RenderPass() {}
}
