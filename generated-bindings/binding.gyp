{
    "targets": [
        {
            "target_name": "nodeminify",
            "cflags": [ "-O3" ],
            "cflags_cc": [ "-O3" ],
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ],
            "sources": [ "nodeminify.cc" ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<(module_root_dir)/prebuild"
            ],
            "defines": [ "NAPI_CPP_EXCEPTIONS", "NAPI_EXPERIMENTAL" ],
            "conditions": [
                [ 'OS=="linux"', {
                    "cflags": [ "-O3", "-fdata-sections", "-ffunction-sections" ],
                    "cflags_cc": [ "-O3", "-fdata-sections", "-ffunction-sections" ],
                    "ldflags": [ "-Wl,--gc-sections" ],
                    "libraries": [
                        "<(module_root_dir)/prebuild/nodeminify.a"
                    ]
                }],
                [ 'OS=="mac"', {
                    "cflags": [ "-O3", "-fdata-sections", "-ffunction-sections" ],
                    "cflags_cc": [ "-O3", "-fdata-sections", "-ffunction-sections" ],
                    "xcode_settings": {
                        "OTHER_LDFLAGS": [ "-Wl,-dead_strip" ]
                    },
                    "libraries": [
                        "<(module_root_dir)/prebuild/nodeminify.a"
                    ]
                }],
                [ 'OS=="win"', {
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "Optimization": "2",
                            "InlineFunctionExpansion": "2",
                            "FavorSizeOrSpeed": "1",
                            "StringPooling": "true",
                            "MinimalRebuild": "false",
                            "BufferSecurityCheck": "false"
                        },
                        "VCLinkerTool": {
                            "OptimizeReferences": "2",
                            "EnableCOMDATFolding": "2"
                        }
                    },
                    "libraries": [
                        "<(module_root_dir)/prebuild/nodeminify.lib"
                    ]
                }],
                [ 'OS!="win" and OS!="linux" and OS!="mac"', {
                    "cflags": [ "-O3" ],
                    "cflags_cc": [ "-O3" ],
                    "libraries": [
                        "<(module_root_dir)/prebuild/nodeminify.a"
                    ]
                }]
            ]
        }
    ]
}