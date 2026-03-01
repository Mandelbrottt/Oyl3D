#pragma once
//
//struct Event
//{
//	virtual ~Event() = default;
//
//	virtual int GetType() = 0;
//
//	int a;
//};
//
//struct DerivedEvent1 : Event
//{
//	static int GetStaticType() { return 1; }
//
//	int GetType() override { return GetStaticType(); }
//
//	int b;
//};
//
//struct DerivedEvent2 : Event
//{
//	static int GetStaticType() { return 2; }
//
//	int GetType() override { return GetStaticType(); }
//
//	int c;
//};
//
//class Dispatcher
//{
//public:
//	explicit
//	Dispatcher(Event& a_event)
//		: event { a_event } {}
//
//	using EventFn = std::function<void(Event&)>;
//
//	Event& event;
//
//	void
//	Dispatch(int a_type, EventFn a_fn) const
//	{
//		if (a_type == event.GetType())
//		{
//			a_fn(event);
//		}
//	}
//};
//
//class BaseReceiver
//{
//	std::unordered_map<int, Dispatcher::EventFn> m_eventFns;
//
//public:
//	virtual ~BaseReceiver() = default;
//
//	template<typename TClass, typename TEvent>
//	void
//	RegisterEvent(void (TClass::*a_fn)(TEvent&))
//	{
//		// Use typeid hash instead of GetStaticType in the future
//		m_eventFns[TEvent::GetStaticType()] =
//			std::bind(
//				reinterpret_cast<void(BaseReceiver::*)(Event&)>(a_fn),
//				this,
//				std::placeholders::_1
//			);
//	}
//
//	void
//	OnEvent(Event& a_event)
//	{
//		Dispatcher dispatcher(a_event);
//		for (auto& [type, fn] : m_eventFns)
//		{
//			dispatcher.Dispatch(type, fn);
//		}
//	}
//};
//
//class Receiver : public BaseReceiver
//{
//public:
//	void
//	OnDerived1(DerivedEvent1& a_b)
//	{
//		printf("a: %d\tb: %d\n", a_b.a, a_b.b);
//	}
//
//	void
//	OnDerived2(DerivedEvent2& a_b)
//	{
//		printf("a: %d\tc: %d\n", a_b.a, a_b.c);
//	}
//};
//
//void
//Init()
//{
//	Receiver receiver;
//	receiver.RegisterEvent(&Receiver::OnDerived1);
//	receiver.RegisterEvent(&Receiver::OnDerived2);
//
//	DerivedEvent1 e1;
//	e1.a = 3;
//	e1.b = 4;
//
//	DerivedEvent2 e2;
//	e2.a = 5;
//	e2.c = 6;
//
//	receiver.OnEvent(e1);
//	receiver.OnEvent(e2);
//}
