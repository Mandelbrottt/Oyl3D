#include "RenderGraph.h"

namespace Oyl
{
	RenderGraph::RenderGraph() {}

	RenderGraph::~RenderGraph() {}

	const Array<IRenderPass*>&
	RenderGraph::GetSortedRenderPasses() const
	{
		return m_sortedRenderPasses;
	}

	bool
	RenderGraph::AddRenderPass(IRenderPass* a_renderPass)
	{
		auto name = String(a_renderPass->GetName());
		if (m_renderPasses.ContainsKey(name))
			return false;

		m_renderPasses.Emplace(name, a_renderPass);
		m_dirty = true;
		return true;
	}

	bool
	RenderGraph::RemoveRenderPass(StringView a_name)
	{
		auto name = String(a_name);
		if (!m_renderPasses.ContainsKey(name))
			return false;

		m_renderPasses.Erase(name);
		m_dirty = true;
		return true;
	}

	IRenderPass*
	RenderGraph::GetRenderPass(StringView a_name) const
	{
		auto name = String(a_name);
		if (m_renderPasses.ContainsKey(name))
			return m_renderPasses[name];
		return nullptr;
	}

	void
	RenderGraph::Execute(const ICommandBuffer& a_commandBuffer)
	{
		if (m_dirty)
		{
			SortPasses();
			m_dirty = false;
		}

		for (auto& pass : m_sortedRenderPasses)
		{
			pass->Execute(a_commandBuffer);
		}
	}

	void
	RenderGraph::SortPasses()
	{
		m_sortedRenderPasses.Clear();

		ArrayMap<String, IRenderPass*> passMap;
		for (const auto& [name, pass] : m_renderPasses)
			passMap[name] = pass;

		ArraySet<StringView> visited;
		ArraySet<StringView> visiting;

		for (const auto& [name, pass] : passMap)
		{
			if (!visited.Contains(name))
				TopologicalSort(name, passMap, visited, visiting);
		}
	}

	void
	RenderGraph::TopologicalSort(
		const String& a_name,
		const IMap<String, IRenderPass*>& a_passMap,
		ISet<StringView>& a_visited,
		ISet<StringView>& a_visiting
	)
	{
		a_visiting.Add(a_name);

		auto pass = a_passMap[a_name];
		for (const auto& dependencyName : pass->GetDependencies())
		{
			if (!a_visited.Contains(dependencyName))
			{
				if (a_visiting.Contains(dependencyName))
					throw "Circular dependency detected in RenderGraph!";

				TopologicalSort(dependencyName, a_passMap, a_visited, a_visiting);
			}
		}

		a_visiting.Erase(a_name);
		a_visited.Add(a_name);
		m_sortedRenderPasses.Add(pass);
	}
}
