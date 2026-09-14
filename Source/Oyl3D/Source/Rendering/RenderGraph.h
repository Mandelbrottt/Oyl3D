#pragma once
#include <Core/Array.h>
#include <Core/Dictionary.h>

#include "RenderPass.h"

namespace Oyl::Rendering
{
	class CommandBuffer;
}

namespace Oyl::Rendering
{
	class OYL_RENDERING_API RenderGraph
	{
	public:
		RenderGraph();

		NO_COPY(RenderGraph);
		DEFAULT_MOVE(RenderGraph);

		virtual
		~RenderGraph();

		virtual
		const Array<RenderPassBase*>&
		GetSortedRenderPasses() const;

		virtual
		bool
		AddRenderPass(RenderPassBase* a_pass);

		virtual
		bool
		RemoveRenderPass(RenderPassBase* a_pass)
		{
			return RemoveRenderPass(a_pass->GetId());
		}

		virtual
		bool
		RemoveRenderPass(RenderPassId a_pass);

		virtual
		void
		Execute(const CommandBuffer& a_commandBuffer);

	private:
		void
		SortPasses();

		void
		TopologicalSort(
			RenderPassId a_id,
			const Dictionary<RenderPassId, RenderPassBase*>& a_passDict,
			Set<RenderPassId>& a_visited,
			Set<RenderPassId>& a_visiting
		);

	private:
		bool m_dirty = true;
		Dictionary<RenderPassId, RenderPassBase*> m_renderPasses;
		Array<RenderPassBase*> m_sortedRenderPasses;
	};
}
