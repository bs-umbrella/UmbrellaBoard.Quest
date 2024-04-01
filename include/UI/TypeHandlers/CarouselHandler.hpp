#pragma once

#include "bsml/shared/BSML/TypeHandlers/TypeHandler.hpp"
#include "UI/Carousel.hpp"

namespace Umbrella::UI::TypeHandlers {
    class CarouselHandler : public BSML::TypeHandler<Carousel*> {
        using Base = TypeHandler<Carousel*>;
        using Base::Base;

        virtual Base::PropMap get_props() const override;
        virtual Base::SetterMap get_setters() const override;
        virtual void HandleType(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) override;
        virtual void HandleTypeAfterChildren(const BSML::ComponentTypeWithData& componentType, BSML::BSMLParserParams& parserParams) override;
    };
}
