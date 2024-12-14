# https://dev.epicgames.com/documentation/en-us/unreal-engine/python-api/class/MovieSceneSkeletalAnimationParams?application_version=5.4
# https://dev.epicgames.com/documentation/en-us/unreal-engine/python-api/class/MovieSceneSkeletalAnimationSection?application_version=5.4

import unreal

# 필요한 언리얼 엔진 API 초기화
editor_util = unreal.EditorAssetLibrary()
sequencer_util = unreal.LevelSequenceEditorBlueprintLibrary()

# 사용자 정의 변수
animations_path = "/Game/Characters/Bloodborne/Animations/Weapon/SawCleaver/Movement"

mirror_table_path = (
    "/Game/Characters/Bloodborne/Animations/NewMirrorDataTable.NewMirrorDataTable"
)
level_sequence_path = "/Game/Characters/Bloodborne/Animations/Weapon/SawCleaver/Test/TestSequence.TestSequence"
skeletal_mesh_path = (
    "/Game/Characters/Bloodborne/Meshes/Hunter/NakedBody/NakedBody.NakedBody"
)
output_path = (
    "/Game/Characters/Bloodborne/Animations/Weapon/SawCleaver/CompleteMovement(flip)"
)


# 스켈레톤 로드
skeletal_mesh = unreal.load_asset(skeletal_mesh_path)
skeleton = skeletal_mesh.skeleton

# AnimSequence 생성
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

factory = unreal.AnimSequenceFactory()
factory.set_editor_property("target_skeleton", skeleton)

# 애니메이션과 시퀀스 로딩
level_sequence = unreal.load_asset(level_sequence_path)

# '세계'와 '바인딩'에 대한 참조 설정
world = unreal.EditorLevelLibrary.get_editor_world()
sequence_binding = level_sequence.get_bindings()[0]  # 예시로 첫 번째 바인딩 사용

animations = editor_util.list_assets(animations_path, recursive=True)

# 처리 루프
for anim_path in animations:
    anim = editor_util.load_asset(anim_path)
    if anim is None or not isinstance(anim, unreal.AnimSequence):
        unreal.log_warning(f"Invalid or unsupported asset: {anim_path}")
        continue

    output_anim_name = anim.get_name()
    anim_sequence = asset_tools.create_asset(
        asset_name=output_anim_name,
        package_path=output_path,
        asset_class=unreal.AnimSequence,
        factory=factory,
    )

    if isinstance(anim, unreal.AnimSequence):
        try:
            # 기존 애니메이션 트랙 제거
            tracks = sequence_binding.get_tracks()
            for track in tracks:
                if isinstance(track, unreal.MovieSceneSkeletalAnimationTrack):
                    sequence_binding.remove_track(track)

            # MovieSceneSkeletalAnimationParams 설정
            params = unreal.MovieSceneSkeletalAnimationParams()
            params.set_editor_property("Animation", anim)

            mirror = unreal.load_asset(mirror_table_path)
            params.set_editor_property("Mirror_Data_Table", mirror)

            # 트랙 클래스 지정
            animation_track_class = unreal.MovieSceneSkeletalAnimationTrack

            # 트랙을 MovieScene에 추가
            new_animation_track = sequence_binding.add_track(animation_track_class)

            # 애니메이션 클립 추가
            section = new_animation_track.add_section()

            # start_frame = unreal.FrameNumber(0)
            # end_frame = unreal.FrameNumber(int(test_anim.get_play_length() * level_sequence.get_display_rate().numerator))
            start_frame = 0
            end_frame = int(
                anim.get_play_length() * level_sequence.get_display_rate().numerator
            )
            section.set_range(start_frame, end_frame)

            # params 설정
            section.set_editor_property("params", params)

            level_sequence.set_playback_start(start_frame)
            level_sequence.set_playback_end(end_frame - 1)

            export_option = unreal.AnimSeqExportOption()
            export_success = unreal.SequencerTools.export_anim_sequence(
                world=world,
                sequence=level_sequence,
                anim_sequence=anim_sequence,
                export_option=export_option,
                binding=sequence_binding,
                create_link=True,
            )

            if export_success:
                unreal.log(f"Successfully exported animation: {anim.get_name()}")
            else:
                unreal.log_error(f"Failed to export animation: {anim.get_name()}")

        except Exception as e:
            unreal.log_error(f"Error processing {anim.get_name()}: {e}")
