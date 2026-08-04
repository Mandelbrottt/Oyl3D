#include "RenderGraph.h"

namespace Oyl::Rendering
{
	RenderGraph::RenderGraph() {}

	RenderGraph::~RenderGraph() {}

	const Array<RenderPassBase*>&
	RenderGraph::GetSortedRenderPasses() const
	{
		return m_sortedRenderPasses;
	}

	bool
	RenderGraph::AddRenderPass(RenderPassBase* a_pass)
	{
		auto id = a_pass->GetId();
		if (m_renderPasses.ContainsKey(id))
			return false;

		m_renderPasses.Assign(id, a_pass);
		return true;
	}

	bool
	RenderGraph::RemoveRenderPass(RenderPassId a_pass)
	{
		if (!m_renderPasses.ContainsKey(a_pass))
			return false;

		m_renderPasses.Erase(a_pass);
		return true;
	}

	void
	RenderGraph::Execute(const CommandBufferImpl& a_commandBuffer)
	{
		if (m_dirty)
			SortPasses();

		for (auto& pass : m_sortedRenderPasses)
		{
			pass->Execute(&a_commandBuffer);
		}
	}

	void
	RenderGraph::SortPasses()
	{
		m_sortedRenderPasses.Clear();
		m_sortedRenderPasses.Add(m_renderPasses.At(static_cast<RenderPassId>(1)));
	}

	void
	RenderGraph::TopologicalSort(
		RenderPassId a_id,
		const Dictionary<RenderPassId, RenderPassBase*>& a_passDict,
		Set<RenderPassId>& a_visited,
		Set<RenderPassId>& a_visiting
	)
	{
		OYL_UNUSED(a_id);
		OYL_UNUSED(a_passDict);
		OYL_UNUSED(a_visited);
		OYL_UNUSED(a_visiting);
		//TODO_IMPLEMENT_ME();
	}
}
