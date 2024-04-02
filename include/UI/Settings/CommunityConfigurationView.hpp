#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DECLARE_CLASS_CODEGEN(Umbrella::UI::Settings, CommunityConfigurationCell, HMUI::TableCell,
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _background);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _mask);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _communityName);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::LayoutElement*, _layout);

        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _moveButtons);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _moveUpButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _moveDownButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _disableButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _enableButton);

        DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
        DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);

        DECLARE_INSTANCE_METHOD(void, MoveLeft);
        DECLARE_INSTANCE_METHOD(void, MoveRight);
        DECLARE_INSTANCE_METHOD(void, MoveUp);
        DECLARE_INSTANCE_METHOD(void, MoveDown);
    public:
        enum MoveDirection {
            Left,
            Right,
            Up,
            Down,
        };

        UnorderedEventCallback<CommunityConfigurationCell*, MoveDirection> CommunityMoved;

        void UpdateHighlight();
        static CommunityConfigurationCell* MakeCell();

        CommunityConfigurationCell* SetData(std::string_view communityName, UnityEngine::Sprite* background = nullptr);
        CommunityConfigurationCell* SetData(std::string_view communityName, std::string_view backgroundUrl);

        CommunityConfigurationCell* SetIsEnabledCell(bool isEnabledCell);

        StringW get_CommunityName() { return _communityName->text; }
        __declspec(property(get=get_CommunityName)) StringW CommunityName;

    private:
        bool _enabledCell;
)

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella::UI::Settings, CommunityConfigurationCellListDataSource, UnityEngine::MonoBehaviour, classof(HMUI::TableView::IDataSource*),
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);

        DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
        DECLARE_OVERRIDE_METHOD_MATCH(float_t, CellSize, &HMUI::TableView::IDataSource::CellSize);
        DECLARE_OVERRIDE_METHOD_MATCH(int32_t, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    public:
        bool _isEnabledCommunitiesList;
        UnorderedEventCallback<CommunityConfigurationCell*, CommunityConfigurationCell::MoveDirection> CommunityMoved;

        HMUI::TableView::IDataSource* i_DataSource() { return reinterpret_cast<HMUI::TableView::IDataSource*>(this); }
    private:
        void CommunityDidMove(CommunityConfigurationCell* cell, CommunityConfigurationCell::MoveDirection direction);
);

DECLARE_CLASS_CODEGEN(Umbrella::UI::Settings, CommunityConfigurationView, HMUI::ViewController,
        DECLARE_INSTANCE_FIELD_PRIVATE(BSML::CustomListTableData*, _enabledCommunitiesBSMLList);
        DECLARE_INSTANCE_FIELD_PRIVATE(BSML::CustomListTableData*, _disabledCommunitiesBSMLList);

        DECLARE_INSTANCE_FIELD_PRIVATE(CommunityConfigurationCellListDataSource*, _disabledCommunitiesList);
        DECLARE_INSTANCE_FIELD_PRIVATE(CommunityConfigurationCellListDataSource*, _enabledCommunitiesList);
        DECLARE_INSTANCE_METHOD(void, PostParse);

        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    private:
        void RefreshLists();

        void CommunityDidMove(CommunityConfigurationCell* cell, CommunityConfigurationCell::MoveDirection direction);

        void EnableCommunity(CommunityConfigurationCell* cell);
        void DisableCommunity(CommunityConfigurationCell* cell);
        void MoveCommunity(CommunityConfigurationCell* cell, bool up);
)
