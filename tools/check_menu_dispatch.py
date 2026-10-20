#!/usr/bin/env python3
"""Verify menu widget callback pointers without executing the rebuilt game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import DEFAULT_BUILD_DIR, ROOT

TABLES = {
    "car": (0x00530640, 6, "g_menuCarPreviewSelectorDispatch"),
    "input_bindings": (0x0052fae4, 6, "g_menuInputBindingWidgetDispatch"),
    "text_entry": (0x00530400, 6, "g_menuTextEntryDispatch"),
    "character": (0x00530670, 6, "g_menuCharacterPreviewSelectorDispatch"),
    "network": (0x00530460, 6, "g_menuNetworkStatusDispatch"),
    "track": (0x0052fb44, 6, "g_menuTrackSelectorDispatch"),
    "base": (0x00530200, 6, "g_menuWidgetBaseDispatch"),
    "volume": (0x00530220, 6, "g_menuVolumeSliderDispatch"),
    "range": (0x00530240, 6, "g_menuRangeSliderDispatch"),
    "choice": (0x0053025c, 6, "g_menuChoiceSelectorDispatch"),
    "integer": (0x00530350, 6, "g_menuIntegerSelectorDispatch"),
    "toggle": (0x00530368, 6, "g_menuToggleDispatch"),
    "label": (0x0052fafc, 6, "g_menuLabelSelectorDispatch"),
    "text": (0x00530498, 6, "g_menuTextGroupDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR)
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "menu-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Menu dispatch verification failed: {error}\n")
    print("Verified fourteen menu widget tables: 84/84 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
