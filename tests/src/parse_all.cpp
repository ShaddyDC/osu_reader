#include <catch2/catch.hpp>
#include <execution>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <osu_reader/beatmap.h>

TEST_CASE("parse all beatmaps")
{
	std::vector<std::filesystem::path> beatmap_files;
	const auto files = std::filesystem::recursive_directory_iterator{ "D:/games/osu!/Songs" };

	std::cout << "Loading all .osu paths...\n";
	std::for_each(begin(files),end(files),
		[&beatmap_files](const auto& file)
	{
		if (file.path().extension() != ".osu") return;
		beatmap_files.push_back(file);
	});

	std::cout << "Done!\n";
	std::cout << "Parsing files...\n";
	std::mutex mx;
	std::for_each(std::execution::par_unseq, beatmap_files.cbegin(), beatmap_files.cend(),  
		[&mx](const auto& file)
	{
		try {
			const auto beatmap = osu::Beatmap_parser::parse(file);
			if (beatmap) {
			}
			else if (file_size(file) >= 5) {
				std::lock_guard<std::mutex> l(mx);
				std::cout << file << "\n";
			}
		}
		catch (const std::exception& e) {
			std::lock_guard<std::mutex> l(mx);
			std::cout << "[EXCEPTION] " << e.what() << '\n';
		}
	});	
	std::cout << "Done!\n";
}