#pragma once

namespace osu {
    constexpr float ar_to_ms(const float ar)
    {
        if(ar >= 5) return 1950 - (ar * 150);
        return 1800 - (ar * 120);
    }

    constexpr float cs_to_osupixel(const float cs)
    {
        return (512.f / 16.f) * (1.f - 0.7f * (cs - 5.f) / 5.f);
    }

    constexpr float od_to_ms300(const float od)
    {
        return (159.f - 12.f * od) / 2.f;
    }

    constexpr float od_to_ms100(const float od)
    {
        return (279.f - 16.f * od) / 2.f;
    }

    constexpr float od_to_ms50(const float od)
    {
        return (399.f - 20.f * od) / 2.f;
    }
}// namespace osu
