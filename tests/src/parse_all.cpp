//#include <catch2/catch.hpp>
//#include <execution>
//#include <filesystem>
//#include <mutex>
//#include <osu_reader/beatmap.h>
//#include <iostream>
//
//TEST_CASE("parse all beatmaps")
//{
//	std::vector<std::filesystem::path> beatmap_files;
//	const auto files = std::filesystem::recursive_directory_iterator{ "D:/games/osu!/Songs" };
//
//	UNSCOPED_INFO("Gathering all .osu paths...");
//	std::for_each(begin(files),end(files),
//		[&beatmap_files](const auto& file)
//	{
//		if (file.path().extension() != ".osu") return;
//		beatmap_files.push_back(file);
//	});
//
//	UNSCOPED_INFO("Parsing files...");
//	std::mutex mx;
//	std::for_each(std::execution::par_unseq, beatmap_files.cbegin(), beatmap_files.cend(),  
//		[&mx](const auto& file)
//	{
//		try {
//			const auto beatmap = osu::Beatmap_parser::parse(file);
//			if (beatmap) {
//			}
//			else if (file_size(file) >= 5) {
//				std::lock_guard<std::mutex> l(mx);
//				WARN("Failed to parse " << file);
//				std::cout << file << '\n';
//			}
//		}
//		catch (const std::exception& e) {
//			std::lock_guard<std::mutex> l(mx);
//			WARN(e.what());
//		}
//	});	
//}
