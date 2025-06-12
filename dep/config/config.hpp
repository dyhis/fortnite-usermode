#pragma once

class config 
{
public:
    using Value = std::variant<ImColor, ImVec2, float, bool, int>;

    void set(const std::string& key, const Value& value) {
        data_[key] = value;
    }

    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            if (auto val = std::get_if<T>(&it->second))
                return *val;
        }
        return std::nullopt;
    }

    bool save_to_file(const std::string& path) const;
    bool load_from_file(const std::string& path);

private:
    std::unordered_map<std::string, Value> data_;
    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
};
