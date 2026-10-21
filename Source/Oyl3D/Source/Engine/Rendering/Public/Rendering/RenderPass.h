#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/Set.h>

#include "CommandBuffer.h"
#include "RenderTarget.h"

namespace Oyl
{
	enum class RenderPassId : uint32 { Null = 0 };

	class RENDERING_EXPORT RenderPassBase
	{
	protected:
		explicit
		RenderPassBase(StringView a_name)
			: m_name(a_name)
		{
			m_id = []
			{
				static auto underlyingNextId = Traits::TUnderlyingType<RenderPassId>(RenderPassId::Null);
				underlyingNextId++;
				return RenderPassId(underlyingNextId);
			}();
		}

		DEFAULT_MOVE(RenderPassBase);

	public:
		NO_COPY(RenderPassBase);

		virtual
		~RenderPassBase() = default;

		RenderPassId
		GetId() const
		{
			return m_id;
		}

		StringView
		GetName() const
		{
			return m_name;
		}

		[[maybe_unused]]
		virtual
		RenderPassBase&
		SetEnabled(bool a_enabled)
		{
			m_enabled = a_enabled;
			return *this;
		}

		virtual
		bool
		IsEnabled() const
		{
			return m_enabled;
		}

		[[maybe_unused]]
		virtual
		RenderPassBase&
		AddDependency(RenderPassId a_id)
		{
			if (a_id == RenderPassId::Null)
				return *this;

			if (m_dependencies.Contains(a_id))
				return *this;

			m_dependencies.Add(a_id);
			return *this;
		}

		[[maybe_unused]]
		virtual
		RenderPassBase&
		AddDependency(const RenderPassBase* a_renderPass)
		{
			return AddDependency(a_renderPass->GetId());
		}

		virtual
		const Array<RenderPassId>&
		GetDependencies() const
		{
			return m_dependencies;
		}

		virtual
		const RenderTarget*
		GetRenderTarget() const
		{
			return m_renderTarget;
		}

		[[maybe_unused]]
		virtual
		RenderPassBase&
		SetRenderTarget(const RenderTarget* a_renderTarget)
		{
			m_renderTarget = a_renderTarget;
			return *this;
		}

		virtual
		void
		Execute(const ICommandBuffer* a_commandBuffer)
		{
			if (!IsEnabled())
				return;

			Begin(*a_commandBuffer);
			Render(*a_commandBuffer);
			End(*a_commandBuffer);
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

	private:
		RenderPassId m_id;
		String m_name;
		bool m_enabled = true;

		const RenderTarget* m_renderTarget = nullptr;
		Array<RenderPassId> m_dependencies;
	};

	template<typename TRenderPass>
	class RenderPass : public RenderPassBase
	{
	protected:
		RenderPass()
			: RenderPassBase(nameof(TRenderPass)) {}

	public:
		virtual
		~RenderPass() = default;
	};
}
