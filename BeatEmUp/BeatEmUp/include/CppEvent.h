#pragma once
#include <vector>


namespace util
{

	/// Base event arguments class - used by CppEvent objects to 
	/// to pass parameters to event handlers.
	/// Must inherit, or pass 'Empty' if no arguments required.
	class EventArgs
	{
	protected:
		EventArgs(){}
	public:
		~EventArgs(){}

		/// No parameters
		const static int Empty = 0;
	};

	/// Base event handler class
	template <typename TSender, typename TEventArgs> class EventHandlerBase
	{
	public:
		/// Notify method.  Must be implemented by child EventHandlers.
		virtual void notify(TSender sender, TEventArgs e) const = 0;
		virtual ~EventHandlerBase(){}
	};


	/// Generic handler class, with two parameters support
	template <typename TListener, typename TSender, typename TEventArgs> 
	class EventHandler : public EventHandlerBase<TSender, TEventArgs>
	{
	private:
		typedef void (TListener::*MemberFuncPtr)(TSender, TEventArgs);
		TListener* object_;
		MemberFuncPtr memberfunc_;

	public:
		/// .ctor..
		EventHandler(TListener* object, MemberFuncPtr member)
			: object_(object), memberfunc_(member){}

		virtual ~EventHandler() { }

		/// Notifies listeners
		virtual void notify(TSender sender,TEventArgs e) const
		{
			return (object_->*memberfunc_)(sender, e);		
		}	

		/// Determines if the given object and function are attached
		/// Returns true/false accordingly
		bool handles(TListener* object, MemberFuncPtr member) const
		{
			return object_ == object && memberfunc_ == member;
		}
	};


	class Uncopyable
	{
	protected:
		Uncopyable () {}
		~Uncopyable () {} /// Protected non-virtual destructor
	private: 
		Uncopyable (const Uncopyable&);
		Uncopyable & operator = (const Uncopyable&);
	};

	/// CppEvent class - Supports an event with two parameters and no return value
	template <typename TSender, typename TEventArgs> 
	class CppEvent : private Uncopyable
	{
	private:
		typedef std::vector<EventHandlerBase<TSender, TEventArgs>*> HandlersVector;
		HandlersVector handlers_;

	public:
		/// .ctor..
		CppEvent(){}

		~CppEvent()
		{
			for(unsigned int i = 0; i < handlers_.size(); ++i)
			{
				delete handlers_[i];
			}
		}

		/// Attaches the given object and method to the event so that it can listen for notifications.
		template <typename TListener> 
		void attach(TListener* object, void (TListener::*member)(TSender,TEventArgs))
		{
			handlers_.push_back(new EventHandler<TListener, TSender, TEventArgs>(object,member));
		}

		/// Detaches (or unsubscribes) the given object and method from this event.
		template<typename TListener>
		bool detach(TListener* object, void (TListener::*member)(TSender, TEventArgs))
		{
			for(unsigned int i = 0; i < handlers_.size(); ++i)
			{
				EventHandler<TListener, TSender, TEventArgs>* h 
					= dynamic_cast<EventHandler<TListener, TSender, TEventArgs>*>(handlers_[i]);
				if(h->handles(object, member)){
					handlers_.erase(handlers_.begin() + i);
					return true;
				}
			}

			return false;		
		}

		/// Notifies attached objects of the occurrence of this event.
		void notify(TSender sender, TEventArgs e) const
		{
			if(handlers_.size() <= 0) return;

			for(unsigned int i = 0; i < handlers_.size(); ++i){
				handlers_[i]->notify(sender, e);
			}
		}
	};

}
