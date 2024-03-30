#pragma once

#pragma once

#include "bsml/shared/BSML/Tags/BSMLTag.hpp"
#include "bsml/shared/BSML/Tags/ClickableTextTag.hpp"
#include "bsml/shared/BSML/Tags/ClickableImageTag.hpp"
#include "bsml/shared/BSML/Tags/ButtonTag.hpp"
#include "bsml/shared/BSML/Tags/PageButtonTag.hpp"
#include "bsml/shared/BSML/Tags/PrimaryButtonTag.hpp"

namespace Umbrella::UI::Tags {
    class OpenPageClickableTextTag : public BSML::ClickableTextTag {
        public:
            using BSML::ClickableTextTag::ClickableTextTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };

    class OpenPageClickableImageTag : public BSML::ClickableImageTag {
        public:
            using BSML::ClickableImageTag::ClickableImageTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };

    class OpenPageButtonTag : public BSML::ButtonTag {
        public:
            using BSML::ButtonTag::ButtonTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };

    class OpenPagePrimaryButtonTag : public BSML::PrimaryButtonTag {
        public:
            using BSML::PrimaryButtonTag::PrimaryButtonTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };

    class OpenPagePageButtonTag : public BSML::PageButtonTag {
        public:
            using BSML::PageButtonTag::PageButtonTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };
}
