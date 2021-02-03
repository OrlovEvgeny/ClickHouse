#include <Coordination/SessionExpiryQueue.h>
#include <common/logger_useful.h>
namespace DB
{

bool SessionExpiryQueue::remove(int64_t session_id)
{
    auto session_it = session_to_timeout.find(session_id);
    if (session_it != session_to_timeout.end())
    {
        auto set_it = expiry_to_sessions.find(session_it->second);
        if (set_it != expiry_to_sessions.end())
            set_it->second.erase(session_id);

        return true;
    }

    return false;
}

bool SessionExpiryQueue::update(int64_t session_id, long timeout_ms)
{
    auto session_it = session_to_timeout.find(session_id);
    long now = getNowMilliseconds();
    long new_expiry_time = roundToNextInterval(now + timeout_ms);

    if (session_it != session_to_timeout.end())
    {
        if (new_expiry_time == session_it->second)
            return false;

        auto set_it = expiry_to_sessions.find(new_expiry_time);
        if (set_it == expiry_to_sessions.end())
            std::tie(set_it, std::ignore) = expiry_to_sessions.emplace(new_expiry_time, std::unordered_set<int64_t>());

        set_it->second.insert(session_id);
        long prev_expiry_time = session_it->second;

        if (prev_expiry_time != new_expiry_time)
        {
            auto prev_set_it = expiry_to_sessions.find(prev_expiry_time);
            if (prev_set_it != expiry_to_sessions.end())
                prev_set_it->second.erase(session_id);
        }
        session_it->second = new_expiry_time;
        return true;
    }
    else
    {
        session_to_timeout[session_id] = new_expiry_time;
        auto set_it = expiry_to_sessions.find(new_expiry_time);
        if (set_it == expiry_to_sessions.end())
            std::tie(set_it, std::ignore) = expiry_to_sessions.emplace(new_expiry_time, std::unordered_set<int64_t>());
        set_it->second.insert(session_id);
        return false;
    }
}

std::unordered_set<int64_t> SessionExpiryQueue::getExpiredSessions()
{
    long now = getNowMilliseconds();
    if (now < next_expiration_time)
        return {};

    auto set_it = expiry_to_sessions.find(next_expiration_time);
    long new_expiration_time = next_expiration_time + expiration_interval;
    next_expiration_time = new_expiration_time;
    if (set_it != expiry_to_sessions.end())
    {
        auto result = set_it->second;
        expiry_to_sessions.erase(set_it);
        return result;
    }
    return {};
}

}
