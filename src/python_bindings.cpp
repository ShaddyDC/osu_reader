#include "osu_reader/beatmap.h"
#include "osu_reader/beatmap_parser.h"
#include "osu_reader/replay.h"
#include "osu_reader/replay_reader.h"
#include <pybind11/chrono.h>
#include <pybind11/stl.h>

namespace py = pybind11;

static void beatmap_bindings(py::module& m)
{
    py::class_<osu::Vector2>(m, "Vector2")
            .def_readwrite("x", &osu::Vector2::x)
            .def_readwrite("y", &osu::Vector2::y)
            .def("__repr__",
                 [](const osu::Vector2& p) {
                     return "<pyshosu.Vector2(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")'>";
                 });

    py::class_<osu::Hitcircle>(m, "Hitcircle")
            .def_readwrite("time", &osu::Hitcircle::time)
            .def_readwrite("pos", &osu::Hitcircle::pos)
            .def("__repr__",
                 [](const osu::Hitcircle& c) {
                     return "<pyshosu.Hitcircle(" + std::to_string(c.time.count()) + ", " + std::to_string(c.pos.x) + ", " + std::to_string(c.pos.y) + ")'>";
                 });

    py::enum_<osu::Slider::Slider_type>(m, "Slider_type")
            .value("linear", osu::Slider::Slider_type::linear)
            .value("perfect", osu::Slider::Slider_type::perfect)
            .value("bezier", osu::Slider::Slider_type::bezier)
            .value("catmull", osu::Slider::Slider_type::catmull);

    py::class_<osu::Slider::Segment>(m, "Slider.Segment")
            .def_readwrite("type", &osu::Slider::Segment::type)
            .def_readwrite("points", &osu::Slider::Segment::points);

    py::class_<osu::Slider>(m, "Slider")
            .def_readwrite("time", &osu::Slider::time)
            .def_readwrite("duration", &osu::Slider::duration)
            .def_readwrite("type", &osu::Slider::type)
            .def_readwrite("segments", &osu::Slider::segments)
            .def_readwrite("points", &osu::Slider::points)
            .def_readwrite("distances", &osu::Slider::distances)
            .def_readwrite("repeat", &osu::Slider::repeat)
            .def_readwrite("length", &osu::Slider::length)
            .def("__repr__",
                 [](const osu::Slider& s) {
                     return "<pyshosu.Slider(" + std::string{static_cast<char>(s.type)} + ", " +
                            std::to_string(s.time.count()) + ", " + std::to_string(s.length) + ")'>";
                 });

    py::class_<osu::Spinner>(m, "Spinner")
            .def_readwrite("start", &osu::Spinner::start)
            .def_readwrite("end", &osu::Spinner::end)
            .def("__repr__",
                 [](const osu::Spinner& s) {
                     return "<pyshosu.Spinner(" + std::to_string(s.start.count()) + ", " + std::to_string(s.end.count()) + ")'>";
                 });

    py::class_<osu::Beatmap>(m, "Beatmap")
            .def_readwrite("version", &osu::Beatmap::version)
            .def_readwrite("audio_file", &osu::Beatmap::audio_file)
            .def_readwrite("audio_lead_in", &osu::Beatmap::audio_lead_in)
            .def_readwrite("preview_time", &osu::Beatmap::preview_time)
            .def_readwrite("countdown", &osu::Beatmap::countdown)
            .def_readwrite("sample_set", &osu::Beatmap::sample_set)
            .def_readwrite("stack_leniency", &osu::Beatmap::stack_leniency)
            .def_readwrite("mode", &osu::Beatmap::mode)
            .def_readwrite("letterbox_in_breaks", &osu::Beatmap::letterbox_in_breaks)
            .def_readwrite("story_fire_in_front", &osu::Beatmap::story_fire_in_front)
            .def_readwrite("skin_preference", &osu::Beatmap::skin_preference)
            .def_readwrite("epilepsy_warning", &osu::Beatmap::epilepsy_warning)
            .def_readwrite("countdown_offset", &osu::Beatmap::countdown_offset)
            .def_readwrite("widescreen_storyboard", &osu::Beatmap::widescreen_storyboard)
            .def_readwrite("special_style", &osu::Beatmap::special_style)
            .def_readwrite("use_skin_sprites", &osu::Beatmap::use_skin_sprites)
            .def_readwrite("bookmarks", &osu::Beatmap::bookmarks)
            .def_readwrite("distance_spacing", &osu::Beatmap::distance_spacing)
            .def_readwrite("beat_divisor", &osu::Beatmap::beat_divisor)
            .def_readwrite("grid_size", &osu::Beatmap::grid_size)
            .def_readwrite("timeline_zoom", &osu::Beatmap::timeline_zoom)
            .def_readwrite("title", &osu::Beatmap::title)
            .def_readwrite("title_unicode", &osu::Beatmap::title_unicode)
            .def_readwrite("artist", &osu::Beatmap::artist)
            .def_readwrite("artist_unicode", &osu::Beatmap::artist_unicode)
            .def_readwrite("creator", &osu::Beatmap::creator)
            .def_readwrite("difficulty_name", &osu::Beatmap::difficulty_name)
            .def_readwrite("source", &osu::Beatmap::source)
            .def_readwrite("tags", &osu::Beatmap::tags)
            .def_readwrite("beatmap_id", &osu::Beatmap::beatmap_id)
            .def_readwrite("beatmap_set_id", &osu::Beatmap::beatmap_set_id)
            .def_readwrite("hp", &osu::Beatmap::hp)
            .def_readwrite("cs", &osu::Beatmap::cs)
            .def_readwrite("od", &osu::Beatmap::od)
            .def_readwrite("ar", &osu::Beatmap::ar)
            .def_readwrite("slider_multiplier", &osu::Beatmap::slider_multiplier)
            .def_readwrite("slider_tick_rate", &osu::Beatmap::slider_tick_rate)
            .def_readwrite("background", &osu::Beatmap::background)
            .def_readwrite("breaks", &osu::Beatmap::breaks)
            .def_readwrite("timingpoints", &osu::Beatmap::timingpoints)
            .def_readwrite("circles", &osu::Beatmap::circles)
            .def_readwrite("sliders", &osu::Beatmap::sliders)
            .def_readwrite("spinners", &osu::Beatmap::spinners)
            .def("__repr__",
                 [](const osu::Beatmap& b) {
                     return "<pyshosu.Beatmap '" + b.title + " (" + b.creator + ") [" + b.difficulty_name + "]'>";
                 });

    py::class_<osu::Beatmap::Timingpoint>(m, "Timingpoint")
            .def_readwrite("time", &osu::Beatmap::Timingpoint::time)
            .def_readwrite("beat_duration", &osu::Beatmap::Timingpoint::beat_duration)
            .def_readwrite("meter", &osu::Beatmap::Timingpoint::meter)
            .def_readwrite("sample_set", &osu::Beatmap::Timingpoint::sample_set)
            .def_readwrite("sample_index", &osu::Beatmap::Timingpoint::sample_index)
            .def_readwrite("sample_volume", &osu::Beatmap::Timingpoint::sample_volume)
            .def_readwrite("uninherited", &osu::Beatmap::Timingpoint::uninherited)
            .def_readwrite("kiai", &osu::Beatmap::Timingpoint::kiai)
            .def("__repr__",
                 [](const osu::Beatmap::Timingpoint& t) {
                     return "<pyshosu.Beatmap.Timingpoint '" + std::to_string(t.time.count()) + " - " + std::to_string(t.beat_duration.count()) + "'>";
                 });

    py::class_<osu::Beatmap_parser>(m, "Beatmap_parser")
            .def(py::init())
            .def_readwrite("slider_paths", &osu::Beatmap_parser::slider_paths)
            .def("from_string", &osu::Beatmap_parser::from_string)
            .def("from_file", &osu::Beatmap_parser::from_file);
}

static void replay_bindings(py::module& m)
{
    py::class_<osu::Replay::Replay_frame>(m, "Replay.Frame")
            .def_readwrite("time", &osu::Replay::Replay_frame::time)
            .def_readwrite("x", &osu::Replay::Replay_frame::x)
            .def_readwrite("y", &osu::Replay::Replay_frame::y)
            .def_readwrite("state", &osu::Replay::Replay_frame::state)
            .def("__repr__",
                 [](const osu::Replay::Replay_frame& f) {
                     return "<pyshosu.Replay.frame(" + std::to_string(f.time.count()) + ", " + std::to_string(f.x) + ", " + std::to_string(f.y) + ", " + std::to_string(f.state) + ")'>";
                 });

    py::enum_<osu::Mods>(m, "Mods")
            .value("Empty", osu::Mods::None)
            .value("NoFail", osu::Mods::NoFail)
            .value("Easy", osu::Mods::Easy)
            .value("TouchDevice", osu::Mods::TouchDevice)
            .value("Hidden", osu::Mods::Hidden)
            .value("HardRock", osu::Mods::HardRock)
            .value("SuddenDeath", osu::Mods::SuddenDeath)
            .value("DoubleTime", osu::Mods::DoubleTime)
            .value("Relax", osu::Mods::Relax)
            .value("HalfTime", osu::Mods::HalfTime)
            .value("Nightcore", osu::Mods::Nightcore)
            .value("Flashlight", osu::Mods::Flashlight)
            .value("Autoplay", osu::Mods::Autoplay)
            .value("SpunOut", osu::Mods::SpunOut)
            .value("Relax2", osu::Mods::Relax2)
            .value("Perfect", osu::Mods::Perfect)
            .value("Key4", osu::Mods::Key4)
            .value("Key5", osu::Mods::Key5)
            .value("Key6", osu::Mods::Key6)
            .value("Key7", osu::Mods::Key7)
            .value("Key8", osu::Mods::Key8)
            .value("keyMod", osu::Mods::keyMod)
            .value("FadeIn", osu::Mods::FadeIn)
            .value("Random", osu::Mods::Random)
            .value("LastMod", osu::Mods::LastMod)
            .value("TargetPractice", osu::Mods::TargetPractice)
            .value("Key9", osu::Mods::Key9)
            .value("Coop", osu::Mods::Coop)
            .value("Key1", osu::Mods::Key1)
            .value("Key3", osu::Mods::Key3)
            .value("Key2", osu::Mods::Key2)
            .value("ScoreV2", osu::Mods::ScoreV2)
            .value("Mirror", osu::Mods::Mirror);

    py::class_<osu::Replay>(m, "Replay")
            .def_readwrite("mode", &osu::Replay::mode)
            .def_readwrite("game_version", &osu::Replay::game_version)
            .def_readwrite("map_hash", &osu::Replay::map_hash)
            .def_readwrite("player_name", &osu::Replay::player_name)
            .def_readwrite("replay_hash", &osu::Replay::replay_hash)
            .def_readwrite("count_300", &osu::Replay::count_300)
            .def_readwrite("count_100", &osu::Replay::count_100)
            .def_readwrite("count_50", &osu::Replay::count_50)
            .def_readwrite("count_geki", &osu::Replay::count_geki)
            .def_readwrite("count_katsu", &osu::Replay::count_katsu)
            .def_readwrite("count_miss", &osu::Replay::count_miss)
            .def_readwrite("score", &osu::Replay::score)
            .def_readwrite("max_combo", &osu::Replay::max_combo)
            .def_readwrite("score", &osu::Replay::score)
            .def_readwrite("max_combo", &osu::Replay::max_combo)
            .def_readwrite("full_combo", &osu::Replay::full_combo)
            .def_readwrite("mods", &osu::Replay::mods)
            .def_readwrite("life_bar", &osu::Replay::life_bar)
            .def_readwrite("time_stamp", &osu::Replay::time_stamp)
            .def_readwrite("replay_compressed", &osu::Replay::replay_compressed)
            .def_readwrite("frames", &osu::Replay::frames)
            .def_readwrite("score_id", &osu::Replay::score_id)
            .def("__repr__",
                 [](const osu::Replay& r) {
                     return "<pyshosu.Replay '" + r.map_hash + " - " + r.player_name + "'>";
                 });

    py::class_<osu::Replay_reader>(m, "Replay_reader")
            .def(py::init())
            .def_readwrite("parse_frames", &osu::Replay_reader::parse_frames)
            .def("from_string", &osu::Replay_reader::from_string)
            .def("from_file", &osu::Replay_reader::from_file);
}

PYBIND11_MODULE(pyshosu, m)
{
    m.doc() = "Library for doing some osu things";

    // Allow calling functions taking a path with string
    py::class_<std::filesystem::path>(m, "Path")
            .def(py::init<std::string>())
            .def("__repr__",
                 [](const std::filesystem::path& p) {
                     return "<std.filesystem.Path '" + p.string() + "'>";
                 });

    py::implicitly_convertible<std::string, std::filesystem::path>();

    py::enum_<osu::Gamemode>(m, "Gamemode")
            .value("osu", osu::Gamemode::osu)
            .value("taiko", osu::Gamemode::taiko)
            .value("ctb", osu::Gamemode::ctb)
            .value("mania", osu::Gamemode::mania);

    beatmap_bindings(m);
    replay_bindings(m);
}
