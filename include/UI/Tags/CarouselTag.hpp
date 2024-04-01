#pragma once

#include "bsml/shared/BSML/Tags/BSMLTag.hpp"

namespace Umbrella::UI::Tags {
    class CarouselTag : public BSML::BSMLTag {
        public:
            using BSML::BSMLTag::BSMLTag;
        protected:
            virtual UnityEngine::GameObject* CreateObject(UnityEngine::Transform* parent) const override;
    };
}
