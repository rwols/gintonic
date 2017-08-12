#pragma once

#include "Scene.hpp"
#include "Signal.hpp"
#include "State.hpp"
#include "StateEvent.hpp"
#include "gintonic_export.h"
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>

namespace gintonic {

class RunLoop;
struct InitializeState;
struct SplashScreenState;
struct MainMenuState;

struct GINTONIC_EXPORT ApplicationStateMachine
    : boost::statechart::state_machine<ApplicationStateMachine, InitializeState>
{
    ApplicationStateMachine(RunLoop& runLoop);
    RunLoop& runLoop;
};

// struct SplashScreenState;

struct GINTONIC_EXPORT InitializeState
    : State<InitializeState, ApplicationStateMachine>
{
    InitializeState(my_context ctx);
    using reactions =
        boost::statechart::transition<EvContinue, SplashScreenState>;
};

struct GINTONIC_EXPORT SplashScreenState
    : State<SplashScreenState, ApplicationStateMachine>
{
    SplashScreenState(my_context ctx);

    using reactions = boost::mpl::list<
        boost::statechart::custom_reaction<EvUpdate>,
        boost::statechart::transition<EvContinue, MainMenuState>>;

    boost::statechart::result react(const EvUpdate&);

  private:
    float mStart;
};

struct GINTONIC_EXPORT MainMenuState
    : State<MainMenuState, ApplicationStateMachine>
{
    MainMenuState(my_context ctx);
    boost::signals2::scoped_connection keyHandler;
    using reactions = boost::mpl::list<
        boost::statechart::custom_reaction<EvUpdate>,
        boost::statechart::transition<EvContinue, MainMenuState>>;

    boost::statechart::result react(const EvUpdate&);

    Scene scene;
};

} // namespace gintonic
