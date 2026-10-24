#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/Set.h>
#include <Core/Preprocessor/Common.h>

#include "CommandBuffer.h"

#include "Rendering/RenderPass/RenderPass.h"

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
		const Array<IRenderPass*>&
		GetSortedRenderPasses() const;

		virtual
		bool
		AddRenderPass(IRenderPass* a_renderPass);

		virtual
		bool
		RemoveRenderPass(StringView a_name);

		virtual
		bool
		RemoveRenderPass(IRenderPass* a_pass)
		{
			return RemoveRenderPass(a_pass->GetName());
		}

		virtual
		IRenderPass*
		GetRenderPass(StringView a_name) const;

		virtual
		void
		Execute(const ICommandBuffer& a_commandBuffer);

	private:
		void
		SortPasses();

		void
		TopologicalSort(
			const String& a_name,
			const IMap<String, IRenderPass*>& a_passMap,
			ArraySet<StringView>& a_visited,
			ArraySet<StringView>& a_visiting
		);

	private:
		bool m_dirty = true;
		ArrayMap<String, IRenderPass*> m_renderPasses;
		Array<IRenderPass*> m_sortedRenderPasses;
	};
}
