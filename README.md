```
kepler@youpi:~/zama/fhe-test1$ node-gyp configure build && node test.js
gyp info it worked if it ends with ok
gyp info using node-gyp@11.4.2
gyp info using node@22.20.0 | darwin | arm64
gyp info find Python using Python version 3.9.6 found at "/Applications/Xcode.app/Contents/Developer/usr/bin/python3"

gyp info spawn /Applications/Xcode.app/Contents/Developer/usr/bin/python3
gyp info spawn args [
gyp info spawn args '/Users/kepler/local/nodejs/lib/node_modules/node-gyp/gyp/gyp_main.py',
gyp info spawn args 'binding.gyp',
gyp info spawn args '-f',
gyp info spawn args 'make',
gyp info spawn args '-I',
gyp info spawn args '/Users/kepler/zama/fhe-test1/build/config.gypi',
gyp info spawn args '-I',
gyp info spawn args '/Users/kepler/local/nodejs/lib/node_modules/node-gyp/addon.gypi',
gyp info spawn args '-I',
gyp info spawn args '/Users/kepler/Library/Caches/node-gyp/22.20.0/include/node/common.gypi',
gyp info spawn args '-Dlibrary=shared_library',
gyp info spawn args '-Dvisibility=default',
gyp info spawn args '-Dnode_root_dir=/Users/kepler/Library/Caches/node-gyp/22.20.0',
gyp info spawn args '-Dnode_gyp_dir=/Users/kepler/local/nodejs/lib/node_modules/node-gyp',
gyp info spawn args '-Dnode_lib_file=/Users/kepler/Library/Caches/node-gyp/22.20.0/<(target_arch)/node.lib',
gyp info spawn args '-Dmodule_root_dir=/Users/kepler/zama/fhe-test1',
gyp info spawn args '-Dnode_engine=v8',
gyp info spawn args '--depth=.',
gyp info spawn args '--no-parallel',
gyp info spawn args '--generator-output',
gyp info spawn args 'build',
gyp info spawn args '-Goutput_dir=.'
gyp info spawn args ]
gyp info spawn make
gyp info spawn args [ 'BUILDTYPE=Release', '-C', 'build' ]
make: Nothing to be done for `all'.
gyp info ok 
generate_keys...
Uint8Array(23910) [
  0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0,
  ... 23810 more items
]
Hey1 = 42
Hey2 = 18
Hadd = 60
kepler@youpi:~/zama/fhe-test1$ 
```

