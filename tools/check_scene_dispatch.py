#!/usr/bin/env python3
"""Verify all recovered startup scene callback pointers without running the game."""

import argparse
import csv
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from check_application_dispatch import map_symbols
from folded_functions import code_bytes
from toolchain import ROOT

TABLES = {
    'foyer_action_0': (0x0053334c, 4, 'g_foyerSceneActionDispatch0'),
    'foyer_action_1': (0x0053336c, 4, 'g_foyerSceneActionDispatch1'),
    'foyer_action_2': (0x0053337c, 4, 'g_foyerSceneActionDispatch2'),
    'foyer_action_3': (0x0053338c, 4, 'g_foyerSceneActionDispatch3'),
    'foyer_action_4': (0x0053339c, 4, 'g_foyerSceneActionDispatch4'),
    'foyer_action_5': (0x005333ac, 4, 'g_foyerSceneActionDispatch5'),
    'foyer_action_6': (0x005333bc, 4, 'g_foyerSceneActionDispatch6'),
    'foyer_action_7': (0x005333cc, 4, 'g_foyerSceneActionDispatch7'),
    'secondary_manager': (0x005304e0, 21, 'g_secondaryManagerDispatch'),
    'network': (0x005342bc, 21, 'g_networkAppLayerDispatch'),
    'battle': (0x0052f444, 21, 'g_battleManagerDispatch'),
    'attract_timer': (0x0052f4c0, 20, 'g_controllerStartupSceneDispatch'),
    'pause_observer': (0x0052f5d8, 5, 'g_gamePauseObserverDispatch'),
    'global_pause': (0x005360d0, 20, 'g_globalPauseDispatch'),
    'secondary_directional_light': (0x00530534, 20, 'g_secondaryDirectionalLightDispatch'),
    'secondary_ambient_light': (0x00530584, 20, 'g_secondaryAmbientLightDispatch'),
    'secondary_car_resources': (0x0052f68c, 20, 'g_secondaryCarResourceDispatch'),
    'secondary_archive': (0x0053648c, 13, 'g_secondaryContentArchiveDispatch'),
    'secondary_archive_base': (0x005364c0, 13, 'g_secondaryContentArchiveBaseDispatch'),
    'brick_builder': (0x00532930, 21, 'g_secondaryBrickBuilderDispatch'),
    'scheduled_setup': (0x005324e0, 10, 'g_scheduledActionSetupCallbacks'),
    'event_state': (0x00536a54, 24, 'g_foyerSceneEventStateDispatch'),
    'foyer_action_8': (0x005333dc, 4, 'g_foyerSceneActionDispatch8'),
    'foyer_action_9': (0x005333ec, 4, 'g_foyerSceneActionDispatch9'),
    'foyer_action_10': (0x005333fc, 4, 'g_foyerSceneActionDispatch10'),
    'foyer_action_11': (0x0053345c, 4, 'g_foyerSceneActionDispatch11'),
    'foyer_action_12': (0x0053340c, 4, 'g_foyerSceneActionDispatch12'),
    'foyer_action_13': (0x0053342c, 4, 'g_foyerSceneActionDispatch13'),
    'foyer_action_14': (0x0053341c, 4, 'g_foyerSceneActionDispatch14'),
    'foyer_action_15': (0x0053347c, 4, 'g_foyerSceneActionDispatch15'),
    'foyer_action_17': (0x0053343c, 4, 'g_foyerSceneActionDispatch17'),
    'foyer_action_18': (0x0053344c, 4, 'g_foyerSceneActionDispatch18'),
    'foyer_action_19': (0x0053346c, 4, 'g_foyerSceneActionDispatch19'),
    "foyer_audio_target_base": (0x00532310, 22, "g_foyerSceneTargetBaseDispatch"),
    "foyer_audio_target": (0x005323b8, 21, "g_foyerSceneAudioTargetDispatch"),
    "foyer_action_callbacks": (0x00532508, 18, "g_foyerSceneActionCallbacks"),
    'foyer_template_lifecycle': (0x00536500, 20, 'g_foyerTemplateLifecycleDispatch'),
    'foyer_template': (0x00536550, 21, 'g_foyerTemplateInstanceDispatch'),
    'foyer_animation': (0x00536604, 21, 'g_foyerAnimationControllerDispatch'),
    'foyer_mesh': (0x0053709c, 2, 'g_foyerMeshUpdateServiceDispatch'),
    'foyer_mesh_base': (0x005370a4, 2, 'g_foyerMeshUpdateServiceBaseDispatch'),
    'character_head_lifecycle': (0x00530880, 20, 'g_characterVisualHeadLifecycleDispatch'),
    'character_head': (0x005308d0, 20, 'g_characterVisualHeadDispatch'),
    'character_body_lifecycle': (0x00531b40, 20, 'g_characterVisualBodyLifecycleDispatch'),
    'character_body': (0x00531b90, 21, 'g_characterVisualBodyDispatch'),
    'character_visual': (0x00531be4, 21, 'g_characterVisualDispatch'),
    'foyer_manager': (0x00530e54, 21, 'g_foyerManagerDispatch'),
    'foyer_scene': (0x005332e8, 20, 'g_foyerSceneResourceDispatch'),
    'foyer_task': (0x0053241c, 20, 'g_controllerTaskManagerDispatch'),
    'foyer_task_source': (0x00532488, 6, 'g_controllerTaskSourceDispatch'),
    'foyer_task_removed': (0x005324a0, 4, 'g_controllerTaskSourceRemovedDispatch'),
    'npc_text': (0x00532628, 21, 'g_npcTextManagerDispatch'),
    'game_template': (0x00536ac0, 24, 'g_gameTemplateInstanceDispatch'),
    "secondary_render_lifecycle": (0x00536000, 20, "g_secondaryRenderPhysicalDispatch"),
    "secondary_render": (0x00536050, 27, "g_secondaryRenderOwnerDispatch"),
    "foyer_model_lifecycle": (0x00536264, 20, "g_foyerModelLifecycleDispatch"),
    "foyer_model": (0x005362b4, 21, "g_foyerModelRenderDispatch"),
    "physical": (0x00535398, 20, "g_physicalLifecycleDispatch"),
    "scene_feedback": (0x005331f0, 20, "g_controllerSceneObjectDispatch"),
    "circle_pool": (0x0053632c, 21, "g_foyerSceneCirclePoolDispatch"),
    "reverb_area": (0x00531188, 20, "g_reverbAreaObjectDispatch"),
    "reverb_gateway": (0x005311f8, 20, "g_reverbGatewayObjectDispatch"),
    "reverb_init": (0x005312a8, 20, "g_reverbInitObjectDispatch"),
    "factory_reverb_area": (0x005311d8, 4, "g_reverbAreaFactoryDispatch"),
    "factory_reverb_gateway": (0x005311e8, 4, "g_reverbGatewayFactoryDispatch"),
    "factory_reverb_init": (0x00531298, 4, "g_reverbInitFactoryDispatch"),
    "factory_base": (0x00536318, 4, "g_gameObjectFactoryEntryDispatch"),
    "factory_surface": (0x00536cf8, 4, "g_surfaceManagerFactoryDispatch"),
    "factory_race_start": (0x0052f7e4, 4, "g_raceStartPositionFactoryDispatch"),
    "race_start": (0x0052f7f4, 21, "g_raceStartPositionObjectDispatch"),
    "race_start_base": (0x0052f848, 21, "g_raceStartPositionBaseObjectDispatch"),
    "reverb_manager": (0x00531248, 20, "g_reverbManagerDispatch"),
    "car_records_base": (0x00536d64, 22, "g_carDisplayIndexedRecordOwnerBaseDispatch"),
    "car_name_transform": (0x0052f318, 20, "g_carDisplayNameTransformDispatch"),
    "car_model_transform": (0x0053529c, 20, "g_carDisplayTransformDispatch"),
    "car_base_transform": (0x00531c3c, 20, "g_carDisplayBaseTransformDispatch"),
    "car_name": (0x0052f368, 20, "g_carDisplayNameObjectDispatch"),
    "car_model": (0x005352ec, 20, "g_carDisplayModelObjectDispatch"),
    "car_records": (0x00536d08, 23, "g_carDisplayIndexedRecordOwnerDispatch"),
    "model_binding": (0x00530f98, 2, "g_gameObjectModelBindingDispatch"),
    "kill_sphere": (0x00535d18, 21, "g_killSphereSupervisorDispatch"),
    "secondary_fx": (0x00533024, 20, "g_secondaryFxManagerDispatch"),
    "bodypart": (0x0052f584, 21, "g_bodypartManagerDispatch"),
    "glyph": (0x0052f57c, 2, "g_overlayGlyphManagerDispatch"),
    "screen": (0x005325b8, 21, "g_screenPrinterDispatch"),
    "textbox": (0x00530a68, 21, "g_textBoxManagerDispatch"),
    "nested_textbox": (0x00530a10, 21, "g_nestedTextBoxManagerDispatch"),
    "pause": (0x0052f5ec, 20, "g_pauseDetectorDispatch"),
    "pause_initial": (0x0052f63c, 20, "g_pauseDetectorInitialDispatch"),
}


def verify_constant_callbacks(original, rebuilt, link_map, inventory):
    # These two retail targets have opposite return values. Pointer inventories
    # alone cannot detect a wrong symbol association repeated in both the CSV
    # and C++ table, so also check their complete original/emitted bodies.
    aliases = {
        0x00403910: ("GameObjectBaseZero", bytes.fromhex("33c0c3")),
        0x00403920: ("GameObjectBaseTrue", bytes.fromhex("b801000000c3")),
    }
    symbols = map_symbols(link_map)
    for address, (symbol, expected) in aliases.items():
        if code_bytes(original.read_bytes(), address, len(expected)) != expected:
            raise ValueError(f"Unexpected retail constant callback at {address:08x}")
        if code_bytes(rebuilt.read_bytes(), symbols[symbol], len(expected)) != expected:
            raise ValueError(f"Unexpected emitted constant callback: {symbol}")
    with inventory.open(newline="", encoding="utf-8") as stream:
        for row in csv.DictReader(stream):
            target = int(row["original"], 16)
            if target in aliases and row["symbol"] != aliases[target][0]:
                raise ValueError(f"{row['table']}+{row['offset']}: wrong constant-return callback")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "scene-dispatch.csv", TABLES)
        verify_constant_callbacks(args.original, args.build_dir / "LEGO Racers 2.exe",
                                  args.build_dir / "legoracers2.map", ROOT / "reccmp" / "scene-dispatch.csv")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Scene dispatch verification failed: {error}\n")
    count = sum(spec[1] for spec in TABLES.values())
    print(f"Verified startup scene tables: {count}/{count} retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
