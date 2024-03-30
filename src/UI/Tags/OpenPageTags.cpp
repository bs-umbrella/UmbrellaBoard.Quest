#include "UI/Tags/OpenPageTags.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UI/PageOpener.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"

namespace Umbrella::UI::Tags {
    static BSML::BSMLNodeParser<OpenPageClickableTextTag> openPageClickableTextTagParser({"open-page-text"});
    static BSML::BSMLNodeParser<OpenPageClickableImageTag> openPageClickableImageTagParser({"open-page-image", "open-page-img"});
    static BSML::BSMLNodeParser<OpenPageButtonTag> openPageButtonTagParser({"open-page-button"});
    static BSML::BSMLNodeParser<OpenPagePrimaryButtonTag> openPagePrimaryButtonTagParser({"open-page-action-button"});
    static BSML::BSMLNodeParser<OpenPagePageButtonTag> openPagePageButtonTagParser({"open-page-page-button"});

    UnityEngine::GameObject* OpenPageClickableTextTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = this->ClickableTextTag::CreateObject(parent);
        auto opener = go->AddComponent<PageOpener*>();
        opener->activationSource = go->GetComponent<BSML::ClickableText*>();
        return go;
    }

    UnityEngine::GameObject* OpenPageClickableImageTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = this->ClickableImageTag::CreateObject(parent);
        auto opener = go->AddComponent<PageOpener*>();
        opener->activationSource = go->GetComponent<BSML::ClickableImage*>();
        return go;
    }

    UnityEngine::GameObject* OpenPageButtonTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = this->ButtonTag::CreateObject(parent);
        auto opener = go->AddComponent<PageOpener*>();
        opener->activationSource = go->GetComponent<UnityEngine::UI::Button*>();
        return go;
    }

    UnityEngine::GameObject* OpenPagePrimaryButtonTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = this->PrimaryButtonTag::CreateObject(parent);
        auto opener = go->AddComponent<PageOpener*>();
        opener->activationSource = go->GetComponent<UnityEngine::UI::Button*>();
        return go;
    }

    UnityEngine::GameObject* OpenPagePageButtonTag::CreateObject(UnityEngine::Transform* parent) const {
        auto go = this->PageButtonTag::CreateObject(parent);
        auto opener = go->AddComponent<PageOpener*>();
        opener->activationSource = go->GetComponent<UnityEngine::UI::Button*>();
        return go;
    }
}
