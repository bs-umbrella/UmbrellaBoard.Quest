#include "Views/PlaceHolderView.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"

DEFINE_TYPE(Umbrella::Views, PlaceHolderView);

namespace Umbrella::Views {
    void PlaceHolderView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();
    }

    void PlaceHolderView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            BSML::parse_and_construct(Assets::placeholder_view_bsml, transform, this);

            auto r = rectTransform;
            r->sizeDelta = {100, 0};
            r->anchorMin = {0.5, 0};
            r->anchorMax = {0.5, 1};
        }
    }
}
