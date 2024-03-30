#pragma once

#include "bsml/shared/BSML/TypeHandlers/TypeHandler.hpp"
#include "UI/PageOpener.hpp"

namespace Umbrella::UI::TypeHandlers {
    class PageOpenerHandler : public BSML::TypeHandler<PageOpener*> {
        using Base = TypeHandler<PageOpener*>;
        using Base::Base;

        virtual Base::PropMap get_props() const override;
        virtual Base::SetterMap get_setters() const override;
        virtual void HandleType(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) override;
    };
}
