#include "Views/CommunityView.hpp"
#include "LoadingControl.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "_config.h"

#include <chrono>
#include <future>

DEFINE_TYPE(Umbrella::Views, CommunityView);

namespace Umbrella::Views {
    void CommunityView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();

        _downloader.UserAgent = USER_AGENT;
        _downloader.TimeOut = TIMEOUT;
    }

    void CommunityView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            _loadingControl = gameObject->AddComponent<LoadingControl*>();
            _loadingControl->ShowLoading(true);
            BSML::parse_and_construct("<bg id='_parsedContentParent'/>", transform, this);
        }
    }

    void CommunityView::Update() {
        if (_responseFuture.valid() && _responseFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
            auto response = _responseFuture.get();
            if (response && response.content.has_value()) {
                _loadingControl->ShowLoading(false);
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
        }
    }

    void CommunityView::ParseNewContent(std::string_view content) {
        auto t = _parsedContentParent->transform;
        int childCount = t->childCount;

        for (int i = childCount - 1; i >= 0; i--) {
            auto c = t->GetChild(i);
            UnityEngine::Object::Destroy(c->gameObject);
        }

        BSML::parse_and_construct(content, t, this);
    }

    void CommunityView::LoadPage(std::string_view pageURL) {
        _loadingControl->ShowLoading(true, "Downloading page...");
        _responseFuture = _downloader.GetString(pageURL);
    }
}
