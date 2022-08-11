#
# Finds and compiles shader files from "LearnToads.Game/Shaders" into "Build/Shaders"
#

# imports
import subprocess
import os
import glob
import getopt
import sys
import shutil
from collections import defaultdict

# defines a useful tool for flattening the lists we get from glob.glob(...)
def flatten(l):
    return [item for sublist in l for item in sublist]

def build_shaders():
    print("Building shaders...Finished")

    # get list of shaders from disk
    shader_file_types = ["*.vert", "*.frag"]
    shader_files = flatten([glob.glob(f"LearnToads.Game/Shaders/{ext}") for ext in shader_file_types])

    # ensure the build directories exist for shaders
    os.makedirs("Build/Content/Shaders", exist_ok=True)

    # get the vulkan path where glslc is located
    vulkan_path = os.environ['VulkanBinPath']

    # compile each of the shaders using glslc
    for shader_file_path in shader_files:
        
        # clean the file name
        shader_file = shader_file_path.replace("\\", "/").replace("LearnToads.Game/Shaders/", "")
        
        print(f"Building shaders...{shader_file}")

        # compile step
        subprocess.call([f"{vulkan_path}/glslc.exe", shader_file_path, f"-oBuild/Content/Shaders/{shader_file}.spv"])

    print("Building shaders...Finished")

# main entry-point
def main(argv):
    print("Building content...")

    # handle changing current working directory if needed
    opts, args = getopt.getopt(argv, "x", ["cd="])    
    opts = flatten(opts)

    pop_cwd = False
    pwd = os.getcwd()

    if "--cd" in opts:
        pop_cwd = True
        os.chdir(opts[1])

    try:
        # clean/remove previous build
        shutil.rmtree(f"{os.getcwd()}/Build/Content")
    except: pass

    # ensure the build directories exist for content
    os.makedirs("Build/Content", exist_ok=True)

    # get list of content from disk
    content_file_types = ["*.*"]
    content_files = flatten([glob.glob(f"Content/{ext}") for ext in content_file_types])
    content_files += flatten([glob.glob(f"LearnToads.Game/Shaders/{ext}") for ext in content_file_types])

    # build the shaders
    build_shaders()

    # creates a csv that supplies information about the content.
    # format: content_id, file_path
    content_lookup_csv = ""

    # get the text that we write to the content header
    content_header = """
#pragma once

// This file is auto-generated -- do not edit it manually!

#include "LTAsset.h"

namespace Content {
"""

    asset_id_counter = 0

    content_map = defaultdict(list)

    for content_file in content_files:
        asset_id = asset_id_counter
        fixed_content_file = content_file.replace("\\", "/")
        file_name_index = fixed_content_file.rfind("/") + 1
        start_of_ext = fixed_content_file.find(".", file_name_index)
        file_name = fixed_content_file[file_name_index:]
        without_ext = fixed_content_file[file_name_index:start_of_ext]
        ext = fixed_content_file[start_of_ext:]
        ns_path = fixed_content_file[:file_name_index - 1] \
            .replace(".", "/") \
            .replace("LearnToads/Game", "") \
            .replace("Content", "") \
            .replace("Shaders", "") \
            .strip("/")
        class_name = without_ext[:1].capitalize() + without_ext[1:]

        ns_parts = ns_path.split("/")
        ns_parts.remove('')

        content_ns = "UnknownContent"

        lookup_path = fixed_content_file

        asset_type_unknown = 0
        asset_type_shader  = 1
        asset_type_texture = 2
        asset_type_model   = 3

        asset_type = asset_type_unknown
        asset_type_cpp = "LTAsset"

        if "png" in ext:
            content_ns = "Images"
            asset_type = asset_type_texture
            asset_type_cpp = "LTTexture"
        elif "fbx" in ext or "obj" in ext:
            content_ns = "Models"
            asset_type = asset_type_model
            asset_type_cpp = "LTModel"
        elif "vert" in ext:
            content_ns = "VertexShaders"
            lookup_path = f"Build/Content/Shaders/{file_name}.spv"
            asset_type = asset_type_shader
            asset_type_cpp = "LTShader"
        elif "frag" in ext:
            content_ns = "FragmentShaders"
            lookup_path = f"Build/Content/Shaders/{file_name}.spv"
            asset_type = asset_type_shader
            asset_type_cpp = "LTShader"

        content_lookup_csv += f"{asset_id},{lookup_path},{asset_type}\n"

        content_map[content_ns].append((content_ns, class_name, ns_parts, asset_id, asset_type_cpp))

        asset_id_counter = asset_id_counter + 1

    for k, v in content_map.items():
        content_header += f"class {k} {{\n"
        content_header += f"private: \n"                
        
        # for element in v:
        #     content_header += f"    static LTAssetHandle m_{element[1]}; \n"

        content_header += "public: \n"

        for element in v:
            content_header += f"""
    static inline uint32_t Get{element[1]}ID();
    static LTAssetHandle Get{element[1]}();
    static LTAssetHandle Get{element[1]}NoLoad();
"""        

        content_header += f"}}; // class {k} \n\n"

    content_header += "} // namespace Content\n"
    content_header += "\n"

    content_cpp = f"""

#include "PrecompiledHeader.h"
#include "LTContent.h"
#include "LTAsset.h"
"""

    for k, v in content_map.items():
        for element in v:
            content_cpp += f"""

uint32_t Content::{k}::Get{element[1]}ID()
{{
    return {element[3]};
}}

LTAssetHandle Content::{k}::Get{element[1]}NoLoad()
{{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().Get(/* asset id = */ {element[3]}, assetHandle);
    return assetHandle;
}}

LTAssetHandle Content::{k}::Get{element[1]}()
{{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().GetLoad(/* asset id = */ {element[3]}, assetHandle);
    return assetHandle;
}}
"""        
    # write out the content header
    with open("LearnToads.Game/Content/LTContent.h", "w") as f:
        f.write(content_header)

    # write out the content header
    with open("LearnToads.Game/Content/LTContent.cpp", "w") as f:
        f.write(content_cpp)

    # write out the content header
    with open("Build/Content/content.csv", "w") as f:
        f.write(content_lookup_csv)

    # todo: so that this is not slow, should probably create some kind of per-file hash and timestamp listing file
    #       that can be used to compare/diff for changes. each run should produce this file as an output as well.
    #       then we only perform the build step for new, changed, or removed content.

    # change cwd back
    if pop_cwd:
        os.chdir(pwd)

    print("Building content...Finished")

# boilerplate
if __name__ == "__main__":
    main(sys.argv[1:])