#pragma once

#include <memory>

namespace sf
{
	class RenderWindow;
}

template<typename Owner, typename StateId>
class State
{
public:
	virtual ~State() = default;
	virtual void enter(Owner&) = 0;
	virtual void exit(Owner&) = 0;
	virtual void update(Owner&, float dt) = 0;
	virtual void draw(Owner&, sf::RenderWindow&) = 0;
	virtual StateId stateId() const = 0;
};


template<typename Owner, typename StateId>
class StateMachine
{
public:
	using StateType = State<Owner, StateId>;

	void update(Owner& owner, float dt)
	{
		if (m_currentState)
		{
			m_currentState->update(owner, dt);
		}
	}

	void draw(Owner& owner, sf::RenderWindow& window)
	{
		if (m_currentState)
		{
			m_currentState->draw(owner, window);
		}
	}

	void changeState(Owner& owner, std::unique_ptr<StateType> newState)
	{
		if (m_currentState)
		{
			m_currentState->exit(owner);
		}

		m_currentState = std::move(newState);

		if (m_currentState)
		{
			m_currentState->enter(owner);
		}
	}

	StateType& currentState() const
	{
		return *m_currentState;
	}

private:
	std::unique_ptr<StateType> m_currentState;
};

template<typename T>
class Singleton
{
public:
	static T& instance()
	{
		static T instance;
		return instance;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;
	~Singleton() = default;
};


class Player;
enum class PlayerStateId;
using PlayerState = State<Player, PlayerStateId>;

enum class HorizontalDirection
{
	Right,
	Left
};
