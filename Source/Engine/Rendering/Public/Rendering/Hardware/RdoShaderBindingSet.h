#pragma once

#include <Core/Containers/Array.h>

#include "RenderDeviceObject.h"

#include "Rendering/ShaderBindings.h"

namespace Oyl
{
	class IRdoShader;

	struct RdoShaderBindingSetId : IRenderDeviceObjectId<RdoShaderBindingSetId> {};

	struct RdoShaderBindingSetCreateParams
	{
		const ArrayProxy<ShaderBufferBinding>& bindings;
	};

	class IRdoShaderBindingSet : public IRenderDeviceObject<RdoShaderBindingSetId>
	{
	protected:
		IRdoShaderBindingSet() = default;
		DEFAULT_MOVE(IRdoShaderBindingSet);

	public:
		NO_COPY(IRdoShaderBindingSet);

		virtual
		~IRdoShaderBindingSet() = 0;

		virtual
		const Array<ShaderBufferBinding>&
		GetBufferBindings() const = 0;

		virtual
		void*
		GetUnderlyingBuffer() = 0;

		const void*
		GetUnderlyingBuffer() const { return const_cast<IRdoShaderBindingSet*>(this)->GetUnderlyingBuffer(); }

		virtual
		void
		CopyToBuffer(const void* a_data, uint64 a_size, uint64 a_offset = 0) = 0;

		template<typename TData>
			requires !std::is_void_v<TData>
		void
		CopyToBuffer(const TData* a_data, uint64 a_offset = 0)
		{
			CopyToBuffer(static_cast<const void*>(a_data), sizeof(TData), a_offset);
		}
	};

	inline
	IRdoShaderBindingSet::~IRdoShaderBindingSet() {}

	using RdoShaderBindingSetHandle = IRenderDeviceObjectHandle<IRdoShaderBindingSet>;
}
