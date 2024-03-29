#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(Umbrella::Views, PageView, HMUI::ViewController,
        DECLARE_CTOR(ctor);
        DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    public:
        UnorderedEventCallback<std::string_view> PageWasOpened;

        void LoadPage(std::string_view pageURL);
)
