#include "UI/Views/CommunitiesView.hpp"
#include "LoadingControl.hpp"
#include "logging.hpp"
#include "assets.hpp"
#include "config.hpp"
#include "_config.h"

#include "System/Collections/Generic/HashSet_1.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

DEFINE_TYPE(Umbrella::UI::Views, CommunitiesView);
DEFINE_TYPE(Umbrella::UI::Views, CommunityCell);

namespace Umbrella::UI::Views {
    void CommunitiesView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();

        _downloader.UserAgent = USER_AGENT;
        _downloader.TimeOut = TIMEOUT;
    }

    void CommunitiesView::Awake() {
        gameObject->AddComponent<UnityEngine::CanvasGroup*>();
    }

    void CommunitiesView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            _loadingControl = gameObject->AddComponent<LoadingControl*>();
            BSML::parse_and_construct(Assets::communities_list_bsml, transform, this);

            auto r = rectTransform;
            r->sizeDelta = {24, 60};
            r->anchorMin = {0.5, 0};
            r->anchorMax = {0.5, 1};

            _bsmlReady = true;
        }
    }

    void CommunitiesView::PostParse() {
        _tableView = _bsmlCommunityList->tableView;
        UnityEngine::Object::Destroy(_bsmlCommunityList);
        _bsmlCommunityList = nullptr;
        _tableView->SetDataSource(this->i_IDataSource(), true);
    }

    void CommunitiesView::RefreshCommunities(std::string_view communitiesURL) {
        _responseFuture = _downloader.GetJson(communitiesURL);
    }

    HMUI::TableCell* CommunitiesView::CellForIdx(HMUI::TableView* tableView, int32_t idx) {
        auto cell = tableView->DequeueReusableCellForIdentifier("UmbrellaCommunities").try_cast<CommunityCell>().value_or(nullptr);

        if (!cell) {
            cell = CommunityCell::GetCell();
            cell->set_reuseIdentifier("UmbrellaCommunities");
        }

        auto& data = config.enabledCommunities[idx];
        cell->SetData(data.communityName, data.communityPageURL, data.communityBackgroundURL);

        return cell;
    }

    float CommunitiesView::CellSize() {
        return 12;
    }

    int CommunitiesView::NumberOfCells() {
        return config.enabledCommunities.size();
    }

    void CommunitiesView::Update() {
        if (!_bsmlReady) return;

        if (_responseFuture.valid()) {
            if (_responseFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready) {
                auto response = _responseFuture.get();
                if (response && response.content.has_value()) {
                    _parsedContentParent->SetActive(true);
                    _loadingControl->ShowLoading(false);

                    HandleCommunitiesReceived(response.content.value());
                } else {
                    if (response.httpCode < 200 && response.httpCode >= 300) {
                        _loadingControl->ShowError(fmt::format("Failed to get content, http response: {}", response.httpCode));
                    } else if (response.curlStatus != 0) {
                        _loadingControl->ShowError(fmt::format("Failed to get content, curl status: {}", response.curlStatus));
                    } else if (!response.content.has_value()) {
                        _loadingControl->ShowError(fmt::format("Failed to get or parse content json"));
                    } else {
                        _loadingControl->ShowError();
                    }
                }
            } else {
                _parsedContentParent->SetActive(false);
                _loadingControl->ShowLoading(true, "Loading Communities...");
            }
        }
    }

    void CommunitiesView::HandleCommunitiesReceived(rapidjson::Document const& doc) {
        auto memberEnd = doc.MemberEnd();
        auto questDiscoveryItr = doc.FindMember("questDiscovery");
        if (questDiscoveryItr == memberEnd) {
            WARNING("No additional quest communities were discovered through discovery URL");
            return;
        }


        for (auto const& community : questDiscoveryItr->value.GetArray()) {
            Community parsedCommunity;
            if (!parsedCommunity.Deserialize(community)) {
                WARNING("Failed to parse a community, skipping...");
                continue;
            }

            // is this a disabled community? if so, update and skip
            auto disabledItr = std::find_if(config.disabledCommunities.begin(), config.disabledCommunities.end(), [&communityName = parsedCommunity.communityName](auto& c){ return c.communityName == communityName; });
            if (disabledItr != config.disabledCommunities.end()) {
                disabledItr->communityBackgroundURL = parsedCommunity.communityBackgroundURL;
                disabledItr->communityPageURL = parsedCommunity.communityPageURL;
                continue;
            }

            // is this an already enabled community? if so, update and skip
            auto enabledItr = std::find_if(config.enabledCommunities.begin(), config.enabledCommunities.end(), [&communityName = parsedCommunity.communityName](auto& c){ return c.communityName == communityName; });
            if (enabledItr != config.enabledCommunities.end()) {
                enabledItr->communityBackgroundURL = parsedCommunity.communityBackgroundURL;
                enabledItr->communityPageURL = parsedCommunity.communityPageURL;
                continue;
            }

            // this is a new community, add to the back of the enabled communities!
            config.enabledCommunities.emplace_back(std::move(parsedCommunity));
        }

        SaveConfig();
        _tableView->ReloadData();
    }

    void CommunitiesView::HandleCommunitySelected(HMUI::TableView* tableView, int32_t selectedCell) {
        CommunityWasSelected.invoke(config.enabledCommunities[selectedCell].communityPageURL);
        // clear selection but not highlight
        for (auto cell : ListW<UnityW<HMUI::TableCell>>(_tableView->_visibleCells)) {
            cell->SetSelected(false, HMUI::SelectableCell::TransitionType::Instant, _tableView, false);
        }
        _tableView->_selectedCellIdxs->Clear();
        _tableView->RefreshCells(true, false);
    }

    void CommunityCell::ctor() {
        INVOKE_CTOR();
        HMUI::TableCell::_ctor();
    }

    CommunityCell* CommunityCell::GetCell() {
        auto gameObject = UnityEngine::GameObject::New_ctor("CommunityCell");
        auto cell = gameObject->AddComponent<CommunityCell*>();
        BSML::parse_and_construct("<stack pref-width='20' pref-height='10' vertical-fit='PreferredSize' horizontal-fit='PreferredSize'><img id='_communityBackground'/><text id='_communityName' font-size='3' font-align='Center'/></stack>", cell->transform, cell);
        return cell;
    }

    void CommunityCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        UpdateHighlight();
    }

    void CommunityCell::SelectionDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        UpdateHighlight();
    }

    void CommunityCell::UpdateHighlight() {
        _communityBackground->set_color(highlighted || selected ? UnityEngine::Color{1, 1, 1, 1} : UnityEngine::Color{0.6, 0.6, 0.6, 1});
    }

    CommunityCell* CommunityCell::SetData(std::string_view communityName, std::string_view communityURL, UnityEngine::Sprite* background) {
        // TODO: default background if sprite not set
        // if (!background) background =

        _communityName->text = communityName;
        _communityURL = communityURL;
        _communityBackground->set_sprite(background);
        _communityBackground->set_color({0.8, 0.8, 0.8, 1});

        return this;
    }

    CommunityCell* CommunityCell::SetData(std::string_view communityName, std::string_view communityURL, std::string_view backgroundURL) {
        _communityName->text = communityName;
        _communityURL = communityURL;
        BSML::Utilities::SetImage(_communityBackground, backgroundURL);
        _communityBackground->set_color({0.8, 0.8, 0.8, 1});

        return this;
    }
}
