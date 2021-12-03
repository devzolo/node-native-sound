{
  'variables': {
    'platform': '<(OS)',
    'build_arch': '<!(node -p "process.arch")',
    'build_win_platform': '<!(node -p "process.arch==\'ia32\'?\'Win32\':process.arch")',
  },
  'conditions': [
    # Replace gyp platform with node platform, blech
    ['platform == "mac"', {'variables': {'platform': 'darwin'}}],
    ['platform == "win"', {'variables': {'platform': 'win32'}}],
  ],
  'targets': [
    {
      'target_name': 'native-sound',
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      'sources': [
        'src/Utils.cc',
        'src/NativeSound.cc',
        'src/sound.cc'
      ],
      'defines' : ['NAPI_DISABLE_CPP_EXCEPTIONS','UNICODE'],
      'libraries': [
        "bass",
        "bassmix",
        "bass_fx"
      ],
      "include_dirs": [
        "./deps/bass/include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'library_dirs' : [
        "./deps/bass/x64"
      ],
    },
    {
      "target_name": "copy_modules",
      "type":"none",
      "dependencies" : [ "native-sound" ],
      "copies":[
        {
          'destination': '<(module_root_dir)\\bin\\<(platform)\\<(target_arch)',
          'files': [
            '<(module_root_dir)\\build\\Release\\native-sound.node',
          ]
        },
        {
          'destination': '<(module_root_dir)\\bin\\<(platform)\\<(target_arch)',
          'files': [
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_fx.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_aac.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_ac3.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassflac.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassmidi.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassmix.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassopus.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\basswma.dll'
          ]
        },
        {
          'destination': '<(module_root_dir)\\dist',
          'files': [
            '<(module_root_dir)\\src\\@types\\index.d.ts',
          ]
        },
        {
          'destination': '<(module_root_dir)\\..\\node-native-opengl-examples\\node_modules\\@devzolo\\node-native-sound\\dist',
          'files': [
            '<(module_root_dir)\\src\\@types\\index.d.ts',
          ]
        },
        {
          'destination': '<(module_root_dir)\\..\\node-native-opengl-examples\\node_modules\\@devzolo\\node-native-sound\\bin\\<(platform)\\<(target_arch)',
          'files': [
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_fx.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_aac.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bass_ac3.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassflac.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassmidi.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassmix.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\bassopus.dll',
            '<(module_root_dir)\\deps\\bass\\<(target_arch)\\basswma.dll',
            '<(module_root_dir)\\bin\\<(platform)\\<(target_arch)\\native-sound.node'
          ]
        }
      ]
    }
  ]
}
