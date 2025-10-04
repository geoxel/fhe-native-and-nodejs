Building a NodeJS native TFHE addon.

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
*What a time to be alive!*
