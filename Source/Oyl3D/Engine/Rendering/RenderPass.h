#pragma once
#include <Core/Array.h>
#include <Core/Set.h>

namespace Oyl::Rendering
{
	class RenderTarget;
}

namespace Oyl::Rendering
{
	class CommandBuffer;
}

namespace Oyl::Rendering
{
	enum class RenderPassId : uint32 { Null = 0 };

	class RenderPassBase
	{
	protected:
		explicit
		RenderPassBase(std::string_view a_name)
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

		std::string_view
		GetName() const
		{
			return m_name;
		}

		virtual
		[[maybe_unused]] RenderPassBase&
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

		virtual
		[[maybe_unused]] RenderPassBase&
		AddDependency(RenderPassId a_id)
		{
			if (a_id == RenderPassId::Null)
				return *this;

			if (m_dependencies.Contains(a_id))
				return *this;

			m_dependencies.Add(a_id);
			return *this;
		}

		virtual
		[[maybe_unused]] RenderPassBase&
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

		virtual
		[[maybe_unused]] RenderPassBase&
		SetRenderTarget(const RenderTarget* a_renderTarget)
		{
			m_renderTarget = a_renderTarget;
			return *this;
		}

		virtual
		void
		Execute(const CommandBuffer* a_commandBuffer)
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
		Begin(const CommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Render(const CommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		End(const CommandBuffer& a_commandBuffer) = 0;

	private:
		RenderPassId m_id;
		std::string m_name;
		bool m_enabled = true;

		const RenderTarget* m_renderTarget = nullptr;
		Array<RenderPassId> m_dependencies;
	};

	template<typename TRenderPass>
	class RenderPass : public RenderPassBase
	{
	protected:
		RenderPass()
			: RenderPassBase(NameOf<TRenderPass>()) {}

	public:
		virtual
		~RenderPass() = default;
	};
}
