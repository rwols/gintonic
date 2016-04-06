#include "gintonic.hpp"

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Global variable.
gintonic::Console gConsole;

// events
struct GoToMainMenu {};
struct GoToPauseMenu {};
struct GoToSimulation {};

// A "complicated" event type that carries some data.
enum DiskTypeEnum
{
	DISK_CD=0,
	DISK_DVD=1
};

struct cd_detected
{
	cd_detected(std::string name, DiskTypeEnum diskType)
	: name(name)
	, disc_type(diskType)
	{

	}

	std::string name;

	DiskTypeEnum disc_type;
};

struct EntryPoint : public boost::msm::front::state<>
{
	template <class Event, class FSM>
	void on_entry(const Event&, FSM&)
	{
		std::cout << "Entering EntryPoint state.\n";
	}

	template <class Event, class FSM>
	void on_exit(const Event&, FSM&)
	{
		std::cout << "Leaving EntryPoint state.\n";
	}
};

struct MainMenu : public boost::msm::front::state<>
{
	template <class Event, class FSM>
	void on_entry(const Event&, FSM&)
	{
		std::cout << "Entering MainMenu state.\n";
	}

	template <class Event, class FSM>
	void on_exit(const Event&, FSM&)
	{
		std::cout << "Leaving MainMenu state.\n";
	}
};

struct Simulation : public boost::msm::front::state<>
{
	template <class Event, class FSM>
	void on_entry(const Event&, FSM&)
	{
		std::cout << "Entering Simulation state.\n";
	}

	template <class Event, class FSM>
	void on_exit(const Event&, FSM&)
	{
		std::cout << "Leaving Simulation state.\n";
	}
};

struct PauseMenu : public boost::msm::front::state<>
{
	template <class Event, class FSM>
	void on_entry(const Event&, FSM&)
	{
		std::cout << "Entering PauseMenu state.\n";
	}

	template <class Event, class FSM>
	void on_exit(const Event&, FSM&)
	{
		std::cout << "Leaving PauseMenu state.\n";
	}
};

struct GameState_ : public boost::msm::front::state_machine_def<GameState_>
{
	template <class Event,class FSM>
	void on_entry(Event const& ,FSM&) 
	{
		std::cout << "Starting GameState FSM." << std::endl;
		gintonic::Renderer::initialize("ConsoleTest");
	}

	template <class Event,class FSM>
	void on_exit(Event const&,FSM& ) 
	{
		std::cout << "Stopping GameState FSM." << std::endl;
	}

	// the initial state of the GameState SM. Must be defined
	typedef EntryPoint initial_state;

	typedef GameState_ g; // makes transition table cleaner

	// Transition table for player
	struct transition_table : boost::mpl::vector<
	// --------START-STATE----EVENT--------TARGET-STATE-
		_row < EntryPoint, GoToMainMenu,   MainMenu   >,
		_row < MainMenu,   GoToSimulation, Simulation >,
		_row < Simulation, GoToMainMenu,   MainMenu   >,
		_row < Simulation, GoToPauseMenu,  PauseMenu  >,
		_row < PauseMenu,  GoToSimulation, Simulation >,
		_row < PauseMenu,  GoToMainMenu,   MainMenu   >
	> {};
};

typedef boost::msm::back::state_machine<GameState_> GameState;

// as global variable
GameState gGameState;

void exitHandler(const int /*signal*/)
{
	if (gConsole.size() > 0)
	{
		std::cout << "\n\tHistory of what you typed:\n\n";
		for (const auto& lHistoryLine : gConsole)
		{
			std::cout << lHistoryLine << '\n';
		}
	}

	exit(EXIT_SUCCESS);
}

int main()
{
	using namespace gintonic;

	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = exitHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, nullptr);

	gGameState.start();
	gGameState.process_event(GoToMainMenu());
	gGameState.process_event(GoToSimulation());
	gGameState.process_event(GoToPauseMenu());
	gGameState.process_event(GoToSimulation());
	gGameState.stop();

	std::cout << "\n\tWelcome! Type something that Lua can understand.\n";
	std::cout << "\tYou can exit with CTRL-C.\n\n";

	while (true)
	{
		std::string lLine;
		std::getline(std::cin, lLine);
		if (lLine.empty())
		{
			if (!gConsole.compileActiveString())
			{
				std::cerr << "\tFailed to compile line!\n\n";
			}
			else
			{
				std::cout << "\tSuccesfully compiled!\n\n";
			}
		}
		else
		{
			gConsole.appendChunk(lLine);
			std::cout << "\n\tCurrent string: " << gConsole.getActiveString() << "\n";
			std::cout << "\t(press enter to compile it)\n\n";	
		}
	}

	return EXIT_SUCCESS; // Make the compiler happy.
}