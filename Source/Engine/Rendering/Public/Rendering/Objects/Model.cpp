#include "Model.h"

#include <Core/Modules/ModuleManager.h>

#include "Rendering/Rendering.h"

namespace Oyl
{
	ModelDesc::ModelDesc(StringView a_filename)
	{
		auto desc = ModuleManager::GetModulePointer<IRenderingModule>("Rendering")->CreateModel(a_filename);
		m_meshes = std::move(desc.m_meshes);
	}
}
