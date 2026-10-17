"""Exercise false-success cases in the FUNCTION-claim regression guard."""

import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from check_matches import check_report


class MatchGuardTests(unittest.TestCase):
    def report(self, **overrides):
        row = {"address": "0x45de00", "recomp": "0x401060", "name": "IsHorizontalSpace", "matching": 1.0}
        row.update(overrides)
        return {"format": 1, "data": [row]}

    def test_exact_match_passes(self):
        self.assertEqual(check_report(self.report(), {0x45de00}), [])

    def test_reviewed_near_match_is_implemented(self):
        self.assertEqual(check_report(self.report(matching=0.99), {0x45de00}), [])
        self.assertEqual(check_report(self.report(matching=0.95), {0x45de00}), [])

    def test_exact_mode_preserves_raw_accuracy_requirement(self):
        self.assertTrue(check_report(self.report(matching=0.99, effective=True), {0x45de00}, require_exact=True))
        self.assertEqual(check_report(self.report(), {0x45de00}, require_exact=True), [])

    def test_effective_score_does_not_override_low_raw_score(self):
        self.assertTrue(check_report(self.report(matching=0.949, effective=True), {0x45de00}))

    def test_invalid_accuracy_fails(self):
        for accuracy in (None, True, False, "0.99", float("nan"), float("inf"), -0.1, 1.1):
            with self.subTest(accuracy=accuracy):
                self.assertTrue(check_report(self.report(matching=accuracy), {0x45de00}))

    def test_missing_or_unlinked_function_fails(self):
        self.assertTrue(check_report(self.report(), {0x45dda0, 0x45de00}))
        self.assertTrue(check_report(self.report(recomp=None), {0x45de00}))

    def test_stub_is_not_a_match(self):
        self.assertTrue(check_report(self.report(stub=True), {0x45de00}))

    def test_empty_claims_and_new_report_format_fail(self):
        with self.assertRaises(ValueError):
            check_report(self.report(), set())
        with self.assertRaises(ValueError):
            check_report({"format": 2, "data": []}, {0x45de00})


if __name__ == "__main__":
    unittest.main()
