#pragma once
#include <vector>
#include <memory>
#include <iostream>


namespace events
{
	// Base event handler class
	template <typename TSender, typename TEventArgs>
	struct EventHandlerBase
	{
		// Notify method.  Must be implemented by child EventHandlers.
		virtual void notify(TSender sender, TEventArgs e) const = 0;
		virtual ~EventHandlerBase() = default;
	};


	// Member-function handler class, with two parameters support
	template <typename TListener, typename TSender, typename TEventArgs>
	struct EventHandler : public EventHandlerBase<TSender, TEventArgs>
	{
		typedef void (TListener::*MemberFuncPtr)(TSender, TEventArgs);

		// .ctor..
		EventHandler(TListener* object, MemberFuncPtr member) : object_(object), memberfunc_(member) {}

		virtual ~EventHandler() { std::cout << "member eventhandler destroyed." << std::endl; }

		// Notifies listeners
		virtual void notify(TSender sender, TEventArgs e) const {
			return (object_->*memberfunc_)(sender, e);
		}


		// Determines if the given object and function are attached
		// Returns true/false accordingly
		bool handles(TListener* object, MemberFuncPtr member) const {
			return object_ == object && memberfunc_ == member;
		}

	private:
		TListener* object_;
		MemberFuncPtr memberfunc_;
	};


	// Non-member function handler class, with two parameters support
	template <typename TSender, typename TEventArgs>
	struct NonMemberEventHandler : public EventHandlerBase<TSender, TEventArgs>
	{
		typedef void(*FuncPtr)(TSender, TEventArgs);

		// .ctor
		NonMemberEventHandler(FuncPtr func) : func_(func) {}

		virtual ~NonMemberEventHandler() { std::cout << "nonmember eventhandler destroyed." << std::endl; }

		// Notifies listeners
		virtual void notify(TSender sender, TEventArgs e) const {
			return (*func_)(sender, e);
		}


		// Determines if the given object and function are attached
		// Returns true/false accordingly
		bool handles(FuncPtr func) const {
			return func_ == func;
		}

	private:
		FuncPtr func_;
	};

	// Event class - Supports an event with two parameters and no return value
	template <typename TSender, typename TEventArgs>
	class Event
	{
	private:
		std::vector<std::shared_ptr<EventHandlerBase<TSender, TEventArgs>>> handlers_;

	public:
		// Attaches the given object and method to the event so that it can listen for notifications.
		template <typename TListener>
		void attach(TListener& object, void (TListener::*member)(TSender, TEventArgs)) {
			handlers_.push_back(std::make_shared<EventHandler<TListener, TSender, TEventArgs>>(&object, member));
		}

		//Attaches a non-member event handler
		void attach(void(*func)(TSender, TEventArgs)) {
			handlers_.push_back(std::make_shared<NonMemberEventHandler<TSender, TEventArgs>>(func));
		}

		//Detaches (or unsubscribes) the given object and method from this event.
		template<typename TListener>
		bool detach(TListener& object, void (TListener::*member)(TSender, TEventArgs))
		{
			for (unsigned int i = 0; i < handlers_.size(); ++i)
			{
				auto handler = dynamic_cast<EventHandler<TListener, TSender, TEventArgs>*>(handlers_[i].get());

				if (handler && handler->handles(&object, member))
				{
					handlers_.erase(handlers_.begin() + i);
					return true;
				}
			}

			return false;
		}

		// Detaches (or unsubscribes) the given non-member function from this event
		bool detach(void(*func)(TSender, TEventArgs))
		{
			for (unsigned int i = 0; i < handlers_.size(); ++i)
			{
				auto handler = dynamic_cast<NonMemberEventHandler<TSender, TEventArgs>*>(handlers_[i].get());

				if (handler && handler->handles(func))
				{
					handlers_.erase(handlers_.begin() + i);
					return true;
				}
			}

			return false;
		}

		// Notifies attached objects of the occurrence of this event.
		void notify(TSender sender, TEventArgs e) const
		{
			if (handlers_.size() <= 0) return;

			for (unsigned int i = 0; i < handlers_.size(); ++i) {
				handlers_[i]->notify(sender, e);
			}
		}
	};

}
