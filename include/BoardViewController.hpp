#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "DownloaderUtility.hpp"

DECLARE_CLASS_CODEGEN(Umbrella, BoardViewController, HMUI::ViewController,
        DECLARE_CTOR(ctor);

        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _headerContent);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _headerText);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _loadingContent);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _loadingText);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _errorContent);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _parsedContentParent);

        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool fistActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, Update);
    private:
        /// @brief updates the various content types with the active state based on isLoading, and if loadingText is not empty sets it
        void ShowLoading(bool isLoading, std::string_view loadingText = "");
        void StartRefreshContent();
        void ParseNewContent(std::string_view content);

        DownloaderUtility downloaderUtility;
        std::future<DownloaderUtility::Response<std::string>> futureResponse;
)
