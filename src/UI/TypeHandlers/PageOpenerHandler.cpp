#include "UI/TypeHandlers/PageOpenerHandler.hpp"
#include "UI/PageOpener.hpp"
#include "bsml/shared/BSML/Components/ClickableText.hpp"
#include "bsml/shared/BSML/Components/ClickableImage.hpp"
#include "UnityEngine/UI/Button.hpp"

namespace Umbrella::UI::TypeHandlers {
    static PageOpenerHandler pageOpenerHandler{};

    PageOpenerHandler::Base::PropMap PageOpenerHandler::get_props() const {
        return {
            { "page", { "page", "url" } },
            { "openInBrowser", { "in-browser", "open-in-browser" } }
        };
    }

    PageOpenerHandler::Base::SetterMap PageOpenerHandler::get_setters() const {
        return {
            { "page", [](auto component, auto value){ component->page = value; }},
            { "openInBrowser", [](auto component, auto value){ component->openInBrowser = value; }},
        };
    }

    void PageOpenerHandler::HandleType(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) {
        auto pageOpener = reinterpret_cast<PageOpener*>(componentType.component);
        auto src = pageOpener->activationSource;
        pageOpener->host = parserParams.get_host();

        if (src) {
            if (auto clickableText = il2cpp_utils::try_cast<BSML::ClickableText>(src).value_or(nullptr); clickableText != nullptr) {
                clickableText->onClick += {&PageOpener::OpenPage, pageOpener};
            } else if (auto clickableImage = il2cpp_utils::try_cast<BSML::ClickableImage>(src).value_or(nullptr); clickableImage != nullptr) {
                clickableImage->onClick += {&PageOpener::OpenPage, pageOpener};
            } else if (auto button = il2cpp_utils::try_cast<UnityEngine::UI::Button>(src).value_or(nullptr); button != nullptr) {
                button->onClick->AddListener(BSML::MakeUnityAction(std::bind(&PageOpener::OpenPage, pageOpener)));
            } else {
                INFO("Could not cast activation source to a known type");
            }
        } else {
            WARNING("Activation source was null on a page opener");
        }

        Base::HandleType(componentType, parserParams);
    }
}
