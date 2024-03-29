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

UMBRELLA_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
}

UMBRELLA_EXPORT_FUNC void late_load() {

    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    Umbrella::Hooking::InstallHooks();

    auto z = Lapiz::Zenject::Zenjector::Get();
    z->Install(Lapiz::Zenject::Location::Menu, [](Zenject::DiContainer* container){
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentAsViewController(container->Bind<Umbrella::BoardViewController*>())->AsSingle();
        container->BindInterfacesAndSelfTo<Umbrella::BoardSetup*>()->AsSingle()->NonLazy();
    });
}
