#include "Views/PageView.hpp"

DEFINE_TYPE(Umbrella::Views, PageView);

namespace Umbrella::Views {
    void PageView::ctor() {
        INVOKE_CTOR();
        HMUI::ViewController::_ctor();
    }

    void PageView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    }

    void PageView::LoadPage(std::string_view pageURL) {

    }
}
