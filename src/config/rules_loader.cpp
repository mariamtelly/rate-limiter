#include "config/rules.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>

namespace ratelimiter::config {
    RulesConfig load_rules(const std::string& path) {
        // try {
        const YAML::Node& config = YAML::LoadFile(path);

        std::string version = config["version"].as<std::string>();
        const YAML::Node& rules = config["rules"];

        std::unordered_map<std::string, Rule> rules_map;
        for (YAML::const_iterator it = rules.begin(); it != rules.end(); ++it) {
            const YAML::Node& rule = *it;
            Rule rule_to_save = {
                rule["id"].as<std::string>(),
                rule["enabled"].as<bool>(),
                { 
                    rule["bucket_config"]["capacity"].as<std::uint64_t>(),
                    rule["bucket_config"]["refill"]["tokens"].as<std::uint64_t>(),
                    rule["bucket_config"]["refill"]["period_ms"].as<std::uint64_t>(),
                }
            };
            if (
                rule_to_save.bucket.capacity <= 0 
                || rule_to_save.bucket.refill_tokens <= 0
                || rule_to_save.bucket.refill_period_ms <= 0
            ) {
                throw ConfigError(path + ": Negative number not allowed.");
            }
            rules_map[rule["id"].as<std::string>()] = rule_to_save;
        }
        RulesConfig rules_config = {
            version,
            rules_map,
        };

        return rules_config;
        // } catch(const std::exception& e) {
        //     throw ConfigError(path + ": " + e.what());
        // }
    }
}