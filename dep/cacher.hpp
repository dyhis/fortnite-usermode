#ifndef CACHER_HPP
#define CACHER_HPP

template <typename key, typename value>
class cacher
{
public:
    value cache_or_fetch(const key& key, std::function<value()> fetch_function)
    {
        auto it = cache.find(key);
        if (it != cache.end())
        {
            return it->second;
        }

        value new_value = fetch_function();
        cache[key] = new_value;
        return new_value;
    }

    void invalidate(const key& key)
    {
        cache.erase(key);
    }

    void clear()
    {
        cache.clear();
    }

    size_t size() const
    {
        return cache.size();
    }

private:
    std::unordered_map<key, value> cache;
};

#endif // CACHER_HPP