#include "BoardViewController.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"
#include "logging.hpp"

#include "HMUI/Touchable.hpp"

#include <chrono>
#include <future>

DEFINE_TYPE(Umbrella, BoardViewController);

#define TIMEOUT 10
#define USER_AGENT "UmbrellaBoard/" VERSION " (+https://github.com/bs-umbrella/UmbrellaBoard)"

#ifndef BOARD_URL
#define BOARD_URL "https://raw.githubusercontent.com/bs-umbrella/UmbrellaBoard/main/default.bsml"
#endif

namespace Umbrella {
    void BoardViewController::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();

        downloaderUtility.TimeOut = TIMEOUT;
        downloaderUtility.UserAgent = USER_AGENT;
    }
    void BoardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        // if first activation, parse the default content so it's visible
        if (firstActivation) {
            gameObject->AddComponent<HMUI::Touchable*>();
            BSML::parse_and_construct(Assets::header_content_bsml, transform, this);
            BSML::parse_and_construct(Assets::loading_content_bsml, transform, this);
            BSML::parse_and_construct(Assets::error_content_bsml, transform, this);
            BSML::parse_and_construct("<bg id='_parsedContentParent'/>", transform, this);
        }

        StartRefreshContent();
    }

    void BoardViewController::Update() {
        if (futureResponse.valid() && futureResponse.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
            auto response = futureResponse.get();
            if (response && response.content.has_value()) {
                DEBUG("Got content: {}", *response.content);
                ParseNewContent(*response.content);
            } else {
                if (response.httpCode < 200 && response.httpCode >= 300) {
                    ShowError(fmt::format("Http response code: {}", response.httpCode));
                } else if (response.curlStatus != 0) {
                    ShowError(fmt::format("CURL status code: {}", response.curlStatus));
                } else if (!response.content.has_value()) {
                    ShowError("Response came back empty!");
                } else {
                    ShowError();
                }
                ERROR("Failed to receive proper content, http: {}, curl: {}, content: {}", response.httpCode, response.curlStatus, response.content.has_value());
            }
        }
    }

    void BoardViewController::StartRefreshContent() {
        ShowLoading(true);
        futureResponse = downloaderUtility.GetString(BOARD_URL);
    }

    void BoardViewController::ShowLoading(bool isLoading, std::string_view loadingText) {
        if (!loadingText.empty()) {
            _loadingText->text = loadingText;
        }
        // no matter what, disable error content here
        _errorContent->SetActive(false);
        _loadingContent->SetActive(isLoading);
        _parsedContentParent->SetActive(!isLoading);
    }

    void BoardViewController::ShowError(std::string_view errorText) {
        if (errorText.empty()) errorText = "An error ocurred while downloading content...";
        _errorText->text = errorText;

        // error content active, loading and parsed inactive
        _errorContent->SetActive(true);
        _loadingContent->SetActive(false);
        _parsedContentParent->SetActive(false);
    }

    void BoardViewController::ParseNewContent(std::string_view content) {
        ShowLoading(false);

        // destroy previous children
        auto t = _parsedContentParent->transform;
        auto childCount = t->childCount;
        for (auto i = childCount - 1; i >= 0; i--) {
            auto c = t->GetChild(i);
            UnityEngine::Object::Destroy(c->gameObject);
        }

        BSML::parse_and_construct(content, t, this);
    }
}
