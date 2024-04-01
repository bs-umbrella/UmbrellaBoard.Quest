#include "UI/Tags/CarouselTag.hpp"
#include "UI/Carousel.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/Mask.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/BSML/Components/ExternalComponents.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "assets.hpp"

namespace Umbrella::UI::Tags {
    static BSML::BSMLNodeParser<CarouselTag> carouselTagParser({"carousel", "bubble-carousel"});

    UnityEngine::GameObject* CarouselTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = UnityEngine::GameObject::New_ctor("Carousel");
        go->SetActive(false);
        go->transform->SetParent(parent, false);

        auto carousel = go->AddComponent<Carousel*>();
        auto hvGroup = go->AddComponent<UnityEngine::UI::VerticalLayoutGroup*>();
        hvGroup->SetLayoutVertical();
        carousel->_carouselLayoutGroup = hvGroup;


        auto vp = UnityEngine::GameObject::New_ctor("Viewport");
        vp->transform->SetParent(go->transform, false);

        auto vpMask = vp->AddComponent<UnityEngine::UI::Mask*>();
        auto vpImg = vp->AddComponent<HMUI::ImageView*>();
        auto vpRect = vp->transform.cast<UnityEngine::RectTransform>();
        vpMask->showMaskGraphic = false;
        vpImg->color = {1, 1, 1, 1};
        vpImg->sprite = BSML::Utilities::ImageResources::GetWhitePixel();
        vpImg->material = BSML::Helpers::GetUINoGlowMat();

        vpRect->localPosition = {0, 0, 0};
        vpRect->anchorMin = {0, 0};
        vpRect->anchorMax = {1, 1};
        vpRect->anchoredPosition = {0, 0};
        vpRect->sizeDelta = {0, 0};

        auto content = UnityEngine::GameObject::New_ctor("Content");
        content->transform->SetParent(vpRect, false);
        auto fitter = content->AddComponent<UnityEngine::UI::ContentSizeFitter*>();
        fitter->horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
        fitter->verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained;
        carousel->_contentSizeFitter = fitter;

        auto layout = content->AddComponent<UnityEngine::UI::HorizontalLayoutGroup*>();
        layout->childControlWidth = true;
        layout->childControlHeight = true;
        layout->childForceExpandWidth = false;
        layout->childForceExpandHeight = false;
        carousel->_contentLayoutGroup = layout;

        auto contentRect = content->transform.cast<UnityEngine::RectTransform>();
        contentRect->localPosition = {0, 0, 0};
        contentRect->anchorMin = {0, 0};
        contentRect->anchorMax = {0, 1};
        contentRect->anchoredPosition = {0, 0};

        carousel->_viewPort = vpRect;
        carousel->_content = contentRect;

        auto externalComponents = content->AddComponent<BSML::ExternalComponents*>();
        externalComponents->Add(go->transform.cast<UnityEngine::RectTransform>());
        externalComponents->Add(carousel);

        auto elem = go->AddComponent<UnityEngine::UI::LayoutElement*>();
        elem->preferredHeight = 50;
        elem->preferredWidth = 90;
        externalComponents->Add(elem);
        carousel->_carouselLayoutElement = elem;

        BSML::parse_and_construct(Assets::UI::carousel_ticker_bsml, carousel->transform, carousel);

        carousel->_tickerLayoutGroup = carousel->_ticker->GetComponent<UnityEngine::UI::HorizontalLayoutGroup*>();
        carousel->_tickerLayoutElement = carousel->_ticker->GetComponent<UnityEngine::UI::LayoutElement*>();
        carousel->_tickerSizeFitter = carousel->_ticker->GetComponent<UnityEngine::UI::ContentSizeFitter*>();

        carousel->_bubblePrefab->SetActive(false);

        carousel->_ticker->SetParent(carousel->transform, false);
        carousel->MoveTicker(Carousel::CarouselLocation::Default, Carousel::CarouselDirection::Horizontal, true);

        go->SetActive(true);
        // we return content for things to be parented to
        return content;
    }
}
