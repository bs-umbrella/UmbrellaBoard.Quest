#include "UI/Settings/SettingsFlowCoordinator.hpp"
#include "bsml/shared/BSML.hpp"

DEFINE_TYPE(Umbrella::UI::Settings, SettingsFlowCoordinator);

namespace Umbrella::UI::Settings {
    void SettingsFlowCoordinator::ctor() {
        INVOKE_CTOR();
        HMUI::FlowCoordinator::_ctor();
    }

    void SettingsFlowCoordinator::Inject(GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, CommunityConfigurationView* configurationView) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _configurationView = configurationView;
    }

    void SettingsFlowCoordinator::Initialize() {
        _menuButton = BSML::Register::RegisterMenuButton("Umbrella Communities", "Configure enabled and disabled communities here!", std::bind(&SettingsFlowCoordinator::OpenSettings, this));
    }

    void SettingsFlowCoordinator::Dispose() {
        BSML::Register::UnRegisterMenuButton(_menuButton);
    }

    void SettingsFlowCoordinator::OpenSettings() {
        auto fc = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        fc->PresentFlowCoordinator(this, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
    }

    void SettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            SetTitle("Umbrella Communities", HMUI::ViewController::AnimationType::In);
            ProvideInitialViewControllers(_configurationView, nullptr, nullptr, nullptr, nullptr);
            showBackButton = true;
        }
    }

    void SettingsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        _parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
    }
}
