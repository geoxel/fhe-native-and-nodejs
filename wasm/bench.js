// Here import assert to check the decryption went well and panic otherwise
const assert = require('node:assert').strict;
const { TfheConfigBuilder, TfheConfig, TfheClientKey, FheUint8, FheUint32 } = require("../../tfhe-rs/tfhe/pkg");

function benchmark(func, params, iterations = 1000) {
    const t1 = performance.now();
    for (let i = 0; i < iterations; i++) {
        func(params, i);
    }
    const t2 = performance.now();
    return (t2 - t1) / iterations;
}

function go_testing(params, index) {
    let client_key = params[0];
    let mask = params[1];
    let encrypt_with_client_key = params[2];
    let deserialize = params[3];

    let a = index & mask;
    let enc_a = encrypt_with_client_key(a, client_key); // FheUint{8,32}
    let serialized_enc_a = enc_a.serialize(); // Uint8Array
    let enc_a2 = deserialize(serialized_enc_a); // FheUint{8,32}
    let a2 = enc_a2.decrypt(client_key);
    assert(a2 == a);

    enc_a2.free();
    enc_a.free();
}

function wasm_bench() {

    let config_builder = TfheConfigBuilder.default();
    let config = config_builder.build(); // TfheConfig

    console.log("Generating keys...")
    let client_key = TfheClientKey.generate(config);

    console.log("Encrypting UInt8(42)...")
    let a = 42;
    let enc_a = FheUint32.encrypt_with_client_key(a, client_key); // FheUint32
    console.log("Serializing...");
    let serialized_enc_a = enc_a.serialize(); // Uint8Array
    console.log("Serialized value is : ");
    console.log(serialized_enc_a);

    console.log("Deserializing...");
    let enc_a2 = FheUint32.deserialize(serialized_enc_a); // FheUint32
    console.log("Decrypting...")
    let a2 = enc_a2.decrypt(client_key);
    console.log("We got " + a2 + " and it should be " + a);
    assert(a2 == a);

    enc_a.free();
    enc_a2.free();

    console.log("Let's bench...")

    let duration = benchmark(go_testing, [client_key, (1 << 8) - 1, FheUint8.encrypt_with_client_key, FheUint8.deserialize], 1000);
    console.log(`FheUInt8  bench: ${duration.toFixed(2)} ms/iter`);

    duration = benchmark(go_testing, [client_key, (1 << 16) - 1, FheUint32.encrypt_with_client_key, FheUint32.deserialize], 200);
    console.log(`FheUInt32 bench: ${duration.toFixed(2)} ms/iter`);

    console.log("All done!")
}

wasm_bench();
