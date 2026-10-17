#pragma once
#include "config/rules.hpp"
#include <string>

using namespace ratelimiter::config;

struct RateLimitResponse {
    bool allowed;
    std::uint64_t limit;
    std::uint64_t remaining;
    std::uint64_t retry_after;
};

class RateLimiter {
    private:
        RulesConfig configRules;
        std::string REDIS_URL;
    public:
        RateLimiter(const std::string& configFilePath);
        RateLimitResponse checkLimit(const std::string& rule_id, const std::string& key);
};

class BadRateLimitRequest: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
};