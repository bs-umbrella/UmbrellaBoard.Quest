#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "CommunityConfigurationView.hpp"
#include "bsml/shared/BSML/MenuButtons/MenuButton.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella::UI::Settings, SettingsFlowCoordinator, HMUI::FlowCoordinator, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD_PRIVATE(BSML::MenuButton*, _menuButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MainFlowCoordinator*, _mainFlowCoordinator);
        DECLARE_INSTANCE_FIELD_PRIVATE(CommunityConfigurationView*, _configurationView);

        DECLARE_INJECT_METHOD(void, Inject, GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, CommunityConfigurationView* configurationView);

        DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
        DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);

        DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);
        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    private:
        void OpenSettings();
)
