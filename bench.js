// test.js
const assert = require('node:assert').strict;
const addon = require('./build/Release/addon.node');

function benchmark(func, params, iterations = 1000) {
    const t1 = performance.now();
    for (let i = 0; i < iterations; i++) {
        func(params, i);
    }
    const t2 = performance.now();
    return (t2 - t1) / iterations;
}

function go_testing(params, index) {
    let mask = params[0];
    let encrypt = params[1];
    let serialize = params[2];
    let deserialize = params[3];
    let decrypt = params[4];
    let destroy = params[5];

    let a = index & mask;
    let enc_a = encrypt(a);
    let serialized_enc_a = serialize(enc_a);
    let enc_a2 = deserialize(serialized_enc_a);
    let a2 = decrypt(enc_a2);
    assert(a2 == a);

    destroy(enc_a2);
    destroy(enc_a);
}

function interop_bench() {

    let a = 42;
    let b = 18;

    console.log("Encrypting UInt8(42 and 18)...")
    const enc_a = addon.EncryptUInt32(a);
    const enc_b = addon.EncryptUInt32(b);

    console.log("Hey1 = " + addon.DecryptUInt32(enc_a)); // 42
    console.log("Hey2 = " + addon.DecryptUInt32(enc_b)); // 18

    const enc_c = addon.AddUInt32(enc_a, enc_b);

    console.log("Hey1 + Hey2 = " + addon.DecryptUInt32(enc_c)); // 60

    console.log("Serializing a...");
    let serialized_enc_a = addon.SerializetUInt32(enc_a); // Uint8Array
    console.log("Serialized value is : ");
    console.log(serialized_enc_a);
    console.log("Deserializing...");
    let enc_a2 = addon.DeserializetUInt32(serialized_enc_a); // FheUint32
    console.log("Decrypting...")
    let a2 = addon.DecryptUInt32(enc_a2);
    console.log("We got " + a2 + " and it should be " + a);
    assert(a2 == a);

    addon.DestroyUInt32(enc_a);
    addon.DestroyUInt32(enc_b);
    addon.DestroyUInt32(enc_c);
    addon.DestroyUInt32(enc_a2);

    console.log("Let's bench...")

    let duration = benchmark(go_testing, [(1 << 8) - 1, addon.EncryptUInt8, addon.SerializetUInt8, addon.DeserializetUInt8, addon.DecryptUInt8, addon.DestroyUInt8], 4000);
    console.log(`FheUInt8  bench: ${duration.toFixed(2)} ms/iter`);

    duration = benchmark(go_testing, [(1 << 16) - 1, addon.EncryptUInt32, addon.SerializetUInt32, addon.DeserializetUInt32, addon.DecryptUInt32, addon.DestroyUInt32], 1000);
    console.log(`FheUInt32 bench: ${duration.toFixed(2)} ms/iter`);
}

console.log("Generating keys...")
addon.GenerateKeys();

interop_bench();

addon.DestroyKeys();
