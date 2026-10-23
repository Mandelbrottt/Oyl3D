#include "RenderGraph.h"

namespace Oyl
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
		if (m_renderPasses.Contains(id))
			return false;

		m_renderPasses.Emplace(id, a_pass);
		m_dirty = true;
		return true;
	}

	bool
	RenderGraph::RemoveRenderPass(RenderPassId a_pass)
	{
		if (!m_renderPasses.Contains(a_pass))
			return false;

		m_renderPasses.Erase(a_pass);
		m_dirty = true;
		return true;
	}

	RenderPassBase*
	RenderGraph::GetRenderPass(RenderPassId a_renderPassId) const
	{
		if (m_renderPasses.Contains(a_renderPassId))
			return m_renderPasses[a_renderPassId];
		return nullptr;
	}

	RenderPassBase*
	RenderGraph::GetRenderPass(StringView a_renderPassName) const
	{
		for (auto& [id, pass] : m_renderPasses)
			if (pass->GetName() == a_renderPassName)
				return pass;
		return nullptr;
	}

	void
	RenderGraph::Execute(const ICommandBuffer& a_commandBuffer)
	{
		SortPasses();

		for (auto& pass : m_sortedRenderPasses)
		{
			pass->Execute(a_commandBuffer);
		}
	}

	void
	RenderGraph::SortPasses()
	{
		if (!m_dirty)
			return;

		m_sortedRenderPasses.Clear();
		for (auto& [id, pass] : m_renderPasses)
			m_sortedRenderPasses.Add(pass);

		// TEMPORARY:
		std::swap(m_sortedRenderPasses[0], m_sortedRenderPasses[1]);

		m_dirty = false;
	}

	void
	RenderGraph::TopologicalSort(
		RenderPassId a_id,
		const ArrayMap<RenderPassId, RenderPassBase*>& a_passDict,
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
