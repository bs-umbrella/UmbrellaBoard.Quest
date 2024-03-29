#include "BoardViewController.hpp"
#include "LoadingControl.hpp"
#include "Views/PlaceHolderView.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"
#include "logging.hpp"

#include "HMUI/Touchable.hpp"

#include <chrono>
#include <future>

DEFINE_TYPE(Umbrella, BoardViewController);

#ifndef BOARD_URL
#define BOARD_URL "https://raw.githubusercontent.com/bs-umbrella/UmbrellaBoard/main/default.bsml"
#endif

#ifndef COMMUNITIES_URL
#define COMMUNITIES_URL "file:///sdcard/ModData/com.beatgames.beatsaber/Mods/UmbrellaBoard/communities.json"
#endif

namespace Umbrella {
    void BoardViewController::ctor() {
        INVOKE_CTOR();
        HMUI::NavigationController::_ctor();
    }

    void BoardViewController::Inject(GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, Views::CommunitiesView* communitiesView, Views::CommunityView* communityView, Views::PageView* pageView, Views::PlaceHolderView* placeHolderView) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _communitiesView = communitiesView;
        _communityView = communityView;
        _pageView = pageView;
        _placeHolderView = placeHolderView;
    }

    void BoardViewController::Initialize() {
        _communitiesView->CommunityWasSelected += {&BoardViewController::CommunityWasSelected, this};
        _communityView->PageWasOpened += {&BoardViewController::PageWasOpened, this};
        _pageView->PageWasOpened += {&BoardViewController::PageWasOpened, this};

    }

    void BoardViewController::Dispose() {
        _communitiesView->CommunityWasSelected -= {&BoardViewController::CommunityWasSelected, this};
        _communityView->PageWasOpened -= {&BoardViewController::PageWasOpened, this};
        _pageView->PageWasOpened -= {&BoardViewController::PageWasOpened, this};
    }

    void BoardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        INFO("BoardViewController DidActivate");

        // if first activation, parse the default content so it's visible
        if (firstActivation) {
            _mainFlowCoordinator->SetViewControllerToNavigationController(this, _communitiesView);
            SwitchDisplayedView(_placeHolderView);

            gameObject->AddComponent<HMUI::Touchable*>();
            BSML::parse_and_construct(Assets::header_content_bsml, transform, this);
        }

        // StartRefreshContent();
    }

    void BoardViewController::StartRefreshContent() {
        _communitiesView->RefreshCommunities(COMMUNITIES_URL);
    }

    void BoardViewController::SwitchDisplayedView(HMUI::ViewController* targetView) {
        if (_activeViewController == targetView) return;
        if (_activeViewController != nullptr) {
            _mainFlowCoordinator->PopViewControllerFromNavigationController(this, nullptr, false);
            _activeViewController = nullptr;
        }

        _mainFlowCoordinator->PushViewControllerToNavigationController(this, targetView, nullptr, false);
        _activeViewController = targetView;
    }

    void BoardViewController::CommunityWasSelected(std::string_view communityURL) {
        SwitchDisplayedView(_communityView);
        _communityView->LoadPage(communityURL);
    }

    void BoardViewController::PageWasOpened(std::string_view pageURL) {
        SwitchDisplayedView(_pageView);
        _pageView->LoadPage(pageURL);
    }
}
