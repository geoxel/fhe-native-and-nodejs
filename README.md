Building a NodeJS native TFHE addon and benchmarking it against WASM.

## Setup

Retrieving the TFHE repo and build it with the C API.
```bash
git clone https://github.com/zama-ai/tfhe-rs.git
cd tfhe-rs
RUSTFLAGS="-C target-cpu=native" cargo +nightly build --release --features=high-level-c-api -p tfhe
ls -lF target/release
cd ..
```
Retrieve this repo and install the node-gyp and the node-addon-api module.
```bash
git clone https://github.com/geoxel/fhe-native-and-nodejs.git
cd fhe-native-and-nodejs
npm install -g node-gyp
npm install node-addon-api
```
Configure and create the Makefile, etc:
```bash
node-gyp configure
```
## Build
Build the NodeJS addon:
```bash
$ node-gyp build
gyp info it worked if it ends with ok
gyp info using node-gyp@11.4.2
gyp info using node@22.20.0 | darwin | arm64
gyp info spawn make
gyp info spawn args [ 'BUILDTYPE=Release', '-C', 'build' ]
  CC(target) Release/obj.target/nothing/node_modules/node-addon-api/nothing.o
  LIBTOOL-STATIC Release/nothing.a
warning: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/libtool: archive library: Release/nothing.a the table of contents is empty (no object file members in the library define global symbols)
  CXX(target) Release/obj.target/addon/fhe-test.o
  CXX(target) Release/obj.target/addon/JSFheUInt8.o
  SOLINK_MODULE(target) Release/addon.node
gyp info ok
```
## Testing
Run it:
```
$ node test.js
generate_keys...
My secret client key is:
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
Hey1 + Hey2 = 60
```
## What about benchmarking against NodeJS WASM ?
Let's compare the WASM and native RUST/C interop performance on simple operations: encrypt a uint8 (and uint32) value, serialize it, deserialize it, and decrypt it.

First compile TFHE-rs to WASM :
```bash
$ cd ~/zama/tfhe-rs
$ make build_node_js_api
cd tfhe && \
        RUSTFLAGS="" rustup run "stable" \
                wasm-pack build --release --target=nodejs \
                -- --features=boolean-client-js-wasm-api,shortint-client-js-wasm-api,integer-client-js-wasm-api,zk-pok,extended-types
[INFO]: 🎯  Checking for the Wasm target...
[INFO]: 🌀  Compiling to Wasm...
    Finished `release` profile [optimized] target(s) in 0.15s
[INFO]: ⬇️  Installing wasm-bindgen...
[INFO]: Optimizing wasm binaries with `wasm-opt`...
[INFO]: ✨   Done in 8.68s
[INFO]: 📦   Your wasm pkg is ready to publish at /Users/kepler/zama/tfhe-rs/tfhe/pkg.
```
Now go to the WASM directory and run the benchmark:
```bash
$ cd ~/zama/fhe-native-and-nodejs/wasm
$ node bench.js
Generating keys...
Encrypting UInt8(42)...
Serializing...
Serialized value is :
Uint8Array(263448) [
   16,   0,   0,   0,   0,   0,   0,   0,   1,   8,   0,  0,
    0,   0,   0,   0,  93,  35,  43, 246,  23,  94,  55, 46,
  150, 157,  99, 201, 197,  50,  94,  47, 201, 164, 158, 24,
  225, 119,  99, 199,  36,  65,  22, 219, 235,  95, 167, 41,
  191,  24,  10, 206, 119, 213, 177, 226,  72, 187,  38, 17,
   33, 226,  12,  10, 217, 141, 208,  45,  21,  63, 148, 96,
    7,  19,  22, 189, 210, 165, 182,  14,  10,   2, 171, 51,
  201,  58, 195, 128, 236, 169, 201, 158,  73, 214, 150, 47,
   10,  90, 108,  77,
  ... 263348 more items
]
Deserializing...
Decrypting...
We got 42 and it should be 42
Let's bench...
FheUInt8  bench: 1.26 ms/iter
FheUInt32 bench: 4.99 ms/iter
All done!

$ uname -a
Darwin youpi.local 25.0.0 Darwin Kernel Version 25.0.0: Mon Aug 25 21:17:45 PDT 2025; root:xnu-12377.1.9~3/RELEASE_ARM64_T8103 arm64 arm64 Macmini9,1 Darwin
```
Finaly run the Rust/C NodeJS interop benchmark:
```bash
$ cd ~/zama/fhe-native-and-nodejs
$ node bench.js
Generating keys...
Encrypting UInt8(42 and 18)...
Hey1 = 42
Hey2 = 18
Hey1 + Hey2 = 60
Serializing a...
Serialized value is :
Uint8Array(263448) [
   16,   0,   0,   0,   0,   0,   0,   0,   1,   8,   0,   0,
    0,   0,   0,   0, 168,  44, 183, 252,   8,  14, 214, 136,
   49, 141,  32, 154, 164, 246,  30,  20, 245,  25, 213,  54,
  163, 113, 127,  54,  83, 198,  56,   5,  76, 135, 158,  68,
  232,  32, 160,  57,  11,  42,  92, 126,  49, 168,  65,  20,
   22,  45, 121,  11, 145, 163,  48, 224, 196, 235,  13,   4,
   47, 228,  91, 140,  98,  37, 240,   2, 135, 195,  62,   0,
  140, 216,  41,  77,  88, 213, 157,  14,   1,  18, 197, 245,
  197, 212, 192,  71,
  ... 263348 more items
]
Deserializing...
Decrypting...
We got 42 and it should be 42
Let's bench...
FheUInt8  bench: 0.33 ms/iter
FheUInt32 bench: 1.29 ms/iter

$ uname -a
Darwin youpi.local 25.0.0 Darwin Kernel Version 25.0.0: Mon Aug 25 21:17:45 PDT 2025; root:xnu-12377.1.9~3/RELEASE_ARM64_T8103 arm64 arm64 Macmini9,1 Darwin
```
Native implementation over the WASM-compiled TFHE library shows some nice performance gain:
| Benchmark | WASM Time (ms) | Native Time (ms)| Performance gain with native interop|
| ----------- | ----------- |  ----------- | ----------- |
| UInt8  | 1.26 | 0.33 | ×3.81 faster |
| UInt32 | 4.99 | 1.29 | ×3.87 faster |

*What a time to be alive!*
