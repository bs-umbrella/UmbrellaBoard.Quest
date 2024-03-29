#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "BoardViewController.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella, BoardSetup, System::Object, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_CTOR(ctor, GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, BoardViewController* boardView);

    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MainFlowCoordinator*, _mainFlowCoordinator);
    DECLARE_INSTANCE_FIELD_PRIVATE(BoardViewController*, _boardView);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);
)
