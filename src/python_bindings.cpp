#include <pybind11/pybind11.h>
#include "osu_reader/beatmap.h"
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

namespace py = pybind11;

static void beatmap_bindings(py::module& m)
{
    py::class_<osu::Beatmap>(m, "Beatmap")
            .def_readwrite("version", &osu::Beatmap::version)
            .def_readwrite("audio_file", &osu::Beatmap::audio_file)
            .def_readwrite("audio_lead_in", &osu::Beatmap::audio_lead_in)
            .def_readwrite("preview_time", &osu::Beatmap::preview_time)
            .def_readwrite("countdown", &osu::Beatmap::countdown)
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
              [](const osu::Beatmap& b){
                  return "<pyshosu.Beatmap '" + b.title + " (" + b.creator + ") [" + b.difficulty_name + "]'>";
              });
    constexpr const auto beatmp_from_file = [](const std::string& file){ return osu::Beatmap::from_file(file); };
    m.def("beatmap_file", beatmp_from_file, "Load replay from file");
    m.def("beatmap_string", &osu::Beatmap::from_string, "Load replay from string");
}

PYBIND11_MODULE(pyshosu, m){
    m.doc() = "Library for doing some osu things";

    beatmap_bindings(m);


}
