#pragma once

#include "beatmap.h"

namespace osu {
    template<typename Callback>
    class Hitobject_iterator {
    public:
        inline Hitobject_iterator(const Beatmap& bm, Callback callback)
            : circle_it{bm.circles.cbegin()}, circle_end{bm.circles.cend()},
              slider_it{bm.sliders.cbegin()}, slider_end{bm.sliders.cend()},
              spinner_it{bm.spinners.cbegin()}, spinner_end{bm.spinners.cend()},
              callback(callback)
        {}

        inline void all()
        {
            while(next()) {}
        }

        inline bool next()
        {
            if(circle_it != circle_end && (slider_it == slider_end || circle_it->time <= slider_it->time) && (spinner_it == spinner_end || circle_it->time <= spinner_it->start)) {
                callback(*circle_it++);
                return true;
            }
            if(slider_it != slider_end && (spinner_it == spinner_end || slider_it->time <= spinner_it->start)) {
                callback(*slider_it++);
                return true;
            }
            if(spinner_it != spinner_end) {
                callback(*spinner_it++);
                return true;
            }
            return false;
        }

    private:
        std::vector<Hitcircle>::const_iterator circle_it, circle_end;
        std::vector<Slider>::const_iterator slider_it, slider_end;
        std::vector<Spinner>::const_iterator spinner_it, spinner_end;
        Callback callback;
    };

}// namespace osu
