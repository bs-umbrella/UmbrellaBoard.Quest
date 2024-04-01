#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/HorizontalOrVerticalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/EventSystems/IPointerEnterHandler.hpp"
#include "UnityEngine/EventSystems/IPointerExitHandler.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"

namespace Umbrella::UI::Tags {
    class CarouselTag;
}

DECLARE_CLASS_CODEGEN(Umbrella::UI, CarouselBubble, UnityEngine::MonoBehaviour,
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD(HMUI::ImageView*, image);

    public:
        HMUI::ImageView* get_Image();
        __declspec(property(get=get_Image)) HMUI::ImageView* Image;

        bool get_Highlighted() const { return _highlighted; }
        void set_Highlighted(bool highlighted) { _highlighted = highlighted; UpdateHighlight(); }
        __declspec(property(get=get_Highlighted, put=set_Highlighted)) bool Highlighted;

        UnityEngine::Color get_HighlightColor() const { return _highlightColor; }
        void set_HighlightColor(UnityEngine::Color highlightColor) { _highlightColor = highlightColor; UpdateHighlight(); }
        __declspec(property(get=get_HighlightColor, put=set_HighlightColor)) UnityEngine::Color HighlightColor;

        UnityEngine::Color get_DefaultColor() const { return _highlightColor; }
        void set_DefaultColor(UnityEngine::Color highlightColor) { _highlightColor = highlightColor; UpdateHighlight(); }
        __declspec(property(get=get_DefaultColor, put=set_DefaultColor)) UnityEngine::Color DefaultColor;

        void UpdateHighlight();
    private:
        bool _highlighted;
        UnityEngine::Color _highlightColor;
        UnityEngine::Color _defaultColor;
)

DECLARE_CLASS_CODEGEN_INTERFACES(Umbrella::UI, HoverDetection, UnityEngine::MonoBehaviour, std::vector<Il2CppClass*>({classof(UnityEngine::EventSystems::IPointerEnterHandler*), classof(UnityEngine::EventSystems::IPointerExitHandler*), classof(UnityEngine::EventSystems::IEventSystemHandler*)}),
        DECLARE_CTOR(ctor);
        DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerEnter, &UnityEngine::EventSystems::IPointerEnterHandler::OnPointerEnter, UnityEngine::EventSystems::PointerEventData* eventData);
        DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerExit, &UnityEngine::EventSystems::IPointerExitHandler::OnPointerExit, UnityEngine::EventSystems::PointerEventData* eventData);
    public:
        UnorderedEventCallback<> Enter;
        UnorderedEventCallback<> Exit;
)

DECLARE_CLASS_CODEGEN(Umbrella::UI, Carousel, UnityEngine::MonoBehaviour,
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, _nextButton);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, _prevButton);

        DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, _carouselLayoutGroup);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::LayoutElement*, _carouselLayoutElement);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, _tickerLayoutGroup);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::LayoutElement*, _tickerLayoutElement);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::ContentSizeFitter*, _tickerSizeFitter);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, _contentLayoutGroup);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::ContentSizeFitter*, _contentSizeFitter);
        DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, _ticker);
        DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, _bubblePrefab);
        DECLARE_INSTANCE_FIELD(ListW<CarouselBubble*>, _carouselBubbles);
        DECLARE_INSTANCE_FIELD(ListW<UnityEngine::CanvasGroup*>, _carouselCanvasGroups);
        DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _content);
        DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _viewPort);

        DECLARE_INSTANCE_FIELD(HoverDetection*, _viewPortHoverDetection);

        /// @brief start displaying the next child, if on last and on loop will go back to first
        DECLARE_INSTANCE_METHOD(bool, NextItem);
        /// @brief go back to the previous child, if on first and on loop will go to last
        DECLARE_INSTANCE_METHOD(bool, PreviousItem);

        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);
        DECLARE_INSTANCE_METHOD(void, OnEnable);
    public:
        enum class CarouselTimerBehaviour {
            None,
            PingPong,
            Loop,
            LoopForward = Loop,
            LoopBackward
        };

        enum class CarouselDirection {
            Horizontal,
            Vertical
        };

        enum class CarouselLocation {
            Default,
            Bottom,
            Top,
            Left,
            Right
        };

        enum class CarouselAlignment {
            Beginning,
            Center,
            Middle = Center,
            End
        };

        /// @brief active child changed, args are self, new index
        UnorderedEventCallback<Carousel*, int> ActiveChildChanged;

        int get_CurrentlyActiveChildIndex() const { return _currentChildIndex; }
        void set_CurrentlyActiveChildIndex(int currentChildIndex);
        __declspec(property(get=get_CurrentlyActiveChildIndex, put=set_CurrentlyActiveChildIndex)) int CurrentlyActiveChildIndex;

        CarouselDirection get_Direction() const { return _carouselDirection; }
        void set_Direction(CarouselDirection direction);
        __declspec(property(get=get_Direction, put=set_Direction)) CarouselDirection Direction;

        CarouselLocation get_Location() const { return _carouselLocation; }
        void set_Location(CarouselLocation location);
        __declspec(property(get=get_Location, put=set_Location)) CarouselLocation Location;

        CarouselTimerBehaviour get_TimerBehaviour() const { return _carouselTimerBehaviour; }
        void set_TimerBehaviour(CarouselTimerBehaviour TimerBehaviour);
        __declspec(property(get=get_TimerBehaviour, put=set_TimerBehaviour)) CarouselTimerBehaviour TimerBehaviour;

        CarouselAlignment get_Alignment() const { return _carouselAlignment; }
        void set_Alignment(CarouselAlignment alignment);
        __declspec(property(get=get_Alignment, put=set_Alignment)) CarouselAlignment Alignment;

        float get_TimerLength() const { return _timerLength; }
        void set_TimerLength(float timerLength) { _timerLength = timerLength; }
        __declspec(property(get=get_TimerLength, put=set_TimerLength)) float TimerLength;

        bool get_ShowButtons() const { return _showButtons; }
        void set_ShowButtons(bool showButtons);
        __declspec(property(get=get_ShowButtons, put=set_ShowButtons)) bool ShowButtons;

        bool get_PauseOnHover() const { return _pauseOnHover; }
        void set_PauseOnHover(bool pauseOnHover) { _pauseOnHover = pauseOnHover; }
        __declspec(property(get=get_PauseOnHover, put=set_PauseOnHover)) bool PauseOnHover;

        float get_InactiveAlpha() const { return _inactiveAlpha; }
        void set_InactiveAlpha(float inactiveAlpha);
        __declspec(property(get=get_InactiveAlpha, put=set_InactiveAlpha)) float InactiveAlpha;

        /// @brief skips to next using timer behaviour
        void Skip();

        /// @brief start displaying the next child, if on last and on loop will go back to first
        bool Next(bool animated = true);
        /// @brief go back to the previous child, if on first and on loop will go to last
        bool Previous(bool animated = true);

        void SetCurrentlyActiveChildIndex(int index, bool animated = true);

        void Setup();
        void SetupAfterChildren();
    private:
        friend class ::Umbrella::UI::Tags::CarouselTag;
        int _currentChildIndex;
        int _movingDirection;
        bool _isAnimating;
        bool _startRealignNextFrame;

        void UpdateViewport();
        void SetContentSize(float size);
        void SetActiveBubble(int index);

        /// @brief clamps the input index, or advances with timer behaviour taken into account
        int ClampedWithTimerBehaviour(int index);

        /// @brief whether any of the children are hovered
        void SetIsHovered(bool isHovered);

        /// @brief advance to next child using timer mode
        void AdvanceWithTimer();

        /// @brief updates whether the buttons are interactable based on the state of the carousel
        void UpdateButtonsInteractable();

        void SetTickerVertical();
        void SetTickerHorizontal();
        void SetContentVertical();
        void SetContentHorizontal();

        /// @brief rearranges the ticker to be at location and direction
        void MoveTicker(CarouselLocation location, CarouselDirection direction, bool force = false);

        void OnPointerEnterViewport();
        void OnPointerExitViewport();

        void SetAlphaToGroups(int index);

        custom_types::Helpers::Coroutine GotoChild(int childIndex, bool animated = true);

        CarouselDirection _carouselDirection;
        CarouselLocation _carouselLocation;
        CarouselTimerBehaviour _carouselTimerBehaviour;
        CarouselAlignment _carouselAlignment;
        float _timerLength;
        float _timer;
        bool _showButtons;
        bool _pauseOnHover;
        bool _beingHovered;
        float _inactiveAlpha;

        bool get_TimerPassed() const { return _timerLength >= 0 && _timer > _timerLength; }
        __declspec(property(get=get_TimerPassed)) bool TimerPassed;
)

// handle enum as int
#define SIMPLE_ENUM_CLASSOF(e) \
static_assert(sizeof(e) == sizeof(int)); \
template<> \
struct il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<e> { \
    static const Il2CppClass* get() { return classof(int); } \
}

SIMPLE_ENUM_CLASSOF(Umbrella::UI::Carousel::CarouselTimerBehaviour);
SIMPLE_ENUM_CLASSOF(Umbrella::UI::Carousel::CarouselDirection);
SIMPLE_ENUM_CLASSOF(Umbrella::UI::Carousel::CarouselLocation);
SIMPLE_ENUM_CLASSOF(Umbrella::UI::Carousel::CarouselAlignment);

#undef SIMPLE_ENUM_CLASSOF
