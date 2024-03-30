#include "LoadingControl.hpp"
#include "bsml/shared/BSML.hpp"
#include "assets.hpp"

DEFINE_TYPE(Umbrella, LoadingControl);

namespace Umbrella {
    void LoadingControl::Awake() {
        BSML::parse_and_construct(Assets::loading_content_bsml, transform, this);
        BSML::parse_and_construct(Assets::error_content_bsml, transform, this);

        ShowLoading(false);
    }

    void LoadingControl::ShowLoading(bool isLoading, std::string_view loadingText) {
        if (isLoading && loadingText.empty()) loadingText = "Loading...";

        _loadingText->text = loadingText;
        _loadingContent->SetActive(isLoading);
        _errorContent->SetActive(false);
    }

    void LoadingControl::ShowError(std::string_view errorText) {
        if (errorText.empty()) errorText = "An error occurred!";
        _errorText->text = _loadingText;

        _loadingContent->SetActive(false);
        _errorContent->SetActive(true);
    }
}
