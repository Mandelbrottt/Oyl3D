#pragma once

#include <Core/Containers/Optional.h>

#include "Mesh.h"

#include "Core/Containers/Array.h"
#include "Core/Containers/SharedPtr.h"

namespace Oyl
{
	class Model
	{
	public:
		Model() = default;

		explicit
		Model(const Array<Mesh>& a_meshes)
			: m_meshes(a_meshes) {}

		NO_COPY(Model);
		DEFAULT_MOVE(Model);

		~Model() = default;

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
		Optional<Model>
		LoadModelFromFile(StringView a_fileName) = 0;
	};

	inline
	IModelLoaderFactory::~IModelLoaderFactory() {}
}
