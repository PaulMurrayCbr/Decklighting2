/*
 * common.hpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMON_HPP_
#define SRC_CORE_COMMON_HPP_

#include <string>
#include <cstdint>
#include <unordered_map>
#include <map>


/*
 * so:
 * #define STR(x) #x
 * #define SECTION(S) \
 *   if(foo == Section::S) { \
 *     cout << "the name of the section is " << STR(s) << "\n"; \
 *   }
 * SECTION_LIST
 * #undef SECTION
 *
 * I think
 */

#define EFFECT_TYPE_LIST \
    EFFECT(SOLID) \
    EFFECT(THEATRE)

#define INTERPOLATION_TYPE_LIST \
    INTERPOLATION(FADE) \
    INTERPOLATION(QFADE)

#define SECTION_LIST \
    SECTION(Door) \
    SECTION(Game) \
    SECTION(Theatre) \
    SECTION(Deck) \
    SECTION(Back)

static const int NSECTIONS = 5;
static const int NCOLORANGES = 2;

enum class SectionEffectType {
#define EFFECT(name) name,
    EFFECT_TYPE_LIST
#undef EFFECT
};

enum class RgbInterpolationType {
#define INTERPOLATION(name) name,
    INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
};

enum class Section {
#define SECTION(name) name,
    SECTION_LIST
#undef SECTION
};

struct EnumClassHash {
    template<typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

extern const std::string EFFECT_TYPE_NAME[];
extern const std::string INTERPOLATION_TYPE_NAME[];
extern const std::string SECTION_NAME[];

extern const std::unordered_map<SectionEffectType, std::string, EnumClassHash> EFFECT_TYPE_NAME_OF;
extern const std::map<std::string, SectionEffectType> EFFECT_TYPE_ENUM_OF;
extern const std::unordered_map<RgbInterpolationType, std::string, EnumClassHash> INTERPOLATION_TYPE_NAME_OF;
extern const std::map<std::string, RgbInterpolationType> INTERPOLATION_TYPE_ENUM_OF;
extern const std::unordered_map<Section, std::string, EnumClassHash> SECTION_NAME_OF;
extern const std::map<std::string, Section> SECTION_ENUM_OF;

// throw std::out_of_range("map::at");

inline std::string sectionName(int section) {
    return SECTION_NAME_OF.at(static_cast<Section>(section));
}
inline std::string sectionName(Section section) {
    return SECTION_NAME_OF.at(section);
}
inline std::string effectName(int effect) {
    return EFFECT_TYPE_NAME_OF.at(static_cast<SectionEffectType>(effect));
}
inline std::string effectName(SectionEffectType effect) {
    return EFFECT_TYPE_NAME_OF.at(effect);
}
inline std::string interpolationName(int interpolation) {
    return INTERPOLATION_TYPE_NAME_OF.at(static_cast<RgbInterpolationType>(interpolation));
}
inline std::string interpolationName(RgbInterpolationType interpolation) {
    return INTERPOLATION_TYPE_NAME_OF.at(interpolation);
}
inline Section sectionLookup(std::string name) {
    return SECTION_ENUM_OF.at(name);
}
inline SectionEffectType effectLookup(std::string name) {
    return EFFECT_TYPE_ENUM_OF.at(name);
}
inline RgbInterpolationType interpolationLookup(std::string name) {
    return INTERPOLATION_TYPE_ENUM_OF.at(name);
}


enum class SectionMode {
    on, off, out
};

// these state values need to be structures that can be assigned with =

struct RGB {
    uint8_t r { 0 }, g { 0 }, b { 0 };
};



#endif /* SRC_CORE_COMMON_HPP_ */
