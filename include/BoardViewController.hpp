#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/NavigationController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "UI/Views/CommunitiesView.hpp"
#include "UI/Views/PageView.hpp"
#include "UI/Views/CommunityConfigurationView.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella, BoardViewController, HMUI::NavigationController, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
        DECLARE_CTOR(ctor);

        DECLARE_INJECT_METHOD(void, Inject, GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, UI::Views::CommunitiesView* communitiesView, UI::Views::PageView* pageView, UI::Views::CommunityConfigurationView* communityConfigurationView);

        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MainFlowCoordinator*, _mainFlowCoordinator);
        DECLARE_INSTANCE_FIELD_PRIVATE(UI::Views::CommunitiesView*, _communitiesView);
        DECLARE_INSTANCE_FIELD_PRIVATE(UI::Views::PageView*, _pageView);
        DECLARE_INSTANCE_FIELD_PRIVATE(UI::Views::CommunityConfigurationView*, _communityConfigurationView);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ViewController*, _activeViewController);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ViewController*, _viewControllerToPresent);

        DECLARE_INSTANCE_FIELD_PRIVATE(System::Action*, _presentAfterPopAction);
        DECLARE_INSTANCE_FIELD_PRIVATE(System::Action*, _finishPushAction);

        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _navButtons);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _openSettings);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _closeSettings);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _headerContent);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _headerText);

        DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
        DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);
        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::NavigationController::DidActivate, bool fistActivation, bool addedToHierarchy, bool screenSystemEnabling);

        DECLARE_INSTANCE_METHOD(void, BackPage);
        DECLARE_INSTANCE_METHOD(void, RefreshPage);
        DECLARE_INSTANCE_METHOD(void, OpenCommunityConfig);
        DECLARE_INSTANCE_METHOD(void, CloseCommunityConfig);

        DECLARE_INSTANCE_METHOD(void, DiscoverCommunities);
        DECLARE_INSTANCE_METHOD(void, PostParse);
    private:
        /// @brief pops a view if there was one active, and pushes this view onto the navigation controller
        void SwitchDisplayedView(HMUI::ViewController* targetView);

        void PresentViewControllerAfterPop();

        void FinishPushingViewController();

        /// @brief user selected a community in the left view
        void CommunityWasSelected(std::string_view communityURL);
)
