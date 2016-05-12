#include "gintonic.hpp"
#include "Application.hpp"
#include "Graphics/GUI/Panel.hpp"
#include "Graphics/GUI/StringView.hpp"
#include "Graphics/GUI/StringPointerView.hpp"
#include "Graphics/GUI/ConsoleView.hpp"

std::string gString = R"xxx(The current button state is returned as a button bitmask, which can be tested using the SDL_BUTTON(X) macros (where X is generally 1 for the left, 2 for middle, 3 for the right button), and x and y are set to the mouse cursor position relative to the focus window for the currently selected mouse. You can pass NULL for either x or y.)xxx";
class LuaConsole : public Application
{
private:
	gintonic::Console mConsole;
	gintonic::GUI::Panel* mGUIRoot = nullptr;
public:

	LuaConsole(const int argc, char** argv)
	: Application("LuaConsole", argc, argv)
	{
		using namespace gintonic;

		mGUIRoot = new GUI::Panel();
		mGUIRoot->box.minCorner = vec2f(-0.5f, -0.5f / Renderer::aspectRatio());
		mGUIRoot->box.maxCorner = vec2f( 0.5f, 0.5f / Renderer::aspectRatio());
		// mGUIRoot->box = box2f(vec2f(-1.0f, -1.0f), vec2f(1.0f, 1.0f));
		mGUIRoot->color = vec4f(0.3f, 0.3f, 0.3f, 0.0f);
		mGUIRoot->texture = std::shared_ptr<Texture2D>(new Texture2D("Resources/DaVinci.jpg"));
		mGUIRoot->onMouseEnter.connect([&]()
		{
			mGUIRoot->color = vec4f(0.3f, 0.0f, 0.0f, 0.1f);
		});
		mGUIRoot->onMouseLeave.connect([&]()
		{
			mGUIRoot->color = vec4f(0.0f, 0.3f, 0.0f, 0.1f);
		});

		auto lConsoleView = new GUI::ConsoleView();

		lConsoleView->console = &mConsole;
		lConsoleView->font    = std::shared_ptr<Font>(new Font("Resources/Inconsolata-Regular.ttf", 24));
		lConsoleView->box     = mGUIRoot->box;
		lConsoleView->color   = vec4f(1.0f, 1.0f, 1.0f, 1.0f);

		mGUIRoot->addChild(lConsoleView);

		Renderer::setGUIRoot(mGUIRoot);

		mConsole.appendChunk("a=5");
		mConsole.compileActiveString();
		mConsole.appendChunk("print(a)");
		mConsole.compileActiveString();

		Renderer::setFreeformCursor(false);
		Renderer::show();
		Renderer::beginTextInput();
	}

	virtual ~LuaConsole() noexcept
	{
		if (mGUIRoot)
		{
			delete mGUIRoot;
			mGUIRoot = nullptr;
		}
	}

private:

	virtual void onRenderUpdate() noexcept final
	{

	}

};

DEFINE_MAIN(LuaConsole);