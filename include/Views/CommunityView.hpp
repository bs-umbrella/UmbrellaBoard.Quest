#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

#include "DownloaderUtility.hpp"
#include "LoadingControl.hpp"

DECLARE_CLASS_CODEGEN(Umbrella::Views, CommunityView, HMUI::ViewController,
        DECLARE_CTOR(ctor);

        DECLARE_INSTANCE_FIELD_PRIVATE(LoadingControl*, _loadingControl);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _parsedContentParent);

        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    public:
        UnorderedEventCallback<std::string_view> PageWasOpened;

        void LoadPage(std::string_view pageURL);
    private:
        void ParseNewContent(std::string_view content);

        DownloaderUtility _downloader;
        std::future<DownloaderUtility::Response<std::string>> _responseFuture;
)
