#include "UI/Views/PageView.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"
#include "logging.hpp"
#include "_config.h"

DEFINE_TYPE(Umbrella::UI::Views, PageView);

namespace Umbrella::UI::Views {
    void PageView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();

        _downloader.UserAgent = USER_AGENT;
        _downloader.TimeOut = TIMEOUT;
        _bsmlReady = false;
    }

    void PageView::Awake() {
        gameObject->AddComponent<UnityEngine::CanvasGroup*>();
    }

    void PageView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            _loadingControl = gameObject->AddComponent<LoadingControl*>();
            BSML::parse_and_construct("<bg id='_parsedContentParent' size-delta-x='100' size-delta-y='0' anchor-min-x='0.5' anchor-max-x='0.5' anchor-min-y='0' anchor-max-y='1'/>", transform, this);
            BSML::parse_and_construct(Assets::placeholder_view_bsml, transform, this);

            auto r = rectTransform;
            r->sizeDelta = {100, 0};
            r->anchorMin = {0.5, 0};
            r->anchorMax = {0.5, 1};
            _bsmlReady = true;
        }

        if (!_nextPageToOpen.empty()) {
            GotoPage(_nextPageToOpen, _nextPageToHistory);
            _nextPageToOpen.clear();
        } else if (addedToHierarchy) {
            _visitedPages.emplace("placeholder");
            _placeHolderContent->SetActive(true);
            _parsedContentParent->SetActive(false);
        }
    }

    void PageView::Update() {
        if (!_bsmlReady) return;

        if (_responseFuture.valid()) {
            if (_responseFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
                auto response = _responseFuture.get();
                if (response && response.content.has_value()) {
                    ShowLoading(false);
                    ParseNewContent(response.content.value());
                } else {
                    _parsedContentParent->SetActive(false);

                    if (response.httpCode < 200 || response.httpCode >= 300) {
                        _loadingControl->ShowError(fmt::format("Http response code: {}", response.httpCode));
                    } else if (response.curlStatus != 0) {
                        _loadingControl->ShowError(fmt::format("Curl status code: {}", response.curlStatus));
                    } else if (!response.content.has_value()) {
                        _loadingControl->ShowError(fmt::format("No content received"));
                    } else {
                        _loadingControl->ShowError();
                    }
                }
            } else {
                ShowLoading(true, "Loading page...");
            }
        }
    }

    void PageView::ShowLoading(bool isLoading, std::string_view loadingText) {
        if (isLoading && loadingText.empty()) loadingText = "Loading page...";
        _loadingControl->ShowLoading(isLoading, loadingText);
        _parsedContentParent->SetActive(!isLoading);
        _placeHolderContent->SetActive(false);
    }

    void PageView::ParseNewContent(std::string_view content) {
        auto t = _parsedContentParent->transform;
        int childCount = t->childCount;

        for (int i = childCount - 1; i >= 0; i--) {
            auto c = t->GetChild(i);
            UnityEngine::Object::Destroy(c->gameObject);
        }

        BSML::parse_and_construct(content, t, this);
    }

    bool PageView::OpenPageNextPresent(std::string_view pageURL, bool addToHistory) {
        if (!_visitedPages.empty() && _visitedPages.top() == pageURL) return false;
        if (pageURL.empty()) return false;

        ShowLoading(true, "Loading page...");
        _nextPageToOpen = pageURL;
        _nextPageToHistory = addToHistory;

        return true;
    }

    void PageView::OpenPage(StringW pageURL) {
        GotoPage(std::string(pageURL));
    }

    void PageView::GotoPage(std::string_view pageURL, bool addToHistory) {
        if (!_visitedPages.empty() && _visitedPages.top() == pageURL) return;
        if (pageURL.empty()) return;

        _responseFuture = _downloader.GetString(pageURL);
        ShowLoading(true, "Loading page...");
        if (addToHistory) _visitedPages.emplace(pageURL);
    }

    void PageView::Back() {
        if (_visitedPages.empty()) return;

        // remove one
        _visitedPages.pop();
        // go to new current top
        GotoPage(_visitedPages.top(), false);
    }

    void PageView::Refresh() {
        if (_visitedPages.empty()) return;

        // go to current top
        GotoPage(_visitedPages.top(), false);
    }

    void PageView::ClearHistory() {
        // default ctored swap is a clear
        std::stack<std::string>().swap(_visitedPages);
        HistoryWasCleared.invoke();
    }
}
