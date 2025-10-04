// test.js
const addon = require('./build/Release/addon.node');

addon.GenerateKeys();

const serialized_client_key = addon.SerializeClientKey();
console.log("My secret client key is:");
console.log(serialized_client_key);

const fhe_uint8_a = addon.EncryptUInt8(42);
const fhe_uint8_b = addon.EncryptUInt8(18);

console.log("Hey1 = "+addon.DecryptUInt8(fhe_uint8_a)); // 42
console.log("Hey2 = "+addon.DecryptUInt8(fhe_uint8_b)); // 18

const fhe_uint8_c = addon.AddUInt8(fhe_uint8_a, fhe_uint8_b);

console.log("Hey1 + Hey2 = "+addon.DecryptUInt8(fhe_uint8_c)); // 60

addon.DestroyKeys();
