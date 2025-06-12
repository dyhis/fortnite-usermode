
#ifndef DRM_SDK_H_
#define DRM_SDK_H_

#include <Windows.h>
#include <optional>
#include <string>
#include <string_view>
#include <filesystem>

namespace safeguard::drm {

    // Public enumeration
    enum class Result {
        kSuccess = 0,
        kConnectionError,
        kProtocolError,
        kServerError,
    };

    enum class VariableScope {
        kProgram,
        kUser
    };

    // Session
    /// Owns the IPC channel, performs the built-in challenge-response handshake,
    /// and exposes strongly-typed wrappers around the commands understood by the
    /// bootstrapper.
    class Session {
    public:
        Session() = default;
        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;
        ~Session() { Close(); }

        // Life-cycle
        /// Opens the pipe, completes the handshake, and leaves the channel ready
        /// for commands. Safe to call more than once; the second call is a no-op if
        /// the session is already connected.
        Result Connect();

        /// Closes the pipe (idempotent).
        void Close();

        // Commands
        Result Login(std::string_view username, std::string_view password);
        Result Register(std::string_view username, std::string_view password,
            std::string_view license_key);
        Result Inject(std::string_view dll_path, std::wstring_view process_name);
        Result GetVariable(std::string_view variable_name,
            std::string& out_value,
            VariableScope scope = VariableScope::kProgram,
            std::string_view target_user = {});

        /**
         * DOWNLOAD
         *
         * If save_path.has_value() => bootstrapper streams the file directly to
         * that absolute location on disk. Otherwise the bytes are kept in
         * memory on the bootstrapper side (future extension: we might expose a
         * secondary IPC to fetch them). Either way the call returns SUCCESS / ERROR.
         */
        Result Download(std::string_view file_name,
            std::vector<unsigned char>& out_bytes,
            std::optional<std::filesystem::path> save_path = std::nullopt);

        // Diagnostics
        /// Returns the raw error string provided by the bootstrapper for the
        /// last command that resulted in kServerError. It is cleared at the
        /// beginning of every command. If no error is available, returns an
        /// empty view.
        std::string_view LastError() const { return last_error_; }

    private:
        // Constants
        static constexpr const char* kPipeName = R"(\\.\pipe\DRMServicePipe)";
        static constexpr const char* kMagicToken = "3405691543";  // matches bootstrapper :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}

        // Helpers
        bool            IsOpen() const { return pipe_ != INVALID_HANDLE_VALUE; }
        bool            WriteLine(std::string_view line);
        std::optional<std::string> ReadLine();
        std::string_view StripPrefix(std::string_view text, std::string_view prefix);
        bool ReadExact(void* dst, size_t bytes);

        HANDLE          pipe_{ INVALID_HANDLE_VALUE };
        std::string     last_error_;
    };

    // Inline implementation
    inline Result Session::Connect() {

        if (IsOpen()) return Result::kSuccess;

        pipe_ = ::CreateFileA(kPipeName, GENERIC_READ | GENERIC_WRITE, 0,
            nullptr, OPEN_EXISTING, 0, nullptr);
        if (pipe_ == INVALID_HANDLE_VALUE) return Result::kConnectionError;

        // receive CHALLENGE:<num>\n
        auto line = ReadLine();
        if (!line || line->rfind(("CHALLENGE:"), 0) != 0) {
            Close();
            return Result::kProtocolError;
        }

        // send RESPONSE:<magic>\n
        std::string response = ("RESPONSE:");
        response += kMagicToken;
        response += '\n';
        if (!WriteLine(response)) {
            Close();
            return Result::kConnectionError;
        }

        return Result::kSuccess;
    }

    inline void Session::Close() {
        if (IsOpen()) {
            ::CloseHandle(pipe_);
            pipe_ = INVALID_HANDLE_VALUE;
        }
    }

    inline Result Session::Login(std::string_view username,
        std::string_view password) {

        if (Connect() != Result::kSuccess) return Result::kConnectionError;
        std::string cmd = ("LOGIN:");
        cmd.append(username).push_back(':');
        cmd.append(password).push_back('\n');
        if (!WriteLine(cmd)) return Result::kConnectionError;

        auto reply = ReadLine();
        if (!reply)                             return Result::kConnectionError;
        if (reply->rfind(("SUCCESS"), 0) == 0)    return Result::kSuccess;
        if (reply->rfind(("ERROR:"), 0) == 0) {
            last_error_ = StripPrefix(*reply, ("ERROR:"));
            return Result::kServerError;
        }
        return Result::kProtocolError;
    }

    inline Result Session::Register(std::string_view username,
        std::string_view password,
        std::string_view license_key) {

        if (Connect() != Result::kSuccess) return Result::kConnectionError;
        std::string cmd = ("REGISTER:");
        cmd.append(username).push_back((':'));
        cmd.append(password).push_back((':'));
        cmd.append(license_key).push_back('\n');
        if (!WriteLine(cmd)) return Result::kConnectionError;

        auto reply = ReadLine();
        if (!reply)                             return Result::kConnectionError;
        if (reply->rfind(("SUCCESS"), 0) == 0)    return Result::kSuccess;
        if (reply->rfind(("ERROR:"), 0) == 0) {
            last_error_ = StripPrefix(*reply, ("ERROR:"));
            return Result::kServerError;
        }
        return Result::kProtocolError;
    }


    inline Result Session::GetVariable(std::string_view variable_name, std::string& out_value, VariableScope scope, std::string_view target_user)
    {
        if (Connect() != Result::kSuccess) return Result::kConnectionError;

        std::string cmd = ("GETVAR:");
        cmd.append(variable_name);
        if (scope == VariableScope::kUser) cmd.append((":user"));
        if (!target_user.empty())
        {
            cmd.push_back((':'));
            cmd.append(target_user);
        }
        cmd.push_back(('\n'));

        if (!WriteLine(cmd)) return Result::kConnectionError;

        auto reply = ReadLine();
        if (!reply)                             return Result::kConnectionError;
        if (reply->rfind(("ERROR:"), 0) == 0) {
            last_error_ = StripPrefix(*reply, ("ERROR:"));
            return Result::kServerError;
        }
        if (reply->rfind(("SUCCESS:"), 0) != 0)    return Result::kProtocolError;

        out_value = std::string(StripPrefix(*reply, ("SUCCESS:")));
        return Result::kSuccess;
    }


    // Private helpers
    inline bool Session::WriteLine(std::string_view line) {
        DWORD written = 0;
        return ::WriteFile(pipe_, line.data(),
            static_cast<DWORD>(line.size()), &written, nullptr) != 0;
    }

    inline std::optional<std::string> Session::ReadLine() {
        constexpr DWORD kBufSize = 256;
        char            buffer[kBufSize]{};
        DWORD           read = 0;
        if (!::ReadFile(pipe_, buffer, kBufSize - 1, &read, nullptr)) return std::nullopt;
        return std::string(buffer, buffer + read);
    }

    // Extracts the payload after the given prefix (without trailing \n)
    [[nodiscard]] inline std::string_view Session::StripPrefix(std::string_view text, std::string_view prefix) {
        return text.substr(prefix.size());
    }

    inline bool Session::ReadExact(void* dst, size_t bytes)
    {
        char* p = static_cast<char*>(dst);
        while (bytes)
        {
            DWORD got = 0;
#define DWORD_MAX       0xffffffffUL
            if (!ReadFile(pipe_, p,
                static_cast<DWORD>(std::min(bytes,
                    static_cast<size_t>(DWORD_MAX))),
                &got, nullptr) || got == 0)
                return false;
            p += got;
            bytes -= got;
        }
        return true;
    }

}  // namespace safeguard::drm

#endif  // DRM_SDK_H_
