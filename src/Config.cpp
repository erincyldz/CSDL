#include "Config.hpp"

#include "Logger.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <variant>

namespace game
{
namespace
{
using FieldRef =
    std::variant<bool Config::*, int Config::*, double Config::*, std::string Config::*>;

struct Field
{
    const char* key;
    FieldRef ref;
    const char* help;
};

// The array size is deduced, so adding an option can't leave an uninitialized entry behind.
const Field FIELDS[] = {
    {"window_width", &Config::windowWidth, "Initial window width in pixels"},
    {"window_height", &Config::windowHeight, "Initial window height in pixels"},
    {"physics_hz", &Config::physicsHz, "Physics steps per second"},
    {"time_scale", &Config::timeScale, "Simulation speed multiplier (0.05 .. 8)"},
    {"vsync", &Config::vsync, "Synchronize rendering with the display"},
    {"max_fps", &Config::maxFps, "Frame cap (0 = none)"},
    {"show_ui", &Config::showUi, "Show the sandbox panel (toggle with F1)"},
    {"ui_scale", &Config::uiScale, "Sandbox panel scale (0.5 .. 4)"},
    {"scene", &Config::scene, "Scene file to load at start and on reset"},
    {"random_objects", &Config::randomObjects, "Start with N random objects (0 = default scene)"},
    {"max_objects", &Config::maxObjects, "Maximum number of random objects"},
    {"gravity", &Config::gravity, "Enable mutual gravitation"},
    {"gravitational_constant", &Config::gravitationalConstant, "Gravitational constant G"},
    {"softening", &Config::softening, "Gravity softening length (px)"},
    {"border_collision", &Config::borderCollision, "Bounce off window borders (else wrap)"},
    {"max_speed", &Config::maxSpeed, "Maximum object speed (px/s, 0 = unlimited)"},
    {"restitution_threshold", &Config::restitutionThreshold,
     "Closing speed below which collisions are inelastic"},
    {"render_trails", &Config::renderTrails, "Draw object trails"},
    {"render_direction", &Config::renderDirection, "Draw velocity arrows"},
    {"render_collision_highlights", &Config::renderCollisionHighlights,
     "Outline colliding objects"},
    {"color_cycle", &Config::colorCycle, "Cycle object colors over time"},
    {"sound", &Config::sound, "Enable sound effects"},
    {"volume", &Config::volume, "Volume 0..100"},
    {"music_path", &Config::musicPath, "Background music file (optional)"},
    {"asset_dir", &Config::assetDir, "Asset directory (default: auto-detect)"},
    {"log_level", &Config::logLevel, "debug | info | warning | error"},
};

std::string normalizeKey(std::string_view key)
{
    std::string out(key);
    std::replace(out.begin(), out.end(), '-', '_');
    return out;
}

const Field* findField(std::string_view key)
{
    const std::string k = normalizeKey(key);
    for (const auto& f : FIELDS)
        if (k == f.key)
            return &f;
    return nullptr;
}

std::string_view trim(std::string_view s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
        s.remove_prefix(1);
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.remove_suffix(1);
    return s;
}

bool parseBool(std::string_view v, bool& out)
{
    if (v == "1" || v == "true" || v == "yes" || v == "on")
        return out = true, true;
    if (v == "0" || v == "false" || v == "no" || v == "off")
        return out = false, true;
    return false;
}

bool parseInt(std::string_view v, int& out)
{
    const std::string s(v);
    char* end = nullptr;
    errno = 0;
    const long value = std::strtol(s.c_str(), &end, 10);
    if (s.empty() || *end != '\0' || errno != 0 || value < INT32_MIN || value > INT32_MAX)
        return false;
    out = static_cast<int>(value);
    return true;
}

bool parseDouble(std::string_view v, double& out)
{
    const std::string s(v);
    char* end = nullptr;
    errno = 0;
    const double value = std::strtod(s.c_str(), &end);
    if (s.empty() || *end != '\0' || errno != 0)
        return false;
    out = value;
    return true;
}

bool isBoolField(const Field& f)
{
    return std::holds_alternative<bool Config::*>(f.ref);
}

std::string valueToString(const Config& config, const Field& f)
{
    return std::visit(
        [&](auto member) -> std::string
        {
            using T = std::decay_t<decltype(config.*member)>;
            if constexpr (std::is_same_v<T, bool>)
                return (config.*member) ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::string>)
                return (config.*member).empty() ? "-" : config.*member;
            else
            {
                std::ostringstream oss;
                oss << config.*member;
                return oss.str();
            }
        },
        f.ref);
}
}  // namespace

bool setConfigValue(Config& config, std::string_view key, std::string_view value,
                    std::string& error)
{
    const Field* field = findField(key);
    if (!field)
    {
        error = "unknown option '" + std::string(key) + "'";
        return false;
    }

    const bool ok = std::visit(
        [&](auto member) -> bool
        {
            using T = std::decay_t<decltype(config.*member)>;
            if constexpr (std::is_same_v<T, bool>)
                return parseBool(value, config.*member);
            else if constexpr (std::is_same_v<T, int>)
                return parseInt(value, config.*member);
            else if constexpr (std::is_same_v<T, double>)
                return parseDouble(value, config.*member);
            else
                return (config.*member = std::string(value), true);
        },
        field->ref);

    if (!ok)
        error = "invalid value '" + std::string(value) + "' for '" + field->key + "'";
    return ok;
}

bool loadConfigFile(Config& config, const std::string& path, std::string& error)
{
    std::ifstream file(path);
    if (!file)
    {
        error = "cannot open config file '" + path + "'";
        return false;
    }

    std::string line;
    int lineNo = 0;
    while (std::getline(file, line))
    {
        ++lineNo;
        std::string_view view = line;
        if (const auto hash = view.find('#'); hash != std::string_view::npos)
            view = view.substr(0, hash);
        view = trim(view);
        if (view.empty())
            continue;

        const auto eq = view.find('=');
        if (eq == std::string_view::npos)
        {
            error = path + ":" + std::to_string(lineNo) + ": expected 'key = value'";
            return false;
        }
        std::string fieldError;
        if (!setConfigValue(config, trim(view.substr(0, eq)), trim(view.substr(eq + 1)),
                            fieldError))
        {
            error = path + ":" + std::to_string(lineNo) + ": " + fieldError;
            return false;
        }
    }
    return true;
}

bool validateConfig(const Config& c, std::string& error)
{
    auto fail = [&](const std::string& msg)
    {
        return error = msg, false;
    };
    if (c.windowWidth < 200 || c.windowHeight < 200)
        return fail("window size must be at least 200x200");
    if (c.physicsHz < 10 || c.physicsHz > 2000)
        return fail("physics_hz must be in [10, 2000]");
    if (c.maxFps < 0)
        return fail("max_fps must be >= 0");
    if (c.randomObjects < 0 || c.maxObjects < 0)
        return fail("object counts must be >= 0");
    if (c.volume < 0 || c.volume > 100)
        return fail("volume must be in [0, 100]");
    if (c.timeScale < 0.05 || c.timeScale > 8.0)
        return fail("time_scale must be in [0.05, 8]");
    if (c.uiScale < 0.5 || c.uiScale > 4.0)
        return fail("ui_scale must be in [0.5, 4]");
    if (c.softening < 0.0 || c.maxSpeed < 0.0 || c.restitutionThreshold < 0.0)
        return fail("softening, max_speed and restitution_threshold must be >= 0");
    if (!utils::Logger::parseLevel(c.logLevel))
        return fail("log_level must be one of debug, info, warning, error");
    return true;
}

ParseResult parseArgs(Config& config, int argc, char* argv[], std::ostream& err)
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string_view arg = argv[i];

        // Legacy short flags for the log level.
        if (arg == "-d" || arg == "-i" || arg == "-w" || arg == "-e")
        {
            config.logLevel = arg == "-d"   ? "debug"
                              : arg == "-i" ? "info"
                              : arg == "-w" ? "warning"
                                            : "error";
            continue;
        }
        if (arg == "-h" || arg == "--help")
        {
            printUsage(std::cout, argv[0]);
            return ParseResult::ExitSuccess;
        }
        if (arg.substr(0, 2) != "--")
        {
            err << "Unexpected argument '" << arg << "'. Use --help for usage.\n";
            return ParseResult::ExitFailure;
        }

        std::string_view body = arg.substr(2);
        std::string_view key = body;
        std::optional<std::string_view> value;
        if (const auto eq = body.find('='); eq != std::string_view::npos)
        {
            key = body.substr(0, eq);
            value = body.substr(eq + 1);
        }

        // --config <file> loads a file at this position; later options override it.
        if (key == "config")
        {
            if (!value)
            {
                if (i + 1 >= argc)
                {
                    err << "Option '--config' needs a file name.\n";
                    return ParseResult::ExitFailure;
                }
                value = argv[++i];
            }
            std::string error;
            if (!loadConfigFile(config, std::string(*value), error))
            {
                err << error << '\n';
                return ParseResult::ExitFailure;
            }
            continue;
        }

        const Field* field = findField(key);
        if (!field && !value && key.substr(0, 3) == "no-")
        {
            const Field* negated = findField(key.substr(3));
            if (negated && isBoolField(*negated))
            {
                config.*std::get<bool Config::*>(negated->ref) = false;
                continue;
            }
        }
        if (!field)
        {
            err << "Unknown option '--" << key << "'. Use --help for usage.\n";
            return ParseResult::ExitFailure;
        }

        if (!value)
        {
            if (isBoolField(*field))
            {
                config.*std::get<bool Config::*>(field->ref) = true;
                continue;
            }
            if (i + 1 >= argc)
            {
                err << "Option '--" << key << "' needs a value.\n";
                return ParseResult::ExitFailure;
            }
            value = argv[++i];
        }

        std::string error;
        if (!setConfigValue(config, field->key, *value, error))
        {
            err << error << '\n';
            return ParseResult::ExitFailure;
        }
    }

    std::string error;
    if (!validateConfig(config, error))
    {
        err << "Invalid configuration: " << error << '\n';
        return ParseResult::ExitFailure;
    }
    return ParseResult::Ok;
}

physics::PhysicsSettings physicsSettingsFrom(const Config& c)
{
    physics::PhysicsSettings s;
    s.gravity = c.gravity;
    s.gravitationalConstant = c.gravitationalConstant;
    s.softening = c.softening;
    s.borderCollision = c.borderCollision;
    s.maxSpeed = c.maxSpeed;
    s.restitutionThreshold = c.restitutionThreshold;
    s.colorCycle = c.colorCycle;
    return s;
}

void printUsage(std::ostream& out, const char* program)
{
    out << "Usage: " << program << " [options]\n\n"
        << "Options (--key=value, --key value, --flag, --no-flag):\n";
    const Config defaults;
    for (const auto& f : FIELDS)
    {
        std::string name = std::string("--") + f.key;
        std::replace(name.begin(), name.end(), '_', '-');
        out << "  " << std::left << std::setw(32) << name << f.help
            << " [default: " << valueToString(defaults, f) << "]\n";
    }
    out << "  " << std::left << std::setw(32) << "--config <file>"
        << "Load a config file (later options override it)\n";
    out << "  " << std::left << std::setw(32) << "-d | -i | -w | -e"
        << "Shortcut for --log-level debug|info|warning|error\n"
        << "  " << std::left << std::setw(32) << "-h, --help" << "Show this help\n\n"
        << "Config files use one 'key = value' per line; '#' starts a comment.\n";
}

void printConfig(std::ostream& out, const Config& config)
{
    constexpr int KEY_WIDTH = 30;
    constexpr int VALUE_WIDTH = 15;
    const char* RESET = "\033[0m";
    const char* RED = "\033[31m";
    const char* BLUE = "\033[34m";
    const char* GREEN = "\033[32m";
    const std::string border =
        "+" + std::string(KEY_WIDTH, '-') + "+" + std::string(VALUE_WIDTH, '-') + "+";

    out << border << '\n'
        << "| " << RED << std::setw(KEY_WIDTH - 2) << std::left << "Configuration" << RESET << " | "
        << RED << std::setw(VALUE_WIDTH - 2) << "Value" << RESET << " |\n"
        << border << '\n';
    for (const auto& f : FIELDS)
    {
        out << "| " << BLUE << std::setw(KEY_WIDTH - 2) << std::left << f.key << RESET << " | "
            << GREEN << std::setw(VALUE_WIDTH - 2) << std::right << valueToString(config, f)
            << RESET << " |\n";
    }
    out << border << std::endl;
}
}  // namespace game
