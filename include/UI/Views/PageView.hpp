#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "DownloaderUtility.hpp"
#include "../LoadingControl.hpp"

DECLARE_CLASS_CODEGEN(Umbrella::UI::Views, PageView, HMUI::ViewController,
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD_PRIVATE(LoadingControl*, _loadingControl);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _parsedContentParent);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _placeHolderContent);

        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
        DECLARE_INSTANCE_METHOD(void, OpenPage, StringW pageURL);
        DECLARE_INSTANCE_METHOD(void, Back);
        DECLARE_INSTANCE_METHOD(void, Refresh);
        DECLARE_INSTANCE_METHOD(void, ClearHistory);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, Awake);

    public:
        UnorderedEventCallback<> HistoryWasCleared;

        /// @brief sets the url to open for the next present
        /// @return whether we entered a loading state
        bool OpenPageNextPresent(std::string_view pageURL, bool addToHistory = true);

        /// @brief opens a new url page
        /// @param pageURL the url to go to
        /// @param addToHistory whether to add to history stack
        void GotoPage(std::string_view pageURL, bool addToHistory = true);
    private:

        void ShowLoading(bool isLoading, std::string_view loadingText = "");
        void ParseNewContent(std::string_view content);

        std::future<DownloaderUtility::Response<std::string>> _responseFuture;
        DownloaderUtility _downloader;

        std::stack<std::string> _visitedPages;

        std::string _nextPageToOpen;
        bool _nextPageToHistory;

        bool _bsmlReady = false;
)
