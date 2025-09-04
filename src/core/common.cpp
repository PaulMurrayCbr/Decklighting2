/*
 * common.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#include "common.hpp"

const std::string EFFECT_TYPE_NAME[] = {
#define EFFECT(name) #name,
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::string INTERPOLATION_TYPE_NAME[] = {
#define INTERPOLATION(name) #name,
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::string SECTION_NAME[] = {
#define SECTION(name) #name,
        SECTION_LIST
#undef SECTION
        };

const std::unordered_map<SectionEffectType, std::string, EnumClassHash> EFFECT_TYPE_NAME_OF = {
#define EFFECT(name) { SectionEffectType::name, #name},
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::map<std::string, SectionEffectType> EFFECT_TYPE_ENUM_OF = {
#define EFFECT(name) { #name, SectionEffectType::name},
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::unordered_map<RgbInterpolationType, std::string, EnumClassHash> INTERPOLATION_TYPE_NAME_OF = {
#define INTERPOLATION(name) { RgbInterpolationType::name, #name},
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::map<std::string, RgbInterpolationType> INTERPOLATION_TYPE_ENUM_OF = {
#define INTERPOLATION(name) { #name, RgbInterpolationType::name},
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::unordered_map<Section, std::string, EnumClassHash> SECTION_NAME_OF = {
#define SECTION(name) { Section::name, #name},
        SECTION_LIST
#undef SECTION
        };

const std::map<std::string, Section> SECTION_ENUM_OF = {
#define SECTION(name) { #name, Section::name},
        SECTION_LIST
#undef SECTION
        };

const int SECTION_LEN[NSECTIONS] = {
#define SECTION(name) name##_LEN ,
        SECTION_LIST
#undef SECTION

        };
