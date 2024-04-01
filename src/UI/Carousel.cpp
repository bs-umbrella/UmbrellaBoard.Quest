#include "UI/Carousel.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Rect.hpp"
#include "logging.hpp"
#include <map>

DEFINE_TYPE(Umbrella::UI, Carousel);
DEFINE_TYPE(Umbrella::UI, CarouselBubble);
DEFINE_TYPE(Umbrella::UI, HoverDetection);

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline UnityEngine::Vector2 lerp(UnityEngine::Vector2 a, UnityEngine::Vector2 b, float t) { return { lerp (a.x, b.x, t), lerp(a.y, b.y, t) }; }

namespace Umbrella::UI {
    void Carousel::ctor() {
        INVOKE_CTOR();
        _movingDirection = 1;
        _carouselBubbles = ListW<CarouselBubble*>::New();
        _timerLength = 10.0f;
    }

    void Carousel::Update() {
        if (!_isAnimating) {
            if (!_pauseOnHover || !_beingHovered) { // if not pause on hover, or if not being hovered, advance timer
                _timer += UnityEngine::Time::get_deltaTime();
            }

            if (TimerPassed) { // if the timer passed, advance selection
                AdvanceWithTimer();
            }
        }
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

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GotoChild(nextChild, false)));
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
                // if >= count not interactable
                _nextButton->interactable = _currentChildIndex >= _content->childCount;
                // if <= 0 not interactable
                _prevButton->interactable = _currentChildIndex <= 0;
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

        // rotate buttons
        SetButtonDirection(_prevButton, PageButtonDirection::Left);
        SetButtonDirection(_nextButton, PageButtonDirection::Right);
    }

    void Carousel::MoveTicker(CarouselLocation location, CarouselDirection direction) {
        // no movement needed, was already in that place
        if (location == _carouselLocation && direction == _carouselDirection) return;
        // carousel layout is the ticker & viewport
        // ticker layout is the ticker itself
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

        switch (direction) {
            using enum CarouselDirection;
            case Horizontal: {
                // set content horizontal
                _contentLayoutGroup->SetLayoutHorizontal();
                // the content direction should be preferred, the other unconstrained
                _contentSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
                _contentSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;

                // update anchor stuff for horizontal
                _content->anchorMin = {0, 0};
                _content->anchorMax = {0, 1};
                _content->pivot = {0, 0.5};

                // update control
                _contentLayoutGroup->childControlWidth = false;
                _contentLayoutGroup->childControlHeight = true;
            } break;
            case Vertical: {
                // set content horizontal
                _contentLayoutGroup->SetLayoutVertical();
                // the content direction should be preferred, the other unconstrained
                _contentSizeFitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
                _contentSizeFitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;

                // update anchor stuff for vertical
                _content->anchorMin = {0, 1};
                _content->anchorMax = {1, 1};
                _content->pivot = {0.5, 1};

                // update control
                _contentLayoutGroup->childControlHeight = false;
                _contentLayoutGroup->childControlWidth = true;
            } break;
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
                _content->sizeDelta = {size, _carouselLayoutElement->preferredHeight};
            } break;
            case Vertical: {
                _content->sizeDelta = {_carouselLayoutElement->preferredWidth, size};
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
        INFO("Had {} children", childCount);

        auto parent = _bubblePrefab->transform->parent;
        _bubblePrefab->AddComponent<CarouselBubble*>();

        for (int i = 0; i < childCount; i++) {
            auto bubble = UnityEngine::Object::Instantiate(_bubblePrefab, parent);
            bubble->transform->localScale = {0.7, 0.7, 0.7};
            _carouselBubbles->Add(bubble->GetComponent<CarouselBubble*>());
            bubble->SetActive(true);
        }

        _nextButton->transform->SetAsLastSibling();

        SetBubbleActive(_currentChildIndex);
        UpdateViewport();
    }

    void Carousel::SetBubbleActive(int index) {
        for (int i = 0; auto bubble : _carouselBubbles) {
            bubble->Highlighted = index == i++;
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
        _isAnimating = true;

        auto currentChildPos = _content->GetChild(_currentChildIndex).cast<UnityEngine::RectTransform>()->anchoredPosition;
        auto targetChildPos = _content->GetChild(childIndex).cast<UnityEngine::RectTransform>()->anchoredPosition;

        auto currentContentPos = _content->anchoredPosition;
        auto targetContentPos = _content->anchoredPosition;
        switch (_carouselDirection) {
            using enum CarouselDirection;
            case Horizontal: {
                auto delta = targetChildPos.x - currentChildPos.x;
                targetContentPos.x += delta;
            } break;
            case Vertical: {
                auto delta = targetChildPos.y - currentChildPos.y;
                targetContentPos.y += delta;
            } break;
        }

        if (animated) {
            for (auto t = 0.0f; t < 1.0f; t += UnityEngine::Time::get_deltaTime() * 5.0f) {
                _content->anchoredPosition = lerp(currentContentPos, targetContentPos, t);
                co_yield nullptr;
            }
        }

        _content->anchoredPosition = targetContentPos;

        SetBubbleActive(childIndex);

        _currentChildIndex = childIndex;
        UpdateButtonsInteractable();

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

    void Carousel::set_ShowButtons(bool showButtons) {
        _nextButton->gameObject->SetActive(showButtons);
        _prevButton->gameObject->SetActive(showButtons);
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
