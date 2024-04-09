#include "UI/Carousel.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Rect.hpp"
#include "logging.hpp"
#include <map>

DEFINE_TYPE(Umbrella::UI, Carousel);
DEFINE_TYPE(Umbrella::UI, CarouselBubble);
DEFINE_TYPE(Umbrella::UI, HoverDetection);

constexpr inline float flip(float t) { return 1.0f - t; }
constexpr inline float square(float t) { return t * t; }
constexpr inline float ease_in(float t) { return square(t); }
constexpr inline float ease_out(float t) { return flip(square(flip(t))); }
constexpr inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
constexpr inline float eased_t(float t) { return lerp(ease_in(t), ease_out(t), t); }
constexpr inline UnityEngine::Vector2 lerp(UnityEngine::Vector2 a, UnityEngine::Vector2 b, float t) { return { lerp (a.x, b.x, t), lerp(a.y, b.y, t) }; }

namespace Umbrella::UI {
    void Carousel::ctor() {
        INVOKE_CTOR();
        _movingDirection = 1;
        _carouselBubbles = ListW<CarouselBubble*>::New();
        _carouselCanvasGroups = ListW<UnityEngine::CanvasGroup*>::New();
        _timerLength = 5.0f;
        _inactiveAlpha = 0.2f;
        _carouselAlignment = CarouselAlignment::Center;
    }

    void Carousel::Update() {
        if (!_isAnimating) {
            if (!_pauseOnHover || !_beingHovered) { // if not pause on hover, or if not being hovered, advance timer
                _timer += UnityEngine::Time::get_deltaTime();
            }

            if (TimerPassed) { // if the timer passed, advance selection
                AdvanceWithTimer();
            }

            if (_startRealignNextFrame) {
                _startRealignNextFrame = false;
                StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(_currentChildIndex, true)));
            }
        }
    }

    void Carousel::OnEnable() {
        _startRealignNextFrame = true;
    }

    void Carousel::OnDestroy() {
        if (_viewPortHoverDetection) {
            _viewPortHoverDetection->Enter -= {&Carousel::OnPointerEnterViewport, this};
            _viewPortHoverDetection->Exit -= {&Carousel::OnPointerExitViewport, this};
        }
    }

    bool Carousel::NextItem() { return Next(true); }
    bool Carousel::Next(bool animated) {
        if (_isAnimating) return false;
        int nextChild = ClampedWithTimerBehaviour(_currentChildIndex + 1);
        if (nextChild == _currentChildIndex) return false;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(nextChild, animated)));
        return true;
    }

    bool Carousel::PreviousItem() { return Previous(true); }
    bool Carousel::Previous(bool animated) {
        if (_isAnimating) return false;
        int nextChild = ClampedWithTimerBehaviour(_currentChildIndex - 1);
        if (nextChild == _currentChildIndex) return false;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(nextChild, animated)));
        return true;
    }

    void Carousel::AdvanceWithTimer() {
        int nextChild;
        switch (_carouselTimerBehaviour) {
            using enum CarouselTimerBehaviour;
            case None: return; // don't do anything
            case LoopForward: {
                nextChild = ClampedWithTimerBehaviour(_currentChildIndex + 1);
            } break;
            case LoopBackward: {
                nextChild = ClampedWithTimerBehaviour(_currentChildIndex - 1);
            } break;
            case PingPong: {
                nextChild = ClampedWithTimerBehaviour(_currentChildIndex + _movingDirection);
            } break;
        }

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(nextChild, true)));
    }

    int Carousel::ClampedWithTimerBehaviour(int index) {
        switch (_carouselTimerBehaviour) {
            using enum CarouselTimerBehaviour;
            case None: {
                if (index >= _content->childCount) return _content->childCount - 1;
                if (index < 0) return 0;
            } break;
            // both loop kinds need to clamp the same way
            case LoopForward: [[fallthrough]];
            case LoopBackward: {
                if (index >= _content->childCount) return 0;
                if (index < 0) return _content->childCount - 1;
            } break;
            case PingPong: {
                if (index >= _content->childCount || index < 0) {
                    // reverse dir
                    _movingDirection = -_movingDirection;
                    return _currentChildIndex + _movingDirection;
                }
            } break;
        }
        return index;
    }

    void Carousel::UpdateButtonsInteractable() {
        switch(_carouselTimerBehaviour) {
            using enum CarouselTimerBehaviour;
            // pingpong and none should stop advancing beyond bounds
            case PingPong: [[fallthrough]];
            case None: {
                // if child index smaller than count, interactable
                _nextButton->interactable = _currentChildIndex < (_content->childCount - 1);
                // if child index greater than 0 (and we have at least 1 child), interactable
                _prevButton->interactable = _currentChildIndex > 0 && _content->childCount > 0;
            } break;
            // loop should just allow
            case LoopBackward: [[fallthrough]];
            case LoopForward: {
                _nextButton->interactable = true;
                _prevButton->interactable = true;
            } break;
        }
    }

    enum class PageButtonDirection {
        Up,
        Down,
        Left,
        Right
    };

    static std::map<std::string, PageButtonDirection> stringToPageButtonDirectionMap {
        {"Up", PageButtonDirection::Up},
        {"Down", PageButtonDirection::Down},
        {"Left", PageButtonDirection::Left},
        {"Right", PageButtonDirection::Right}
    };

    static PageButtonDirection stringToPageButtonDirection(const std::string& str) {
        auto itr = stringToPageButtonDirectionMap.find(str);
        if (itr == stringToPageButtonDirectionMap.end()) return PageButtonDirection::Up;
        return itr->second;
    }

    void SetButtonDirection(UnityEngine::UI::Button* pageButton, PageButtonDirection pageButtonDirection) {
        bool isHorizontal = false;
        int angle = 0;
        auto buttonTransform = pageButton->transform->Find("Icon").cast<UnityEngine::RectTransform>();
        buttonTransform->set_anchoredPosition({0, 0});
        auto layoutElement = pageButton->GetComponent<UnityEngine::UI::LayoutElement*>();
        switch(pageButtonDirection) {
            using enum PageButtonDirection;
            case Up:
                isHorizontal = true;
                angle = -180;
                break;
            case Down:
                isHorizontal = true;
                angle = 0;
                break;
            case Left:
                isHorizontal = false;
                angle = -90;
                break;
            case Right:
                isHorizontal = false;
                angle = 90;
                break;
        }
        buttonTransform->set_localRotation(UnityEngine::Quaternion::Euler(0, 0, angle));
        if (layoutElement->get_preferredHeight() == -1) {
            layoutElement->set_preferredHeight(isHorizontal ? 8 : 8);
        }

        if (layoutElement->get_preferredWidth() == -1) {
            layoutElement->set_preferredWidth(isHorizontal ? 8 : 8);
        }
    }

    void Carousel::SetTickerVertical() {
        // a vertical ticker means content & ticker are side by side
        _carouselLayoutGroup->SetLayoutHorizontal();
        // turn ticker vertical
        _tickerLayoutGroup->SetLayoutVertical();

        // set height & width for ticker layout
        _tickerLayoutElement->preferredHeight = -1;
        _tickerLayoutElement->preferredWidth = 8;

        // update fit modes
        _tickerSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;
        _tickerSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;

        // rotate buttons
        SetButtonDirection(_prevButton, PageButtonDirection::Up);
        SetButtonDirection(_nextButton, PageButtonDirection::Down);
    }

    void Carousel::SetTickerHorizontal() {
        // a horizontal ticker means content & ticker are over/under
        _carouselLayoutGroup->SetLayoutVertical();
        // turn ticker horizontal
        _tickerLayoutGroup->SetLayoutHorizontal();

        // set height & width for ticker layout
        _tickerLayoutElement->preferredHeight = 8;
        _tickerLayoutElement->preferredWidth = -1;

        // update fit modes
        _tickerSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;
        _tickerSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;

        // rotate buttons
        SetButtonDirection(_prevButton, PageButtonDirection::Left);
        SetButtonDirection(_nextButton, PageButtonDirection::Right);
    }

    void Carousel::SetContentVertical() {
        // set content vertical
        _contentLayoutGroup->SetLayoutVertical();
        // the content direction should be preferred, the other unconstrained
        _contentSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
        _contentSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;

        // update anchor stuff for vertical
        _content->anchorMin = {0, 1};
        _content->anchorMax = {1, 1};
        _content->pivot = {0.5, 1};
    }

    void Carousel::SetContentHorizontal() {
        // set content horizontal
        _contentLayoutGroup->SetLayoutHorizontal();
        // the content direction should be preferred, the other unconstrained
        _contentSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
        _contentSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;

        // update anchor stuff for horizontal
        _content->anchorMin = {0, 0};
        _content->anchorMax = {0, 1};
        _content->pivot = {0, 0.5};
    }

    void Carousel::MoveTicker(CarouselLocation location, CarouselDirection direction, bool force) {
        // no movement needed, was already in that place
        if (!force && location == _carouselLocation && direction == _carouselDirection) return;
        // carousel layout is the ticker & viewport
        // ticker layout is the ticker itself

        if (force || location != _carouselLocation) {
            switch (location) {
                using enum CarouselLocation;
                case Bottom: {
                    SetTickerHorizontal();
                    _ticker->SetAsLastSibling();
                } break;
                case Top: {
                    SetTickerHorizontal();
                    _ticker->SetAsFirstSibling();
                } break;
                case Left: {
                    SetTickerVertical();
                    _ticker->SetAsFirstSibling();
                } break;
                case Right: {
                    SetTickerVertical();
                    _ticker->SetAsFirstSibling();
                } break;
                case Default: {
                    switch (direction) {
                        using enum CarouselDirection;
                        // default for horizontal is the same as bottom
                        case Horizontal: {
                            SetTickerHorizontal();
                            _ticker->SetAsLastSibling();
                        } break;
                        // default for vertical is the same as left
                        case Vertical: {
                            SetTickerVertical();
                            _ticker->SetAsFirstSibling();
                        } break;
                    }
                } break;
            }
        }

        if (force || direction != _carouselDirection) {
            switch (direction) {
                using enum CarouselDirection;
                case Horizontal: {
                    SetContentHorizontal();
                } break;
                case Vertical: {
                    SetContentVertical();
                } break;
            }
        }

        // update variables
        _carouselLocation = location;
        _carouselDirection = direction;

        UpdateViewport();
    }

    void Carousel::UpdateViewport() {
        switch (_carouselDirection) {
            using enum CarouselDirection;
            case Horizontal: {
                // set size to rect width for horizontal
                SetContentSize(_content->rect.width);
            } break;
            case Vertical: {
                // set size to rect height for vertical
                SetContentSize(_content->rect.height);
            } break;
        }
    }

    void Carousel::SetContentSize(float size) {
        switch (_carouselDirection) {
            using enum CarouselDirection;
            case Horizontal: {
                _content->sizeDelta = {size, 0};
            } break;
            case Vertical: {
                _content->sizeDelta = {0, size};
            } break;
        }
    }

    void Carousel::Setup() {
        _viewPortHoverDetection = _viewPort->gameObject->AddComponent<HoverDetection*>();
        _viewPortHoverDetection->Enter += {&Carousel::OnPointerEnterViewport, this};
        _viewPortHoverDetection->Exit += {&Carousel::OnPointerExitViewport, this};
    }

    void Carousel::SetupAfterChildren() {
        int childCount = _content->childCount;

        auto parent = _bubblePrefab->transform->parent;
        _bubblePrefab->AddComponent<CarouselBubble*>();

        _carouselBubbles->EnsureCapacity(childCount);
        _carouselCanvasGroups->EnsureCapacity(childCount);
        for (int i = 0; i < childCount; i++) {
            auto bubbleGO = UnityEngine::Object::Instantiate(_bubblePrefab, parent);
            bubbleGO->transform->localScale = {0.7, 0.7, 0.7};
            auto bubble = bubbleGO->GetComponent<CarouselBubble*>();
            _carouselBubbles->Add(bubble);

            auto child = _content->GetChild(i);
            _carouselCanvasGroups->Add(child->gameObject->AddComponent<UnityEngine::CanvasGroup*>());
            bubbleGO->SetActive(true);
        }

        _nextButton->transform->SetAsLastSibling();

        UpdateViewport();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(_currentChildIndex, false)));
    }

    void Carousel::SetActiveBubble(int index) {
        for (int i = 0; auto bubble : _carouselBubbles) {
            bubble->Highlighted = index == i++;
        }
    }

    void Carousel::SetAlphaToGroups(int index) {
        for (int i = 0; auto group : _carouselCanvasGroups) {
            bool isActiveGroup = index == i++;
            group->alpha = isActiveGroup ? 1.0f : _inactiveAlpha;
            group->interactable = isActiveGroup;
        }
    }

    void Carousel::OnPointerEnterViewport() {
        SetIsHovered(true);
    }

    void Carousel::OnPointerExitViewport() {
        SetIsHovered(false);
    }


    void Carousel::SetIsHovered(bool isHovered) {
        _beingHovered = isHovered;
    }

    custom_types::Helpers::Coroutine Carousel::GotoChild(int childIndex, bool animated) {
        if (_isAnimating) co_return;
        if (childIndex >= _content->childCount || childIndex < 0) co_return;
        _isAnimating = true;

        auto currentChild = _content->GetChild(_currentChildIndex).cast<UnityEngine::RectTransform>();
        auto targetChild = _content->GetChild(childIndex).cast<UnityEngine::RectTransform>();

        auto targetPos = targetChild->anchoredPosition;
        auto childRect = targetChild->rect;
        auto viewPortRect = _viewPort->rect;

        auto currentPos = _content->anchoredPosition;

        // setup alignment
        switch (_carouselAlignment) {
            using enum CarouselAlignment;
            case Beginning: { // to get beginning we have to subtract half the childrect sizes
                targetPos.x -= childRect.width * 0.5f;
                targetPos.y -= childRect.height * 0.5f;
            } break;
            case Center: { // to get center we just subtract half the viewport size
                targetPos.x -= viewPortRect.width * 0.5f;
                targetPos.y -= viewPortRect.height * 0.5f;
            } break;
            case End: { // to get end we add have to add half the childrect sizes and subtract the entire viewport rect
                targetPos.x += childRect.width * 0.5f;
                targetPos.x -= viewPortRect.width;
                targetPos.y += childRect.height * 0.5f;
                targetPos.y -= viewPortRect.height;
            } break;
        }


        // 0 on the non used axis and flip the other
        switch (_carouselDirection) {
            using enum CarouselDirection;
            case Horizontal: {
                targetPos.y = 0;
                targetPos.x *= -1;
            } break;
            case Vertical: {
                targetPos.x = 0;
                targetPos.y *= -1;
            } break;
        }

        SetActiveBubble(childIndex);
        auto oldCanvasGroup = _carouselCanvasGroups[_currentChildIndex];
        auto newCanvasGroup = _carouselCanvasGroups[childIndex];

        float oldAlpha = oldCanvasGroup->alpha;
        float newAlpha = newCanvasGroup->alpha;

        // if animated, move it
        if (animated) {
            for (auto t = 0.0f; t < 1.0f; t += UnityEngine::Time::get_deltaTime() * 5.0f) {
                float eased = eased_t(t);
                _content->anchoredPosition = lerp(currentPos, targetPos, eased);
                oldCanvasGroup->alpha = lerp(oldAlpha, _inactiveAlpha, eased);
                newCanvasGroup->alpha = lerp(newAlpha, 1.0f, eased);
                co_yield nullptr;
            }
        }

        _content->anchoredPosition = targetPos;
        _currentChildIndex = childIndex;
        UpdateButtonsInteractable();
        SetAlphaToGroups(_currentChildIndex);

        _isAnimating = false;
        _timer = 0;
        co_return;
    }

    void Carousel::set_CurrentlyActiveChildIndex(int currentChildIndex) {
        if (_isAnimating) return;
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(currentChildIndex, false)));
    }

    void Carousel::SetCurrentlyActiveChildIndex(int index, bool animated) {
        if (_isAnimating) return;
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(index, animated)));
    }

    void Carousel::set_Direction(CarouselDirection direction) {
        MoveTicker(_carouselLocation, direction);
    }

    void Carousel::set_Location(CarouselLocation location) {
        MoveTicker(location, _carouselDirection);
    }

    void Carousel::set_TimerBehaviour(CarouselTimerBehaviour timerBehaviour) {
        _carouselTimerBehaviour = timerBehaviour;
        UpdateButtonsInteractable();
    }

    void Carousel::set_Alignment(CarouselAlignment alignment) {
        _carouselAlignment = alignment;
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(_currentChildIndex, false)));
    }

    void Carousel::set_ShowButtons(bool showButtons) {
        _showButtons = showButtons;
        _nextButton->gameObject->SetActive(_showButtons);
        _prevButton->gameObject->SetActive(_showButtons);
    }

    void Carousel::set_InactiveAlpha(float inactiveAlpha) {
        _inactiveAlpha = inactiveAlpha;
        SetAlphaToGroups(_currentChildIndex);
    }

    void CarouselBubble::ctor() {
        INVOKE_CTOR();
        UnityEngine::MonoBehaviour::_ctor();
        _defaultColor = {0.5, 0.5, 0.5, 1};
        _highlightColor = {1, 1, 1, 1};
    }

    HMUI::ImageView* CarouselBubble::get_Image() {
        if (!image) image = GetComponentInChildren<HMUI::ImageView*>(true);
        return image;
    }

    void CarouselBubble::UpdateHighlight() {
        Image->set_color(_highlighted ? _highlightColor : _defaultColor);
        Image->transform->localScale = _highlighted ? UnityEngine::Vector3{1.0, 1.0, 1.0} : UnityEngine::Vector3{0.5, 0.5, 0.5};
    }

    void HoverDetection::ctor() {
        INVOKE_CTOR();
    }

    void HoverDetection::OnPointerEnter(UnityEngine::EventSystems::PointerEventData* eventData) {
        Enter.invoke();
    }

    void HoverDetection::OnPointerExit(UnityEngine::EventSystems::PointerEventData* eventData) {
        Exit.invoke();
    }
}
