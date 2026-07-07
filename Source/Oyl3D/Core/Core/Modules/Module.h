#pragma once

#include "ModuleRegistry.h"

#include "Core/Common.h"
#include "Core/Events/EventListener.h"
#include "Core/Logging/Logging.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	class EventDispatcher;
	class ModuleRegistry;
}

namespace Oyl
{
	class OYL_CORE_API Module : public EventListener
	{
		friend ModuleRegistry;

	public:
		Module();

		virtual
		~Module();

	#pragma region Public Interface

		virtual
		Reflection::TypeId
		GetTypeId() const = 0;

		virtual
		std::string_view
		GetName() const = 0;

		virtual
		bool
		IsEnabled() { return m_enabled; }

		virtual
		void
		SetEnabled(bool a_value) { m_enabled = a_value; }

	#pragma endregion

	#pragma region Callback functions

		virtual
		void
		OnInit() {}

		virtual
		void
		OnUpdate() {}

		virtual
		void
		OnShutdown() {}

	#pragma endregion

	private:
		bool m_enabled = true;
	};
}

#define OYL_DECLARE_MODULE(...) OYL_MACRO_OVERLOAD(_OYL_DECLARE_MODULE, __VA_ARGS__)

#define _OYL_DECLARE_MODULE_2(_class_, _parent_) \
	OYL_FORCE_FORMAT_INDENT \
private: \
	using This = _class_; \
	using Super = _parent_; \
	friend ::Oyl::ModuleRegistry; \
public: \
	constexpr ::Oyl::Reflection::TypeId \
	GetTypeId() const override \
	{ \
		return ::Oyl::Reflection::GetTypeId<decltype(*this)>(); \
	} \
	\
	constexpr std::string_view \
	GetName() const override\
	{ \
		return ::Oyl::NameOf<decltype(*this)>(); \
	} \
	\
private: \
	OYL_FORCE_SEMICOLON

#define _OYL_DECLARE_MODULE_1(_class_) _OYL_DECLARE_MODULE_2(_class_, ::Oyl::Module)
