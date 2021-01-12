#include "helpers/registerTypeHelper.hpp"
#include "Point.hpp"
#include "Rect.hpp"

#define REGISTER_FUNC_DECL(COMP) void register##COMP##Component();

REGISTER_FUNC_DECL(Adjustable);
REGISTER_FUNC_DECL(Animation);
REGISTER_FUNC_DECL(AppearsInViewport);
REGISTER_FUNC_DECL(Camera);
REGISTER_FUNC_DECL(DebugGraphics);
REGISTER_FUNC_DECL(Execute);
REGISTER_FUNC_DECL(GodRays);
REGISTER_FUNC_DECL(Graphics);
REGISTER_FUNC_DECL(Highlight);
REGISTER_FUNC_DECL(ImGui);
REGISTER_FUNC_DECL(Input);
REGISTER_FUNC_DECL(Instance);
REGISTER_FUNC_DECL(Light);
REGISTER_FUNC_DECL(Lua);
REGISTER_FUNC_DECL(Model);
REGISTER_FUNC_DECL(ModelCollider);
REGISTER_FUNC_DECL(Name);
REGISTER_FUNC_DECL(NavMesh);
REGISTER_FUNC_DECL(OnClick);
REGISTER_FUNC_DECL(Pathfinding);
REGISTER_FUNC_DECL(Physics);
REGISTER_FUNC_DECL(Python);
REGISTER_FUNC_DECL(Selected);
REGISTER_FUNC_DECL(Skeleton);
REGISTER_FUNC_DECL(SkyBox);
REGISTER_FUNC_DECL(Sprite);
REGISTER_FUNC_DECL(Text);
REGISTER_FUNC_DECL(TimeModulator);
REGISTER_FUNC_DECL(Transform);
REGISTER_FUNC_DECL(UI);

#define REGISTER_FUNC_NAME(COMP) register##COMP##Component

using RegisterFunc = void(*)();
static const RegisterFunc funcs[] = {
	REGISTER_FUNC_NAME(Adjustable),
	REGISTER_FUNC_NAME(Animation),
	REGISTER_FUNC_NAME(AppearsInViewport),
	REGISTER_FUNC_NAME(Camera),
	REGISTER_FUNC_NAME(DebugGraphics),
	REGISTER_FUNC_NAME(Execute),
	REGISTER_FUNC_NAME(GodRays),
	REGISTER_FUNC_NAME(Graphics),
	REGISTER_FUNC_NAME(Highlight),
	REGISTER_FUNC_NAME(ImGui),
	REGISTER_FUNC_NAME(Input),
	REGISTER_FUNC_NAME(Instance),
	REGISTER_FUNC_NAME(Light),
	REGISTER_FUNC_NAME(Lua),
	REGISTER_FUNC_NAME(Model),
	REGISTER_FUNC_NAME(ModelCollider),
	REGISTER_FUNC_NAME(Name),
	REGISTER_FUNC_NAME(NavMesh),
	REGISTER_FUNC_NAME(OnClick),
	REGISTER_FUNC_NAME(Pathfinding),
	REGISTER_FUNC_NAME(Physics),
	REGISTER_FUNC_NAME(Python),
	REGISTER_FUNC_NAME(Selected),
	REGISTER_FUNC_NAME(Skeleton),
	REGISTER_FUNC_NAME(SkyBox),
	REGISTER_FUNC_NAME(Sprite),
	REGISTER_FUNC_NAME(Text),
	REGISTER_FUNC_NAME(TimeModulator),
	REGISTER_FUNC_NAME(Transform),
	REGISTER_FUNC_NAME(UI),
};

void registerTypes() {
	kengine::registerTypes<
		putils::Rect3f,
		putils::Color, putils::NormalizedColor
	>();

	for (const auto f : funcs)
		f();
}
