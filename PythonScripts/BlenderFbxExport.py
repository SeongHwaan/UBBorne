import bpy
import os

input_folder = r""
output_folder = r""
glb_files = [f for f in os.listdir(input_folder) if f.endswith('.glb')]
bpy.context.scene.unit_settings.scale_length = 0.01
for glb_file in glb_files:
    try:
        input_path = os.path.join(input_folder, glb_file)
        bpy.ops.object.select_all(action='SELECT')  # 모든 오브젝트 선택
        bpy.ops.object.delete()  # 선택된 모든 오브젝트 삭제
        for block in bpy.data.actions:
            bpy.data.actions.remove(block)
        for block in bpy.data.armatures:
            bpy.data.armatures.remove(block)
        for block in bpy.data.meshes:
            bpy.data.meshes.remove(block)
        for block in bpy.data.objects:
            bpy.data.objects.remove(block)
        bpy.ops.import_scene.gltf(filepath=input_path)
        for obj in bpy.context.scene.objects:
            if obj.type == 'MESH' and obj.name.startswith("Icosphere"):
                bpy.data.objects.remove(obj, do_unlink=True)
        for obj in bpy.context.scene.objects:
            if obj.type not in {'ARMATURE', 'MESH'}:
                bpy.data.objects.remove(obj, do_unlink=True)
        output_path = os.path.join(output_folder, glb_file.replace('.glb', '.fbx'))
        bpy.ops.export_scene.fbx(
            filepath=output_path,
            use_selection=False,  # 모든 객체를 내보내도록 설정
            apply_unit_scale=True,  # 단위 설정 적용
            use_mesh_modifiers=True,  # 메쉬 수정자 적용
            bake_space_transform=True,  # 스페이스 변환 적용
            apply_scale_options='FBX_SCALE_NONE',  # 단위 스케일 조정 안 함
            axis_forward='-Z',  # Z축을 전방으로 설정
            axis_up='Y',  # Y축을 위로 설정
            mesh_smooth_type='EDGE',  # Smoothing을 Edge로 설정
            add_leaf_bones=False  # Armature에서 Leaf Bone 추가하지 않도록 설정
        )
        print(f"Processed {glb_file} successfully.")
    except Exception as e:
        print(f"Error processing {glb_file}: {str(e)}")
