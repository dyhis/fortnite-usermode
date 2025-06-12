#include "general.hpp"

nlohmann::json config::to_json() const {
    nlohmann::json j;
    for (const auto& [key, value] : data_) {
        if (std::holds_alternative<ImColor>(value)) {
            const ImColor& c = std::get<ImColor>(value);
            j[key] = { c.Value.x, c.Value.y, c.Value.z, c.Value.w };
        }
        else if (std::holds_alternative<ImVec2>(value)) {
            const ImVec2& v = std::get<ImVec2>(value);
            j[key] = { v.x, v.y };
        }
        else if (std::holds_alternative<float>(value)) {
            j[key] = std::get<float>(value);
        }
        else if (std::holds_alternative<bool>(value)) {
            j[key] = std::get<bool>(value);
        }
        else if (std::holds_alternative<int>(value)) {
            j[key] = std::get<int>(value);
        }
    }
    return j;
}

void config::from_json(const nlohmann::json& j) {
    data_.clear();
    for (auto it = j.begin(); it != j.end(); ++it) {
        const std::string& key = it.key();
        const auto& val = it.value();
        if (val.is_array() && val.size() == 4) {
            data_[key] = ImColor(val[0].get<float>(), val[1].get<float>(), val[2].get<float>(), val[3].get<float>());
        }
        else if (val.is_array() && val.size() == 2) {
            data_[key] = ImVec2(val[0].get<float>(), val[1].get<float>());
        }
        else if (val.is_boolean()) {
            data_[key] = val.get<bool>();
        }
        else if (val.is_number_float()) {
            data_[key] = val.get<float>();
        }
        else if (val.is_number_integer()) {
            data_[key] = val.get<int>();
        }
    }
}

bool config::save_to_file(const std::string& path) const 
{
    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }

    auto json_str = to_json().dump(4);

    out << json_str;
    if (out.fail()) {
        return false;
    }

    return true;
}

bool config::load_from_file(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return false;
    }

    // Read entire file into string
    std::string file_contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    // Parse with error handler instead of exceptions
    nlohmann::json j;
    auto result = nlohmann::json::parse(file_contents, nullptr, false);
    if (result.is_discarded()) {
        return false; // parse failed
    }
    j = std::move(result);

    from_json(j);
    return true;
}