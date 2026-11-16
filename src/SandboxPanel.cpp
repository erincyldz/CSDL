#include "SandboxPanel.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <imgui.h>

namespace game::ui
{
using object::GameObject;

namespace
{
bool dragDouble(const char* label, double* v, float speed, double min, double max,
                const char* fmt = "%.2f", ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp)
{
    return ImGui::DragScalar(label, ImGuiDataType_Double, v, speed, &min, &max, fmt, flags);
}

bool sliderDouble(const char* label, double* v, double min, double max, const char* fmt = "%.2f",
                  ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp)
{
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &min, &max, fmt, flags);
}

bool dragVector(const char* label, math::Vector2D& v, float speed)
{
    double xy[2] = {v.getX(), v.getY()};
    if (!ImGui::DragScalarN(label, ImGuiDataType_Double, xy, 2, speed, nullptr, nullptr, "%.1f"))
        return false;
    v = math::Vector2D{xy[0], xy[1]};
    return true;
}

bool colorEdit(const char* label, object::Color& c)
{
    float rgb[3] = {c.r / 255.0f, c.g / 255.0f, c.b / 255.0f};
    if (!ImGui::ColorEdit3(label, rgb, ImGuiColorEditFlags_NoInputs))
        return false;
    auto to8 = [](float f)
    {
        return static_cast<std::uint8_t>(std::lround(std::clamp(f, 0.0f, 1.0f) * 255.0f));
    };
    c = object::Color{to8(rgb[0]), to8(rgb[1]), to8(rgb[2]), c.a};
    return true;
}

void helpMarker(const char* text)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void hint(const char* text)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();
}

const char* shapeName(const object::Shape& s)
{
    return std::holds_alternative<object::CircleShape>(s) ? "circle" : "rect";
}
}  // namespace

PanelActions SandboxPanel::draw(PanelContext& ctx)
{
    PanelActions actions;
    const ImGuiIO& io = ImGui::GetIO();
    // Docked to the right edge, full height; the width can be changed by dragging the left edge.
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(m_width, io.DisplaySize.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(std::min(240.0f * m_scale, io.DisplaySize.x * 0.5f), io.DisplaySize.y),
        ImVec2(std::max(240.0f * m_scale, io.DisplaySize.x * 0.6f), io.DisplaySize.y));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Sandbox  (F1 hides)", nullptr, flags))
    {
        m_width = ImGui::GetWindowWidth();
        ImGui::PushItemWidth(-ImGui::GetFontSize() * 9.0f);
        drawSimulationControls(ctx, actions);
        drawTools(ctx);
        drawInspector(ctx, actions);
        drawSpawnTemplate(ctx);
        drawObjectList(ctx);
        drawScene(ctx, actions);
        drawPhysics(ctx, actions);
        drawRendering(ctx, actions);
        drawStats(ctx);
        ImGui::PopItemWidth();
    }
    ImGui::End();
    return actions;
}

void SandboxPanel::drawSimulationControls(PanelContext& ctx, PanelActions& actions)
{
    if (ImGui::Button(ctx.paused ? "Resume (P)" : "Pause (P)"))
        actions.togglePause = true;
    ImGui::SameLine();
    ImGui::BeginDisabled(!ctx.paused);
    if (ImGui::Button("Step (N)"))
        actions.step = true;
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Reset (R)"))
        actions.reset = true;
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        actions.clear = true;

    if (sliderDouble("Time scale", &ctx.config.timeScale, 0.05, 8.0, "%.2fx",
                     ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp))
        actions.timingChanged = true;
    ImGui::SameLine();
    if (ImGui::SmallButton("1x"))
    {
        ctx.config.timeScale = 1.0;
        actions.timingChanged = true;
    }
    ImGui::Separator();
}

void SandboxPanel::drawTools(PanelContext& ctx)
{
    auto& sb = ctx.sandbox;
    if (!ImGui::CollapsingHeader("Mouse tool", ImGuiTreeNodeFlags_DefaultOpen))
        return;
    if (ImGui::RadioButton("Select / drag", sb.tool == sandbox::Tool::Select))
        sb.tool = sandbox::Tool::Select;
    ImGui::SameLine();
    if (ImGui::RadioButton("Spawn", sb.tool == sandbox::Tool::Spawn))
        sb.tool = sandbox::Tool::Spawn;
    if (sb.tool == sandbox::Tool::Select)
    {
        ImGui::Checkbox("Throw on release", &sb.throwOnRelease);
        hint("Left click: select. Left drag: move / throw. Right click: delete. "
             "DEL: delete selected.");
    }
    else
    {
        hint("Left click: place at rest. Left drag: place and aim (drag = velocity). "
             "Right click: delete.");
    }
}

void SandboxPanel::drawSpawnTemplate(PanelContext& ctx)
{
    auto& t = ctx.sandbox.spawn;
    const ImGuiTreeNodeFlags flags =
        ctx.sandbox.tool == sandbox::Tool::Spawn ? ImGuiTreeNodeFlags_DefaultOpen : 0;
    if (!ImGui::CollapsingHeader("Spawn template", flags))
        return;

    if (ImGui::RadioButton("Circle", t.circle))
        t.circle = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rectangle", !t.circle))
        t.circle = false;

    if (t.circle)
        dragDouble("Radius", &t.radius, 0.5f, 1.0, 500.0, "%.1f");
    else
    {
        dragDouble("Width", &t.width, 0.5f, 1.0, 2000.0, "%.1f");
        dragDouble("Height", &t.height, 0.5f, 1.0, 2000.0, "%.1f");
    }

    ImGui::Checkbox("Mass = area", &t.massFromArea);
    if (t.massFromArea)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(%.1f)", object::area(t.shape()));
    }
    else
        dragDouble("Mass", &t.mass, 1.0f, 0.001, 1e9, "%.3f",
                   ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
    sliderDouble("Restitution", &t.restitution, 0.0, 1.0);
    ImGui::Checkbox("Stable (immovable)", &t.stable);
    ImGui::SameLine();
    ImGui::Checkbox("Random color", &t.randomColor);
    if (!t.randomColor)
        colorEdit("Color", t.color);
    dragDouble("Aim factor", &t.velocityPerPixel, 0.05f, 0.0, 20.0);
    helpMarker("Spawn tool: the initial speed (px/s) is the drag length (px) times this factor.");
}

void SandboxPanel::drawInspector(PanelContext& ctx, PanelActions& actions)
{
    if (!ImGui::CollapsingHeader("Selected object", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    GameObject* obj = ctx.sandbox.selected() ? ctx.world.find(*ctx.sandbox.selected()) : nullptr;
    if (!obj)
    {
        hint("Nothing selected. Click an object with the Select tool, or pick one in the "
             "Objects list.");
        return;
    }

    ImGui::Text("#%u  %s  (%s)", static_cast<unsigned>(obj->getId()), obj->getName().c_str(),
                shapeName(obj->getShape()));

    // Shape
    object::Shape shape = obj->getShape();
    bool shapeChanged = false;
    if (auto* c = std::get_if<object::CircleShape>(&shape))
        shapeChanged = dragDouble("Radius##sel", &c->radius, 0.5f, 1.0, 1000.0, "%.1f");
    else if (auto* r = std::get_if<object::RectShape>(&shape))
    {
        shapeChanged |= dragDouble("Width##sel", &r->width, 0.5f, 1.0, 4000.0, "%.1f");
        shapeChanged |= dragDouble("Height##sel", &r->height, 0.5f, 1.0, 4000.0, "%.1f");
    }
    if (shapeChanged)
        obj->setShape(shape);

    // Kinematics
    math::Vector2D pos = obj->getPosition();
    if (dragVector("Position", pos, 1.0f))
        obj->setPosition(pos);
    math::Vector2D vel = obj->getVelocity();
    ImGui::BeginDisabled(obj->isStable());
    if (dragVector("Velocity", vel, 1.0f))
        obj->setVelocity(vel);
    ImGui::EndDisabled();
    ImGui::TextDisabled("speed %.2f px/s", obj->getVelocity().magnitude());

    // Mass properties
    double mass = obj->getMass();
    if (dragDouble("Mass##sel", &mass, 1.0f, 0.001, 1e9, "%.3f",
                   ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp))
        obj->setMass(mass);
    ImGui::SameLine();
    if (ImGui::SmallButton("= area"))
        obj->setMass(object::area(obj->getShape()));
    double e = obj->getRestitution();
    if (sliderDouble("Restitution##sel", &e, 0.0, 1.0))
        obj->setRestitution(e);
    bool stable = obj->isStable();
    if (ImGui::Checkbox("Stable", &stable))
        obj->setStable(stable);
    ImGui::SameLine();
    object::Color color = obj->getColor();
    if (colorEdit("Color##sel", color))
        obj->setColor(color);

    if (ImGui::Button("Stop"))
        obj->setVelocity({});
    ImGui::SameLine();
    if (ImGui::Button("Clear trail"))
        obj->clearTrail();
    ImGui::SameLine();
    if (ImGui::Button("Duplicate"))
        actions.duplicateSelected = true;
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
        actions.deleteSelected = true;
}

void SandboxPanel::drawObjectList(PanelContext& ctx)
{
    char header[64];
    std::snprintf(header, sizeof(header), "Objects (%d)###objects",
                  static_cast<int>(ctx.world.size()));
    if (!ImGui::CollapsingHeader(header))
        return;

    const ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable;
    const float height = ImGui::GetTextLineHeightWithSpacing() * 10.0f;
    if (!ImGui::BeginTable("objects_table", 5, flags, ImVec2(0.0f, height)))
        return;
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("#");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Shape");
    ImGui::TableSetupColumn("Mass");
    ImGui::TableSetupColumn("Speed");
    ImGui::TableHeadersRow();

    const auto& objects = ctx.world.objects();
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(objects.size()));
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            const GameObject& o = objects[static_cast<std::size_t>(i)];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            char id[32];
            std::snprintf(id, sizeof(id), "%u", static_cast<unsigned>(o.getId()));
            const bool selected = ctx.sandbox.selected() == o.getId();
            if (ImGui::Selectable(id, selected, ImGuiSelectableFlags_SpanAllColumns))
                ctx.sandbox.select(o.getId());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(o.getName().c_str());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(o.isStable() ? "stable" : shapeName(o.getShape()));
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", o.getMass());
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", o.getVelocity().magnitude());
        }
    }
    ImGui::EndTable();
}

void SandboxPanel::drawScene(PanelContext& ctx, PanelActions& actions)
{
    if (!ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    const auto preset = static_cast<scenario::Preset>(m_presetIndex);
    if (ImGui::BeginCombo("Preset", scenario::presetName(preset)))
    {
        for (int i = 0; i < scenario::PRESET_COUNT; ++i)
        {
            const auto p = static_cast<scenario::Preset>(i);
            if (ImGui::Selectable(scenario::presetName(p), i == m_presetIndex))
                m_presetIndex = i;
            if (ImGui::IsItemHovered())
                ImGui::SetItemTooltip("%s", scenario::presetDescription(p));
        }
        ImGui::EndCombo();
    }
    ImGui::TextWrapped("%s", scenario::presetDescription(preset));
    if (preset == scenario::Preset::Random)
        ImGui::SliderInt("Count", &m_randomCount, 1, std::max(1, ctx.config.maxObjects));
    if (ImGui::Button("Load preset"))
        actions.loadPreset = preset;

    ImGui::Separator();
    ImGui::InputText("File", m_scenePath, sizeof(m_scenePath));
    if (ImGui::Button("Save scene"))
        actions.saveScene = std::string(m_scenePath);
    ImGui::SameLine();
    if (ImGui::Button("Load scene"))
        actions.loadScene = std::string(m_scenePath);
    helpMarker("Plain text, one object per line. Relative paths are relative to the directory "
               "the program was started from. R reloads the last loaded scene or preset.");

    if (!m_status.empty())
    {
        const ImVec4 color =
            m_statusIsError ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f) : ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextWrapped("%s", m_status.c_str());
        ImGui::PopStyleColor();
    }
}

void SandboxPanel::drawPhysics(PanelContext& ctx, PanelActions& actions)
{
    if (!ImGui::CollapsingHeader("Physics"))
        return;
    auto& c = ctx.config;
    bool changed = false;
    changed |= ImGui::Checkbox("Gravity", &c.gravity);
    changed |= dragDouble("G", &c.gravitationalConstant, 1.0f, 0.0, 1e6, "%.2f",
                          ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
    changed |= dragDouble("Softening", &c.softening, 0.1f, 0.0, 200.0, "%.1f px");
    helpMarker("Prevents the gravitational force from exploding when two bodies get very close.");
    changed |= ImGui::Checkbox("Bounce off borders", &c.borderCollision);
    helpMarker("Off: objects wrap around the window edges.");
    changed |= dragDouble("Max speed", &c.maxSpeed, 5.0f, 0.0, 1e5, "%.0f px/s");
    helpMarker("0 disables the limit.");
    changed |=
        dragDouble("Inelastic below", &c.restitutionThreshold, 0.5f, 0.0, 500.0, "%.1f px/s");
    helpMarker("Collisions slower than this are treated as perfectly inelastic, so resting "
               "objects stop bouncing.");
    if (ImGui::SliderInt("Physics Hz", &c.physicsHz, 10, 1000))
        actions.timingChanged = true;
    actions.physicsChanged |= changed;
}

void SandboxPanel::drawRendering(PanelContext& ctx, PanelActions& actions)
{
    if (!ImGui::CollapsingHeader("Rendering & audio"))
        return;
    auto& c = ctx.config;
    ImGui::Checkbox("Trails", &c.renderTrails);
    ImGui::SameLine();
    ImGui::Checkbox("Velocity arrows", &c.renderDirection);
    ImGui::Checkbox("Collision outlines", &c.renderCollisionHighlights);
    ImGui::SameLine();
    actions.physicsChanged |= ImGui::Checkbox("Color cycle", &c.colorCycle);
    actions.audioChanged |= ImGui::Checkbox("Sound", &c.sound);
    actions.audioChanged |= ImGui::SliderInt("Volume", &c.volume, 0, 100);
}

void SandboxPanel::drawStats(PanelContext& ctx)
{
    if (!ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
        return;
    const auto e = ctx.world.computeEnergy();
    ImGui::Text("Objects %d   Collisions %llu   FPS %.0f", static_cast<int>(ctx.world.size()),
                static_cast<unsigned long long>(ctx.world.totalCollisions()), ctx.fps);
    ImGui::Text("Kinetic   %14.2f", e.kinetic);
    ImGui::Text("Potential %14.2f", e.potential);
    ImGui::Text("Total     %14.2f", e.total());
    ImGui::Text("Momentum  (%.2f, %.2f)  |p| %.2f", e.momentum.getX(), e.momentum.getY(),
                e.momentum.magnitude());
    helpMarker("Energy and momentum of movable objects. Stable objects can absorb momentum, so "
               "only isolated systems (no stable objects, restitution 1, borders off) conserve "
               "both exactly.");

    if (m_energyCount > 1)
    {
        const bool full = m_energyCount == HISTORY;
        float lo = m_energy[0], hi = m_energy[0];
        for (int i = 0; i < m_energyCount; ++i)
        {
            lo = std::min(lo, m_energy[static_cast<std::size_t>(i)]);
            hi = std::max(hi, m_energy[static_cast<std::size_t>(i)]);
        }
        char overlay[96];
        std::snprintf(overlay, sizeof(overlay), "total energy  [%.1f .. %.1f]", lo, hi);
        // Keep a small visible range so a perfectly flat line is drawn in the middle.
        const float pad = std::max(1e-3f, (hi - lo) * 0.1f);
        ImGui::PlotLines("##energy", m_energy.data(), m_energyCount, full ? m_energyOffset : 0,
                         overlay, lo - pad, hi + pad, ImVec2(-1.0f, 80.0f * m_scale));
    }
    if (ImGui::SmallButton("Reset graph"))
        clearEnergyHistory();
}

void SandboxPanel::recordEnergy(double totalEnergy)
{
    m_energy[static_cast<std::size_t>(m_energyOffset)] = static_cast<float>(totalEnergy);
    m_energyOffset = (m_energyOffset + 1) % HISTORY;
    m_energyCount = std::min(m_energyCount + 1, HISTORY);
}

void SandboxPanel::clearEnergyHistory()
{
    m_energyOffset = 0;
    m_energyCount = 0;
}

void SandboxPanel::setStatus(std::string message, bool isError)
{
    m_status = std::move(message);
    m_statusIsError = isError;
}
}  // namespace game::ui
