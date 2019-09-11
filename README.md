# osu_reader
A c++17 library for consuming osu files with alrightish testing.  
Probably doesn't throw.

## Usage
```
#include <osu_reader/beatmap.h>
const auto bm_optional = osu::Beatmap_parser::parse("path/to/beatmap.osu");
if(!bm_optional) handler_for_beatmap_not_found_or_however_you_deal_with_this_case();
else{
    const auto beatmap = *bm_optional;
    print(beatmap.title);
}
```
Failing to read files altogether, i.e. the file couldn't be opened or the beatmap version couldn't be read, are handled by returning an empty optional. Other errors such as being unable to read a specific value are ignored altogether.

## Installation
Recommended usage is to add the library as a submodule or clone it into a subdirectory of your project and to consume it with cmake by calling `add_subdirectory(yourSubdirectory)`. You can then add it to your application with `target_link_libraries(yourApplication PRIVATE osuReader::osuReader)`.  
Maybe it will be added to other package managers in the future if it is more mature. 

Running the tests requires [Catch2](https://github.com/catchorg/Catch2/). Installing it via [vcpkg](https://github.com/Microsoft/vcpkg/) is supported.

## Completeness
### Implemented 
- `.osu` format (Mostly, missing colours and hit extras)
### Planned
- `.osr` format
- `osu!.db` 
- ???

If there's anything missing you need, open an issue and I'll push it on the priority list. 

## Licence
MIT