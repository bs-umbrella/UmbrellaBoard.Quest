#include "UI/TypeHandlers/CarouselHandler.hpp"
#include <string>
#include <map>

namespace Umbrella::UI::TypeHandlers {
    static CarouselHandler carouselHandler{};
    static Carousel::CarouselDirection ParseDirection(std::string const& str);
    static Carousel::CarouselLocation ParseLocation(std::string const& str);
    static Carousel::CarouselTimerBehaviour ParseTimerBehaviour(std::string const& str);
    static Carousel::CarouselAlignment ParseAlignment(std::string const& str);

    CarouselHandler::Base::PropMap CarouselHandler::get_props() const {
        return {
            { "startChildIndex", { "start-child-index", "start-index" } },
            { "direction", { "direction" } },
            { "location", { "location", "ticker-location" } },
            { "timerBehaviour", { "timer-behaviour" } },
            { "timerLength", { "timer-length" } },
            { "contentAlignment", { "content-alignment"} },
            { "showButtons", { "show-buttons" } },
            { "pauseOnHover", { "pause-on-hover" } },
            { "inactiveAlpha", { "inactive-alpha" } },
        };
    }
    CarouselHandler::Base::SetterMap CarouselHandler::get_setters() const {
        return {
            { "direction",          [](auto component, auto value){ component->set_Direction(ParseDirection(value)); }},
            { "location",           [](auto component, auto value){ component->set_Location(ParseLocation(value)); }},
            { "timerBehaviour",     [](auto component, auto value){ component->set_TimerBehaviour(ParseTimerBehaviour(value)); }},
            { "timerLength",        [](auto component, auto value){ component->set_TimerLength(value); }},
            { "contentAlignment",   [](auto component, auto value){ component->set_Alignment(ParseAlignment(value)); }},
            { "showButtons",        [](auto component, auto value){ component->set_ShowButtons(value); }},
            { "pauseOnHover",       [](auto component, auto value){ component->set_PauseOnHover(value); }},
            { "inactiveAlpha",      [](auto component, auto value){ component->set_InactiveAlpha(value); }}
        };
    }

    void CarouselHandler::HandleType(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) {
        CarouselHandler::Base::HandleType(componentType, parserParams);
        auto component = reinterpret_cast<Carousel*>(componentType.component);
        component->Setup();
    }

    void CarouselHandler::HandleTypeAfterChildren(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) {
        CarouselHandler::Base::HandleTypeAfterChildren(componentType, parserParams);
        auto component = reinterpret_cast<Carousel*>(componentType.component);
        component->SetupAfterChildren();

        auto& data = componentType.data;
        auto startChildIndexItr = data.find("startChildIndex");
        if (startChildIndexItr != data.end()) {
            component->SetCurrentlyActiveChildIndex(StringParseHelper(startChildIndexItr->second), false);
        }
    }

    Carousel::CarouselDirection ParseDirection(std::string const& str) {
        if (str == "Vertical") return Carousel::CarouselDirection::Vertical;
        if (str == "Horizontal") return Carousel::CarouselDirection::Horizontal;

        return Carousel::CarouselDirection::Horizontal;
    }

    Carousel::CarouselLocation ParseLocation(std::string const& str) {
        if (str == "Default") return Carousel::CarouselLocation::Default;
        if (str == "Bottom") return Carousel::CarouselLocation::Bottom;
        if (str == "Top") return Carousel::CarouselLocation::Top;
        if (str == "Left") return Carousel::CarouselLocation::Left;
        if (str == "Right") return Carousel::CarouselLocation::Right;

        return Carousel::CarouselLocation::Default;
    }

    Carousel::CarouselTimerBehaviour ParseTimerBehaviour(std::string const& str) {
        if (str == "None") return Carousel::CarouselTimerBehaviour::None;
        if (str == "PingPong") return Carousel::CarouselTimerBehaviour::PingPong;
        if (str == "Loop") return Carousel::CarouselTimerBehaviour::Loop;
        if (str == "LoopForward") return Carousel::CarouselTimerBehaviour::LoopForward;
        if (str == "LoopBackward") return Carousel::CarouselTimerBehaviour::LoopBackward;

        return Carousel::CarouselTimerBehaviour::None;
    }

    Carousel::CarouselAlignment ParseAlignment(std::string const& str) {
        if (str == "Beginning") return Carousel::CarouselAlignment::Beginning;
        if (str == "Middle") return Carousel::CarouselAlignment::Middle;
        if (str == "Center") return Carousel::CarouselAlignment::Center;
        if (str == "End") return Carousel::CarouselAlignment::End;

        return Carousel::CarouselAlignment::Center;
    }
}
