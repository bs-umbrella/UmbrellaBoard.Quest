#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(Umbrella::UI, PageOpener, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(StringW, page);
    DECLARE_INSTANCE_FIELD(System::Object*, host);
    DECLARE_INSTANCE_FIELD(UnityEngine::Component*, activationSource);

    DECLARE_INSTANCE_METHOD(void, OpenPage);
)
