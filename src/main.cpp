#include "Views/CommunitiesView.hpp"
#include "logging.hpp"
#include "hooking.hpp"
#include "_config.h"

#include "bsml/shared/BSML.hpp"
#include "lapiz/shared/zenject/Zenjector.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "custom-types/shared/register.hpp"

#include "Zenject/FromBinderNonGeneric.hpp"
#include "Zenject/ScopeConcreteIdArgConditionCopyNonLazyBinder.hpp"

#include "BoardSetup.hpp"
#include "BoardViewController.hpp"

#include "Views/CommunitiesView.hpp"
#include "Views/CommunityView.hpp"
#include "Views/PageView.hpp"

MAKE_HOOK(abort_hook, nullptr, void) {
    auto logger = Paper::ConstLoggerContext("Install_Abort");
    logger.Backtrace(40);
    abort_hook();
}

UMBRELLA_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
}

UMBRELLA_EXPORT_FUNC void late_load() {

    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    Umbrella::Hooking::InstallHooks();

    auto libc = dlopen("libc.so", RTLD_NOW);
    auto abrt = dlsym(libc, "abort");
    auto logger = Paper::ConstLoggerContext("Install_Abort");
    INSTALL_HOOK_DIRECT(logger, abort_hook, (void*)abrt);

    auto z = Lapiz::Zenject::Zenjector::Get();
    z->Install(Lapiz::Zenject::Location::Menu, [](Zenject::DiContainer* container){
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Umbrella::Views::CommunitiesView*>())->AsSingle();
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Umbrella::Views::CommunityView*>())->AsSingle();
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Umbrella::Views::PageView*>())->AsSingle();
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Umbrella::Views::PlaceHolderView*>())->AsSingle();
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Umbrella::BoardViewController*>())->AsSingle();
        container->BindInterfacesAndSelfTo<Umbrella::BoardSetup*>()->AsSingle()->NonLazy();
    });
}
