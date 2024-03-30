#include "UI/PageOpener.hpp"
#include "UnityEngine/Application.hpp"
#include "logging.hpp"

DEFINE_TYPE(Umbrella::UI, PageOpener);

namespace Umbrella::UI {
    void PageOpener::OpenPage() {
        INFO("Page Open called! {}, {}", fmt::ptr(host), page);
        if (!host || !page) return;

        if (openInBrowser) {
            UnityEngine::Application::OpenURL(page);
        } else {
            // try to find the method and run it if found
            auto method = il2cpp_utils::FindMethod(host, "OpenPage", std::array<Il2CppClass*, 0>{}, std::array<Il2CppType const*, 1>{il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<StringW>::get()});
            if (method) il2cpp_utils::RunMethod(host, method, page);
        }
    }
}
