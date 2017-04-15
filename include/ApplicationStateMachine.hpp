#pragma once

#include "Signal.hpp"
#include "State.hpp"
#include "StateEvent.hpp"
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>

namespace gintonic
{

class RunLoop;
struct InitializeState;
struct SplashScreenState;
struct MainMenuState;

struct ApplicationStateMachine
    : boost::statechart::state_machine<ApplicationStateMachine, InitializeState>
{
    ApplicationStateMachine(RunLoop& runLoop);
    RunLoop& runLoop;
};

// struct SplashScreenState;

struct InitializeState : State<InitializeState, ApplicationStateMachine>
{
    InitializeState(my_context ctx);
    using reactions =
        boost::statechart::transition<EvContinue, SplashScreenState>;
};

struct SplashScreenState : State<SplashScreenState, ApplicationStateMachine>
{
    SplashScreenState(my_context ctx);

    using reactions = boost::mpl::list<
        boost::statechart::custom_reaction<EvUpdate>,
        boost::statechart::transition<EvContinue, MainMenuState>>;

    boost::statechart::result react(const EvUpdate&);

  private:
    float mStart;
};

struct MainMenuState : State<MainMenuState, ApplicationStateMachine>
{
    MainMenuState(my_context ctx);
    boost::signals2::scoped_connection keyHandler;
    using reactions = boost::statechart::transition<EvContinue, MainMenuState>;
};

} // gintonic
