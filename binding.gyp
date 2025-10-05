{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "fhe-test.cc", "JSFheUInt8.cc", "JSFheUInt32.cc" ],
      
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },

      "include_dirs": [
         "node_modules/node-addon-api",
        "../tfhe-rs/target/release"
      ],
      
      "libraries": [
        "-L../../tfhe-rs/target/release",   # library search path
        "-ltfhe"             # link libmylib.so / libmylib.a
      ],

      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ]
    }
  ]
}
