#include "BoardViewController.hpp"
#include "LoadingControl.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"
#include "logging.hpp"

#include "HMUI/Touchable.hpp"

#include <chrono>
#include <future>

DEFINE_TYPE(Umbrella, BoardViewController);

#ifndef COMMUNITIES_URL
#define COMMUNITIES_URL "file:///sdcard/ModData/com.beatgames.beatsaber/Mods/UmbrellaBoard/communities.json"
#endif

namespace Umbrella {
    void BoardViewController::ctor() {
        INVOKE_CTOR();
        HMUI::NavigationController::_ctor();

        _presentAfterPopAction = BSML::MakeSystemAction<>(std::bind(&BoardViewController::PresentViewControllerAfterPop, this));
        _finishPushAction = BSML::MakeSystemAction<>(std::bind(&BoardViewController::FinishPushingViewController, this));
        this->_alignment = HMUI::NavigationController::Alignment::Beginning;
    }

    void BoardViewController::Inject(GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, Views::CommunitiesView* communitiesView, Views::PageView* pageView) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _communitiesView = communitiesView;
        _pageView = pageView;
    }

    void BoardViewController::Initialize() {
        _communitiesView->CommunityWasSelected += {&BoardViewController::CommunityWasSelected, this};
        _pageView->HistoryWasCleared += {&BoardViewController::StartRefreshContent, this};
    }

    void BoardViewController::Dispose() {
        _communitiesView->CommunityWasSelected -= {&BoardViewController::CommunityWasSelected, this};
        _pageView->HistoryWasCleared -= {&BoardViewController::StartRefreshContent, this};
    }

    void BoardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        INFO("BoardViewController DidActivate");

        // if first activation, parse the default content so it's visible
        if (firstActivation) {
            _mainFlowCoordinator->SetViewControllersToNavigationController(this, {static_cast<HMUI::ViewController*>(_communitiesView), static_cast<HMUI::ViewController*>(_pageView)});
            _activeViewController = _pageView;

            gameObject->AddComponent<HMUI::Touchable*>();
            BSML::parse_and_construct(Assets::header_content_bsml, transform, this);
        }

        StartRefreshContent();
    }

    void BoardViewController::StartRefreshContent() {
        _communitiesView->RefreshCommunities(COMMUNITIES_URL);
        SwitchDisplayedView(_pageView);
    }

    void BoardViewController::SwitchDisplayedView(HMUI::ViewController* targetView) {
        // we're currently presenting something
        if (_viewControllerToPresent) return;

        _viewControllerToPresent = targetView;
        // we had an active one, so remove it and wait for push to conclude
        if (_activeViewController != nullptr) {
            _mainFlowCoordinator->PopViewControllerFromNavigationController(this, _presentAfterPopAction, false);
            return;
        }

        // we had no active one, so we can just push directly
        _mainFlowCoordinator->PushViewControllerToNavigationController(this, targetView, _finishPushAction, false);
    }

    void BoardViewController::PresentViewControllerAfterPop() {
        if (!_viewControllerToPresent) return;
        _activeViewController = nullptr;
        _mainFlowCoordinator->PushViewControllerToNavigationController(this, _viewControllerToPresent, _finishPushAction, false);
    }

    void BoardViewController::FinishPushingViewController() {
        // set active and clear topresent
        _activeViewController = _viewControllerToPresent;
        _viewControllerToPresent = nullptr;
    }

    void BoardViewController::CommunityWasSelected(std::string_view communityURL) {
        if (_pageView->OpenPageNextPresent(communityURL)) SwitchDisplayedView(_pageView);
    }
}
