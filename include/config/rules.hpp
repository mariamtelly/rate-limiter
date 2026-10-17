#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace ratelimiter::config {

    struct BucketConfig {
        std::uint64_t capacity = 0;
        std::uint64_t refill_tokens = 0;
        std::uint64_t refill_period_ms = 0;
    };

    struct Rule {
        std::string id;
        bool enabled = true;
        BucketConfig bucket;
    };

    struct RulesConfig {
        std::string version = "0";
        std::unordered_map<std::string, Rule> rules;
    };

    class ConfigError: public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    };

    RulesConfig load_rules(const std::string& path);
}