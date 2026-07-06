#pragma once

#include "Core/Events/Event.h"

namespace Oyl
{
	struct ApplicationCloseRequestEvent : EventBase<ApplicationCloseRequestEvent> {};
}