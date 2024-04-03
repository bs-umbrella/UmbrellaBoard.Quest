#include "BoardViewController.hpp"
#include "UI/Views/CommunityConfigurationView.hpp"
#include "config.hpp"
#include "assets.hpp"
#include "logging.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"

#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/Time.hpp"
#include "HMUI/Touchable.hpp"

#include <chrono>
#include <future>

DEFINE_TYPE(Umbrella, BoardViewController);

#define FADE_OUT_TIME 0.2
#define FADE_IN_TIME 0.2
constexpr inline float flip(float t) { return 1.0f - t; }
constexpr inline float square(float t) { return t * t; }
constexpr inline float ease_in(float t) { return square(t); }
constexpr inline float ease_out(float t) { return flip(square(flip(t))); }
constexpr inline float ease_in_quart(float t) { return square(square(t)); }
constexpr inline float ease_out_quart(float t) { return flip(square(square(t))) * -1; }
constexpr inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
constexpr inline float eased_t(float t) { return lerp(ease_in(t), ease_out(t), t); }

namespace Umbrella {
    void BoardViewController::ctor() {
        INVOKE_CTOR();
        HMUI::NavigationController::_ctor();

        _presentAfterPopAction = BSML::MakeSystemAction<>(std::bind(&BoardViewController::PresentViewControllerAfterPop, this));
        _finishPushAction = BSML::MakeSystemAction<>(std::bind(&BoardViewController::FinishPushingViewController, this));
        this->_alignment = HMUI::NavigationController::Alignment::Beginning;
    }

    void BoardViewController::Inject(GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, UI::Views::CommunitiesView* communitiesView, UI::Views::PageView* pageView, UI::Views::CommunityConfigurationView* communityConfigurationView) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _communitiesView = communitiesView;
        _pageView = pageView;
        _communityConfigurationView = communityConfigurationView;
    }

    void BoardViewController::Initialize() {
        _communitiesView->CommunityWasSelected += {&BoardViewController::CommunityWasSelected, this};
        _pageView->HistoryWasCleared += {&BoardViewController::DiscoverCommunities, this};
    }

    void BoardViewController::Dispose() {
        _communitiesView->CommunityWasSelected -= {&BoardViewController::CommunityWasSelected, this};
        _pageView->HistoryWasCleared -= {&BoardViewController::DiscoverCommunities, this};
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

        if (addedToHierarchy) {
            DiscoverCommunities();
        }
    }

    void BoardViewController::PostParse() {
        _navButtons->transform->GetChild(0)->eulerAngles = {0, 0, -90};
    }

    void BoardViewController::DiscoverCommunities() {
        _communitiesView->RefreshCommunities(config.communitiesDiscoveryURL);
        SwitchDisplayedView(_pageView);
    }

    void BoardViewController::BackPage() {
        _pageView->Back();
    }

    void BoardViewController::RefreshPage() {
        _pageView->Refresh();
    }

    void BoardViewController::OpenCommunityConfig() {
        SwitchDisplayedView(_communityConfigurationView);
    }

    void BoardViewController::CloseCommunityConfig() {
        SwitchDisplayedView(_pageView);
    }

    custom_types::Helpers::Coroutine FadeInOverSeconds(HMUI::ViewController* viewController, float time) {
        float step = 1.0 / time;
        auto group = viewController->canvasGroup;
        if (!group) co_return;

        for (float t = 0.0f; t < 1.0f; t += UnityEngine::Time::get_deltaTime() * step) {
            auto cur = group->alpha;
            // appear faster at end
            group->alpha = std::max(cur, ease_in_quart(t));
            co_yield nullptr;
        }

        group->alpha = 1.0f;
        co_return;
    }

    custom_types::Helpers::Coroutine FadeOutOverSeconds(HMUI::ViewController* viewController, float time) {
        float step = 1.0 / time;
        auto group = viewController->canvasGroup;
        if (!group) co_return;

        for (float t = 1.0f; t > 0.0f; t -= UnityEngine::Time::get_deltaTime() * step) {
            auto cur = group->alpha;
            // disappear faster at start
            group->alpha = std::min(cur, ease_in_quart(t));
            co_yield nullptr;
        }

        group->alpha = 0.0f;
        co_return;
    }

    void BoardViewController::SwitchDisplayedView(HMUI::ViewController* targetView) {
        // we're currently presenting something
        if (_viewControllerToPresent) return;

        _viewControllerToPresent = targetView;
        // if the presented view is the community config view, disable the nav buttons
        _navButtons->SetActive(_viewControllerToPresent != _communityConfigurationView);

        // if we are going to settings, set open inactive and set close active
        _openSettings->SetActive(_viewControllerToPresent != _communityConfigurationView);
        _closeSettings->SetActive(_viewControllerToPresent == _communityConfigurationView);

        // we had an active one, so remove it and wait for push to conclude
        if (_activeViewController != nullptr) {
            BSML::SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(FadeOutOverSeconds(_activeViewController, FADE_OUT_TIME)));
            _mainFlowCoordinator->PopViewControllerFromNavigationController(this, _presentAfterPopAction, false);
            return;
        }

        // we had no active one, so we can just push directly
        BSML::SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(FadeInOverSeconds(targetView, FADE_IN_TIME)));
        _mainFlowCoordinator->PushViewControllerToNavigationController(this, targetView, _finishPushAction, false);
    }

    void BoardViewController::PresentViewControllerAfterPop() {
        if (!_viewControllerToPresent) return;
        _activeViewController = nullptr;
        BSML::SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(FadeInOverSeconds(_viewControllerToPresent, FADE_IN_TIME)));
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
