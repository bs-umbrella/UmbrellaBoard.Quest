#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/ImageView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Sprite.hpp"

#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "../DownloaderUtility.hpp"
#include "../LoadingControl.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella::Views, CommunitiesView, HMUI::ViewController, classof(HMUI::TableView::IDataSource*),
        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

        DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
        DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
        DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);

        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::TableView*, _tableView);
        DECLARE_INSTANCE_FIELD_PRIVATE(LoadingControl*, _loadingControl);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _parsedContentParent);

        DECLARE_INSTANCE_FIELD_PRIVATE(BSML::CustomListTableData*, _bsmlCommunityList);

        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, PostParse);
        DECLARE_INSTANCE_METHOD(void, HandleCommunitySelected, HMUI::TableView* tableView, int selectedCell);
        DECLARE_CTOR(ctor);

    public:
        /// @brief downloads a json at the location given and parses it for community information
        void RefreshCommunities(std::string_view communitiesURL);

        UnorderedEventCallback<std::string_view> CommunityWasSelected;

        HMUI::TableView::IDataSource* i_IDataSource() noexcept { return reinterpret_cast<HMUI::TableView::IDataSource*>(this); }
    private:
        /// @brief handle when the communities have been received
        void HandleCommunitiesReceived(rapidjson::Document const& doc);

        bool _bsmlReady;
        std::future<DownloaderUtility::Response<rapidjson::Document>> _responseFuture;
        DownloaderUtility _downloader;
)

DECLARE_CLASS_CODEGEN(Umbrella::Views, CommunityCell, HMUI::TableCell,
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _communityName);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _communityBackground);

        DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
        DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);

        DECLARE_CTOR(ctor);
    public:
        static CommunityCell* GetCell();

        /// @brief sets data for cell, background is a sprite to set directly
        /// @param communityName the name of the community to display
        /// @param communityURL the url of the index for that community
        /// @param background the background for that community in the list, should be ~8w to 1h
        CommunityCell* SetData(std::string_view communityName, std::string_view communityURL, UnityEngine::Sprite* background = nullptr);

        /// @brief sets data for cell, background is a url to download from instead
        /// @param communityName the name of the community to display
        /// @param communityURL the url of the index for that community
        /// @param background the background URL for that community in the list, should be ~8w to 1h
        CommunityCell* SetData(std::string_view communityName, std::string_view communityURL, std::string_view backgroundURL);

        /// @brief getter for community URL
        std::string_view get_CommunityURL() const { return _communityURL; }
        __declspec(property(get=get_Community)) std::string_view CommunityURL;
    private:
        void UpdateHighlight();
        std::string _communityURL;
)
