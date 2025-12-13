#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include "ListManager.h"
#include <matjson.hpp> 

using namespace geode::prelude;

class $modify(MenuLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        if (ListManager::firstTimeOpen) {
            return true;
        }

        m_fields->m_listener.bind([] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                
                // 1. Check for HTTP errors
                if (!res->ok()) {
                    ListManager::firstTimeOpen = true;
                    ListManager::filterType = -2;
					log::error("Grandpa Demon: Failed to load list. HTTP Code: {}", res->code()); 
                    
                    std::string errorStr = fmt::format(
                        "\n\n<cr>Could not load data from AREDL.</c>\nHTTP Error: {}\n\n<cb>-Grandpa Demon</c>", 
                        res->code()
                    );
                    FLAlertLayer::create("What the??", errorStr, "OK")->show();
                    return;
                }

                // 2. Parse JSON
                auto jsonRes = res->json();
                
                if (jsonRes.isErr()) {
                    ListManager::firstTimeOpen = true;
                    ListManager::filterType = -2;
                    log::error("Grandpa Demon: JSON Parsing Error: {}", jsonRes.unwrapErr().c_str()); 
                    return;
                }

                // Send to ListManager
                ListManager::parseData(jsonRes.unwrap());
                ListManager::firstTimeOpen = true;
                ListManager::filterType = -1;
                
                log::info("Grandpa Demon: Loaded {} levels from AREDL.", ListManager::demonIDList.size());
            }
            else if (e->isCancelled()) {
                // Handle cancellation if needed
				log::warn("Grandpa Demon: Request cancelled.");
            }
        });

        // URL CHANGE: The endpoint /list is often deprecated or redirects. 
        // usually /levels returns the full list array in V2 API.
        auto req = web::WebRequest();
        
        // CRITICAL FIX: Set a User-Agent to avoid 403 Forbidden from Cloudflare
        req.userAgent("GrandpaDemonMod/2.0"); 
        
        m_fields->m_listener.setFilter(req.get("https://api.aredl.net/api/aredl/levels"));

        return true;
    }
};