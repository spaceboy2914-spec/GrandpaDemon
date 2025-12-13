#ifndef LISTMANAGER_H
#define LISTMANAGER_H
#include <vector>
#include <string>
#include <matjson.hpp>

using namespace geode::prelude;

class ListManager {
    public:
        inline static std::vector<int> demonIDList;
        inline static bool firstTimeOpen;
        inline static int filterType;
        inline static bool isSupremeSearching;

        inline static void parseData(matjson::Value const& data) {
            demonIDList.clear();

            if (!data.isArray()) {
                log::error("Grandpa Demon: Data is not an array!");
                return;
            }

            for (auto const& level : data.asArray().unwrap()) {
                // FIX: Look for "level_id" (GD ID) instead of "id" (AREDL UUID)
                if (level.contains("level_id") && level["level_id"].isNumber()) {
                    demonIDList.push_back(level["level_id"].asInt().unwrap());
                } 
                // Fallback: sometimes older APIs used "levelID" or "id" as a number
                else if (level.contains("levelID") && level["levelID"].isNumber()) {
                    demonIDList.push_back(level["levelID"].asInt().unwrap());
                }
                else if (level.contains("id") && level["id"].isNumber()) {
                     demonIDList.push_back(level["id"].asInt().unwrap());
                }
            }
            
            log::info("Grandpa Demon: Successfully parsed {} IDs", demonIDList.size());
        }

        inline static int getPositionOfID(int id) {
            for (unsigned int i = 0; i < demonIDList.size(); i++) {
                if (demonIDList.at(i) == id) return i;
            }
            return -1;
        }

        inline static CCSprite* getSpriteFromPosition(int pos, bool hasText) {
            // Rank 1
            if (pos == 0) {
                bool disableGrandpa = Mod::get()->getSettingValue<bool>("grandpa-demon-disable");
                if (disableGrandpa) {
                     return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon4_text.png"_spr : "GrD_demon4.png"_spr);
                } else {
                     return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon5_text.png"_spr : "GrD_demon5.png"_spr);
                }
            } 
            // Rank 2-25 (Silent Demon)
            if (pos > 0 && pos <= 24) {
                return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon4_text.png"_spr : "GrD_demon4.png"_spr);
            }
            // Rank 26-75 (Mythical Demon)
            if (pos > 24 && pos <= 74) {
                return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon3_text.png"_spr : "GrD_demon3.png"_spr);
            }
            // Rank 76-150 (Legendary Demon)
            if (pos > 74 && pos <= 149) {
                return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon2_text.png"_spr : "GrD_demon2.png"_spr);
            }
            // Rank 151-250 (Ultimate Demon)
            if (pos > 149 && pos <= 249) {
                return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon1_text.png"_spr : "GrD_demon1.png"_spr);
            }
            // Rank 251-500 (Supreme Demon)
            if (pos > 249 && pos <= 499) {
                return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon0_text.png"_spr : "GrD_demon0.png"_spr);
            }

            // Default fallback
            return CCSprite::createWithSpriteFrameName(hasText ? "GrD_demon0_text.png"_spr : "GrD_demon0.png"_spr);
        }

        inline static GJSearchObject* getSearchObject(int upper, int lower) {
            std::stringstream download;
            bool first = true;
            if (!(upper == 0 && lower == 0)) {
                for (unsigned int i = upper; i > lower; i--) {
                    if (i < demonIDList.size()) {
                        if (!first) download << ",";
                        download << std::to_string(ListManager::demonIDList.at(i));
                        first = false;
                    }
                }
            } else {
                if (!demonIDList.empty()) {
                    download << std::to_string(ListManager::demonIDList.at(0));
                }
            }
            
            download << "&gameVersion=22";
            GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type19, download.str());
            return searchObj;
        }   
};

#endif