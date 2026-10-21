#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/Set.h>
#include <Core/Preprocessor/Common.h>

#include "CommandBuffer.h"
#include "RenderPass.h"

namespace Oyl
{
	class RENDERING_EXPORT RenderGraph
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
		Execute(const ICommandBuffer& a_commandBuffer);

	private:
		void
		SortPasses();

		void
		TopologicalSort(
			RenderPassId a_id,
			const ArrayMap<RenderPassId, RenderPassBase*>& a_passDict,
			Set<RenderPassId>& a_visited,
			Set<RenderPassId>& a_visiting
		);

	private:
		bool m_dirty = true;
		ArrayMap<RenderPassId, RenderPassBase*> m_renderPasses;
		Array<RenderPassBase*> m_sortedRenderPasses;
	};
}
