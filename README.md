# osu_reader

![C/C++ CI](https://github.com/ShaddyDC/osu_reader/workflows/C/C++%20CI/badge.svg)

A c++17 library for consuming osu files with alrightish testing for the positive cases.

## Usage

```cpp
#include <osu_reader/beatmap.h>
const auto bm_optional = osu::Beatmap::from_file("path/to/beatmap.osu");
// Alternatively: osu::Beatmap::from_string(beatmap_string);
if(!bm_optional) handler_for_beatmap_not_found_or_however_you_deal_with_this_case();
else{
    const auto beatmap = *bm_optional;
    print(beatmap.title);
}
```

```cpp
#include <osu_reader/replay.h>
// .value() throws on access if loading failed
const auto replay = osu::Replay::from_file("path/to/replay.osr", true).value();
print(replay.player_name);
```

Failing to parse files, i.e. the file couldn't be opened or the beatmap version couldn't be read, are handled by returning an empty optional. This also applies for malformed replay files. For beatmaps, other errors such as being unable to read a specific value are ignored altogether and the value will be in its default state.

## Installation

Recommended usage is to add the library as a submodule or clone it into a subdirectory of your project and to consume it with cmake by calling `add_subdirectory(osu_reader)`. You can then add it to your application with `target_link_libraries(yourApplication PRIVATE osuReader::osuReader)`.  
Maybe it will be added to other package managers in the future if it is more mature.  
**Note that the xz submodule is required to compile the code.** This is necessary to support replay frame extraction. This requirement may become optional in the future. 
You can recursively download submodules with `git submodule update --init --recursive`

Running the tests requires [Catch2](https://github.com/catchorg/Catch2/). Installing it via [vcpkg](https://github.com/Microsoft/vcpkg/) is supported, although it is now also shipped with the project as a submodule.

## Python Bindings

This library supports usage in python.
Simply install it by running `python setup.py install` in the project main directory.
Note that it requires the pybind11 and xz submodules by default.
You can run its tests with `python setup.py test`.

```python
import pyshosu

bm = pyshosu.beatmap_file("path/to/replay.osr")
print(bm.title)

r = pyshosu.replay_file("path/to/beatmap.osu", True)
print(r.player_name)
```

## Completeness

### Implemented

- `.osu` format (Mostly, missing colours and hit extras)
- `.osr` format (Not thoroughly tested yet)
- Slider curve computation (wip - not exposed yet)

### Planned

- `osu!.db` (maybe)
- Matching replay clicks to hitobjects etc
- Stack notes

If there's anything missing you need, open an issue and I'll push it on the priority list. 

## Licence

MIT
