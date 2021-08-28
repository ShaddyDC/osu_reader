//#include <catch2/catch.hpp>
////#include <execution>
//#include <filesystem>
//#include <iostream>
//#include <mutex>
//#include <osu_reader/beatmap.h>
//
//TEST_CASE("parse all beatmaps")
//{
//    const auto files = std::filesystem::recursive_directory_iterator{"/run/media/space/ext4/games/osu/drive_c/osu/Songs/"};
//
//    std::cout << "Gathering all .osu paths...\n";
//    auto i = 0;
//    std::for_each(begin(files), end(files),
//                  [&i](const auto& file) {
//                      if(file.path().extension() != ".osu") return;
//                      ++i;
//
//                      std::cout << '\r' << i << ": parsing" << file << "...";
//
//                      try {
//                          const auto beatmap = osu::Beatmap::from_file(file);
//                          if(beatmap) {
//                          } else if(file_size(file) >= 5) {
//                              //				std::lock_guard<std::mutex> l(mx);
//                              std::cout << "\n\033[1;31m" << "Failed to parse " << file << "\033[0m\n";
//                          }
//                      } catch(const std::exception& e) {
//                          std::cout << "\n\033[1;31m" << "Error on " << file << ": " << e.what() << "\033[0m\n";
//                      }
//                  });
//}
