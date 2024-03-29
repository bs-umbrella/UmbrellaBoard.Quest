#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

DECLARE_CLASS_CODEGEN(Umbrella, LoadingControl, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, _loadingContent);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, _loadingText);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, _errorContent);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, _errorText);

    DECLARE_INSTANCE_METHOD(void, Awake);
    public:
        void ShowLoading(bool isLoading, std::string_view loadingText = "");
        void ShowError(std::string_view errorText = "");
)
