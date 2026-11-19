#pragma once

#include <Core/Containers/Optional.h>

#include "Mesh.h"

#include "Core/Containers/Array.h"
#include "Core/Containers/SharedPtr.h"

namespace Oyl
{
	class ModelDesc;

	using Model = SharedPtr<ModelDesc>;

	class RENDERING_EXPORT ModelDesc
	{
	public:
		ModelDesc() = default;

		explicit
		ModelDesc(StringView a_filename);

		explicit
		ModelDesc(const Array<Mesh>& a_meshes)
			: m_meshes(a_meshes) {}

		NO_COPY_ASSIGN(ModelDesc);
		NO_MOVE_ASSIGN(ModelDesc);

		~ModelDesc() = default;

		const Array<Mesh>&
		GetMeshes() const { return m_meshes; }

	private:
		Array<Mesh> m_meshes;
	};

	class IModelLoaderFactory
	{
	public:
		virtual
		~IModelLoaderFactory() = 0;

		virtual
		bool
		SupportsExtension(StringView a_extension) = 0;

		virtual
		bool
		LoadModelFromFile(StringView a_fileName, ModelDesc* a_modelDesc) = 0;
	};

	inline
	IModelLoaderFactory::~IModelLoaderFactory() {}
}
