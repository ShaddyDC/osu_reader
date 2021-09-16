import unittest

import pyshosu


class BeatmapTest(unittest.TestCase):
    def test_test(self):
        file = "tests/res/A.SAKA - Nanatsu Koyoto (ailv) [Extra].osu"

        with open(file, 'r') as f:
            content = f.read()

        parser = pyshosu.Beatmap_parser()
        parser.slider_paths = True

        for bm in [parser.from_file(file), parser.from_string(content)]:
            self.assertEqual(bm.version, 14)
            self.assertEqual(bm.countdown, 1)
            self.assertEqual(bm.sample_set, "Soft")
            self.assertAlmostEqual(bm.stack_leniency, 0.7)

            self.assertEqual(bm.title, "Nanatsu Koyoto")

            self.assertTrue(bm.sliders[0].points)


class ReplayTest(unittest.TestCase):
    def test_test(self):
        file = "tests/res/cptnXn - xi - FREEDOM DiVE [FOUR DIMENSIONS] (2014-05-11) Osu.osr"

        with open(file, 'rb') as f:
            content = f.read()

        reader = pyshosu.Replay_reader()
        reader.parse_frames = True

        for r in [reader.from_file(file), reader.from_string(content)]:
            self.assertEqual(r.mode, pyshosu.Gamemode.osu)
            self.assertEqual(r.game_version, 20181231)
            self.assertEqual(r.map_hash, "da8aae79c8f3306b5d65ec951874a7fb")
            self.assertEqual(r.player_name, "cptnXn")
            self.assertEqual(r.replay_hash, "391edbb7774bfa13bd252d5b92c72637")
            self.assertEqual(r.count_300, 1960)
            self.assertEqual(r.count_100, 23)
            self.assertEqual(r.count_50, 0)
            self.assertEqual(r.count_geki, 240)
            self.assertEqual(r.count_katsu, 11)
            self.assertEqual(r.count_miss, 0)
            self.assertEqual(r.score, 116845640)
            self.assertEqual(r.max_combo, 2384)
            self.assertEqual(r.full_combo, False)
            self.assertEqual(r.mods, 0)
            self.assertEqual(r.score_id, 1740197996)
            self.assertEqual(len(r.frames), 21188)

            self.assertEqual(r.frames[1].x, 256);
            self.assertEqual(r.frames[1].y, -500);
            self.assertEqual(r.frames[1].state, 0);


if __name__ == '__main__':
    unittest.main()
