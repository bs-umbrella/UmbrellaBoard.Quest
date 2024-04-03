#include "UI/Views/CommunityConfigurationView.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "assets.hpp"
#include "config.hpp"

#include "HMUI/Touchable.hpp"
#include "UnityEngine/UI/Mask.hpp"
#include <utility>

DEFINE_TYPE(Umbrella::UI::Views, CommunityConfigurationView);
DEFINE_TYPE(Umbrella::UI::Views, CommunityConfigurationCell);
DEFINE_TYPE(Umbrella::UI::Views, CommunityConfigurationCellListDataSource);

namespace Umbrella::UI::Views {
    void CommunityConfigurationView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();
    }

    void CommunityConfigurationView::Inject(CommunitiesView* communitiesView) {
        _communitiesView = communitiesView;
    }

    void CommunityConfigurationView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            gameObject->AddComponent<UnityEngine::CanvasGroup*>();
            BSML::parse_and_construct(Assets::UI::community_configuration_view_bsml, transform, this);
            auto r = rectTransform;

            r->sizeDelta = {100, 0};
            r->anchorMin = {0.5, 0};
            r->anchorMax = {0.5, 1};
        }

        RefreshLists();
    }

    void CommunityConfigurationView::PostParse() {
        {
            auto tableView = _enabledCommunitiesBSMLList->tableView;
            auto go = _enabledCommunitiesBSMLList->gameObject;
            UnityEngine::Object::Destroy(_enabledCommunitiesBSMLList);
            _enabledCommunitiesBSMLList = nullptr;
            _enabledCommunitiesList = go->AddComponent<CommunityConfigurationCellListDataSource*>();
            _enabledCommunitiesList->_isEnabledCommunitiesList = true;
            _enabledCommunitiesList->tableView = tableView;
            tableView->SetDataSource(_enabledCommunitiesList->i_DataSource(), false);
            _enabledCommunitiesList->CommunityMoved += {&CommunityConfigurationView::CommunityDidMove, this};
        }

        {
            auto tableView = _disabledCommunitiesBSMLList->tableView;
            auto go = _disabledCommunitiesBSMLList->gameObject;
            UnityEngine::Object::Destroy(_disabledCommunitiesBSMLList);
            _disabledCommunitiesBSMLList = nullptr;
            _disabledCommunitiesList = go->AddComponent<CommunityConfigurationCellListDataSource*>();
            _disabledCommunitiesList->tableView = tableView;
            tableView->SetDataSource(_disabledCommunitiesList->i_DataSource(), false);
            _disabledCommunitiesList->CommunityMoved += {&CommunityConfigurationView::CommunityDidMove, this};
        }
    }

    void CommunityConfigurationView::RefreshLists() {
        _enabledCommunitiesList->tableView->ReloadDataKeepingPosition();
        _disabledCommunitiesList->tableView->ReloadDataKeepingPosition();
    }

    void CommunityConfigurationView::CommunityDidMove(CommunityConfigurationCell* cell, CommunityConfigurationCell::MoveDirection direction) {
        switch (direction) {
            using enum CommunityConfigurationCell::MoveDirection;
            case Left: {
                EnableCommunity(cell);
            } break;
            case Right: {
                DisableCommunity(cell);
            } break;
            case Up:  {
                MoveCommunity(cell, true);
            } break;
            case Down: {
                MoveCommunity(cell, false);
            } break;
        }

        SaveConfig();
        RefreshLists();

        _communitiesView->RefreshCommunitiesNoRequest();
    }

    void CommunityConfigurationView::EnableCommunity(CommunityConfigurationCell* cell) {
        std::string communityName(cell->CommunityName);
        auto itr = std::find_if(config.disabledCommunities.begin(), config.disabledCommunities.end(), [&communityName](auto& x){ return x.communityName == communityName; });
        if (itr == config.disabledCommunities.end()) return; // cell wasnt disabled

        // add to back
        config.enabledCommunities.emplace_back(*itr);
        // erase from disabled
        config.disabledCommunities.erase(itr);
    }

    void CommunityConfigurationView::DisableCommunity(CommunityConfigurationCell* cell) {
        std::string communityName(cell->CommunityName);
        auto itr = std::find_if(config.enabledCommunities.begin(), config.enabledCommunities.end(), [&communityName](auto& x){ return x.communityName == communityName; });
        if (itr == config.enabledCommunities.end()) return; // cell wasnt enabled

        // add to back
        config.disabledCommunities.emplace_back(*itr);
        // erase from enabled
        config.enabledCommunities.erase(itr);
    }

    void CommunityConfigurationView::MoveCommunity(CommunityConfigurationCell* cell, bool up) {
        std::string communityName(cell->CommunityName);
        auto itr = std::find_if(config.enabledCommunities.begin(), config.enabledCommunities.end(), [&communityName](auto& x){ return x.communityName == communityName; });
        if (itr == config.enabledCommunities.end()) return;
        auto idx = std::distance(config.enabledCommunities.begin(), itr);

        if (up) {
            if (idx <= 0) return; // it was already the first
            // swap with idx - 1 (move towards start of array)
            std::swap(config.enabledCommunities[idx - 1], config.enabledCommunities[idx]);
        } else { // down
            if (idx >= config.enabledCommunities.size() - 1) return; // it was already the last
            // swap with idx + 1 (move towards end of array)
            std::swap(config.enabledCommunities[idx + 1], config.enabledCommunities[idx]);
        }
    }

    void CommunityConfigurationCellListDataSource::ctor() {
        INVOKE_CTOR();
    }

    HMUI::TableCell* CommunityConfigurationCellListDataSource::CellForIdx(HMUI::TableView* tableView, int idx) {
        auto cell = tableView->DequeueReusableCellForIdentifier("CommunityConfigurationCell").try_cast<CommunityConfigurationCell>().value_or(nullptr);
        if (!cell) {
            cell = CommunityConfigurationCell::MakeCell();
            cell->reuseIdentifier = "CommunityConfigurationCell";
            cell->CommunityMoved += {&CommunityConfigurationCellListDataSource::CommunityDidMove, this};
            cell->interactable = false;
        }

        auto& communities = _isEnabledCommunitiesList ? config.enabledCommunities : config.disabledCommunities;
        auto& data = communities[idx];

        return cell->SetData(data.communityName, data.communityBackgroundURL)->SetIsEnabledCell(_isEnabledCommunitiesList);
    }

    float CommunityConfigurationCellListDataSource::CellSize() {
        return 15.0f;
    }

    int CommunityConfigurationCellListDataSource::NumberOfCells() {
        auto& communities = _isEnabledCommunitiesList ? config.enabledCommunities : config.disabledCommunities;
        return communities.size();
    }

    void CommunityConfigurationCellListDataSource::CommunityDidMove(CommunityConfigurationCell* cell, CommunityConfigurationCell::MoveDirection direction) {
        CommunityMoved.invoke(cell, direction);
    }

    void CommunityConfigurationCell::ctor() {
        INVOKE_CTOR();
        HMUI::TableCell::_ctor();
    }

    void CommunityConfigurationCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        UpdateHighlight();
    }

    void CommunityConfigurationCell::SelectionDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        UpdateHighlight();
    }

    void CommunityConfigurationCell::UpdateHighlight() {
        _moveUpButton->gameObject->SetActive(_enabledCell && (highlighted || selected));
        _moveDownButton->gameObject->SetActive(_enabledCell && (highlighted || selected));

        _background->set_color(highlighted || selected ? UnityEngine::Color{1, 1, 1, 1} : UnityEngine::Color{0.6, 0.6, 0.6, 1});
        _communityName->enabled = !(highlighted || selected);
    }

    CommunityConfigurationCell* CommunityConfigurationCell::MakeCell() {
        auto go = UnityEngine::GameObject::New_ctor("CommunityConfigurationCell");
        go->AddComponent<HMUI::Touchable*>();

        auto cell = go->AddComponent<CommunityConfigurationCell*>();
        BSML::parse_and_construct(Assets::UI::community_configuration_cell_bsml, cell->transform, cell);
        auto maskImg = cell->_mask;
        maskImg->rectTransform->sizeDelta = {30, 15};
        auto mask = maskImg->gameObject->AddComponent<UnityEngine::UI::Mask*>();
        mask->showMaskGraphic = false;
        maskImg->color = {1, 1, 1, 1};
        maskImg->type = UnityEngine::UI::Image::Type::Sliced;

        cell->_background->raycastTarget = false;
        return cell;
    }

    CommunityConfigurationCell* CommunityConfigurationCell::SetData(std::string_view communityName, UnityEngine::Sprite* background) {
        _communityName->text = communityName;
        _background->sprite = background;
        UpdateHighlight();

        return this;
    }

    CommunityConfigurationCell* CommunityConfigurationCell::SetData(std::string_view communityName, std::string_view backgroundUrl) {
        _communityName->text = communityName;
        BSML::Utilities::SetImage(_background, backgroundUrl);
        UpdateHighlight();

        return this;
    }

    CommunityConfigurationCell* CommunityConfigurationCell::SetIsEnabledCell(bool isEnabledCell) {
        _enabledCell = isEnabledCell;
        _disableButton->gameObject->SetActive(_enabledCell);
        _enableButton->gameObject->SetActive(!_enabledCell);
        _moveButtons->SetActive(isEnabledCell);
        _layout->preferredWidth = _enabledCell ? 45 : 35;
        UpdateHighlight();

        return this;
    }

    void CommunityConfigurationCell::MoveLeft() { CommunityMoved.invoke(this, Left); }
    void CommunityConfigurationCell::MoveRight() { CommunityMoved.invoke(this, Right); }
    void CommunityConfigurationCell::MoveUp() { CommunityMoved.invoke(this, Up); }
    void CommunityConfigurationCell::MoveDown() { CommunityMoved.invoke(this, Down); }
}
