#include "rate_limiter.hpp"
#include <iostream>
#include <sw/redis++/redis++.h>
#include <vector>

const std::string TOKEN_BUCKET_SCRIPT = R"lua(
    local key           = KEYS[1]
    local capacity      = tonumber(ARGV[1])
    local refill_tokens = tonumber(ARGV[2])
    local refill_period_ms     = tonumber(ARGV[3])

    local t   = redis.call('TIME')
    local now = tonumber(t[1]) * 1000 + math.floor(tonumber(t[2]) / 1000)

    local data   = redis.call('HMGET', key, 'tokens', 'last_refill')
    local tokens = tonumber(data[1])
    local last = tonumber(data[2])

    if tokens == nil then
        tokens = capacity
        last   = now
    end

    local rate = refill_tokens / refill_period_ms
    local elapsed = math.max(0, now - last)
    tokens = math.min(capacity, tokens + elapsed * rate)

    local allowed, retry_after = 0, 0
    if tokens >= 1 then
        tokens = tokens - 1
        allowed = 1
    else
        retry_after = math.ceil((1 - tokens) / rate)
    end

    redis.call('HSET', key, 'tokens', tokens, 'last_refill', now)
    redis.call('PEXPIRE', key, math.ceil(capacity / rate))

    return { allowed, math.floor(tokens), retry_after }
)lua";

RateLimiter::RateLimiter(const std::string& configFilePath, const std::string& REDIS_URL) {
    RulesConfig configRules 
        = load_rules(configFilePath);

    this->configRules = configRules;
    this->REDIS_URL = REDIS_URL;
};

RateLimitResponse RateLimiter::checkLimit(const std::string& rule_id, const std::string& key) {
    auto it = this->configRules.rules.find(rule_id);

    if (it == this->configRules.rules.end()) {
        throw BadRateLimitRequest("No rule matching the key: " + rule_id);
    }

    const Rule& rule = it->second;
    if(!rule.enabled) {
        return { 
            true, 
            rule.bucket.capacity, 
            rule.bucket.capacity, 
            0 
        };
    }

    try {
        sw::redis::Redis redis_client = sw::redis::Redis(REDIS_URL);
        std::vector<long long> result;
        redis_client.eval(
            TOKEN_BUCKET_SCRIPT,
            { rule_id + "::" + key },
            {
                std::to_string(rule.bucket.capacity),
                std::to_string(rule.bucket.refill_tokens),
                std::to_string(rule.bucket.refill_period_ms),
            },
            std::back_insert_iterator(result),
        );
        
        return {
            result[0] == 1,
            bucket.capacity,
            static_cast<std::uint64_t>(result[1]),
            static_cast<std::uint64_t>(result[2]),
        };
    } catch(const sw::redis::Error& e) {
        std::cerr << "Rate limiter error, failed to reach redis server: " << e.what() << '\n';
        return { 
            true, 
            rule.bucket.capacity, 
            rule.bucket.capacity, 
            0 
        };
    }
}
