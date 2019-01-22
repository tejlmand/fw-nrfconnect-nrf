#secp256r1 = prime256v1 = NIST P-256
import cfile as C
import operator
from hashlib import sha256
from ecdsa import SigningKey, NIST256p

def arr_to_hexstr(arr):
    return b''.join([bytes([x]) for x in arr])

def hexstr_to_array(hexstr):
    ret_str = ""
    for byte in map(operator.add, hexstr[::2], hexstr[1::2]):
        ret_str += "0x"+byte+","
    return ret_str[:-1]

fw_data_file = open('fw_data.py')
exec(fw_data_file.read())
with open('root-ec-p256.pem') as f:
    pem = f.read()

fw_sk = SigningKey.generate(curve=NIST256p, hashfunc = sha256)
fw_sk = fw_sk.from_pem(pem)
fw_vk = fw_sk.get_verifying_key()
fw_hex = arr_to_hexstr(fw_data)
generated_sig = fw_sk.sign(fw_hex, hashfunc = sha256)
fw_sig = [0xac, 0x95, 0x65, 0x12, 0x30, 0xde, 0xe1, 0xb, 0x8, 0x57, 0xd2, 0x9, 0x97, 0x1f, 0xd5, 0x17, 0x7c, 0xf4, 0x53, 0x6e, 0xe4, 0xa8, 0x19, 0xab, 0xae, 0xc9, 0x50, 0xcc, 0xca, 0xe2, 0x75, 0x48, 0xa3, 
0x82, 0x3f, 0xf0, 0x93, 0xcc, 0x2a, 0x64, 0xa8, 0xd, 0xab, 0x7f, 0x4d, 0xf7, 0x3d, 0xec, 0x9a, 0xac, 0x4, 0x72, 0x54, 0x2d, 0x55, 0xee, 0xca, 0xc0, 0x79, 0xad, 0x2c, 0x6a, 0xee, 0x58]
fw_sig_hex = arr_to_hexstr(fw_sig)
print(sha256(fw_hex).hexdigest())
fw_hash = sha256(fw_hex).hexdigest()
fw_hash = hexstr_to_array(fw_hash) 
print(generated_sig.hex())
get_sig = b'\xac\x95e\x120\xde\xe1\x0b\x08W\xd2\t\x97\x1f\xd5\x17|\xf4Sn\xe4\xa8\x19\xab\xae\xc9P\xcc\xca\xe2uH\xa3\x82?\xf0\x93\xcc*d\xa8\r\xab\x7fM\xf7=\xec\x9a\xac\x04rT-U\xee\xca\xc0y\xad,j\xeeX'
print(get_sig.hex())
print(fw_sig_hex.hex())
gen_sig = hexstr_to_array(generated_sig.hex())

fw_x = fw_vk.pubkey.point.x()
fw_pubkey_x = hexstr_to_array(fw_x.to_bytes(32, "big").hex())
fw_y = fw_vk.pubkey.point.y()
fw_pubkey_y = hexstr_to_array(fw_y.to_bytes(32, "big").hex())
fw_pubkey = fw_pubkey_x +","+ fw_pubkey_y

assert fw_vk.verify(generated_sig, fw_hex, hashfunc = sha256)
assert fw_vk.verify(get_sig, fw_hex, hashfunc= sha256)
assert fw_vk.verify(fw_sig_hex, fw_hex, hashfunc = sha256)


test_vectors = C.cfile('test_vector.h')
sk = SigningKey.generate(curve=NIST256p, hashfunc = sha256)
vk = sk.get_verifying_key()
my_hash = b"breadcrumb"
my_hash_array = hexstr_to_array(my_hash.hex())
breadcrumb = sha256(b"breadcrumb")
sha256_hash =  hexstr_to_array(breadcrumb.hexdigest())

signature = sk.sign(my_hash)
r = signature[:int(len(signature)/2)]
s = signature[int(len(signature)/2):]
sig_r = hexstr_to_array(r.hex())
sig_s = hexstr_to_array(s.hex())
sig_concat = hexstr_to_array(signature.hex())

x = vk.pubkey.point.x()
pubkey_x = hexstr_to_array(x.to_bytes(32, "big").hex())
y = vk.pubkey.point.y()
pubkey_y = hexstr_to_array(y.to_bytes(32, "big").hex())

pubkey_concat = pubkey_x + "," + pubkey_y

mcuboot_key = [0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04, 0x2a, 0xcb, 0x40, 0x3c, 0xe8,
    0xfe, 0xed, 0x5b, 0xa4, 0x49, 0x95, 0xa1, 0xa9,
    0x1d, 0xae, 0xe8, 0xdb, 0xbe, 0x19, 0x37, 0xcd,
    0x14, 0xfb, 0x2f, 0x24, 0x57, 0x37, 0xe5, 0x95,
    0x39, 0x88, 0xd9, 0x94, 0xb9, 0xd6, 0x5a, 0xeb,
    0xd7, 0xcd, 0xd5, 0x30, 0x8a, 0xd6, 0xfe, 0x48, 0xb2, 0x4a, 0x6a, 0x81, 0x0e, 0xe5, 0xf0, 0x7d,
    0x8b, 0x68, 0x34, 0xcc, 0x3a, 0x6a, 0xfc, 0x53,
    0x8e, 0xfa, 0xc1]

mcuboot_key_hash = sha256(b''.join(bytes([x]) for x in mcuboot_key))
mcuboot_key_hash = hexstr_to_array(mcuboot_key_hash.hexdigest())

mcuboot_key = b''.join(bytes([x]) for x in mcuboot_key)
mcuboot_key = hexstr_to_array(mcuboot_key.hex())

long_input = b'a' * 100000
long_input_hash = hexstr_to_array(sha256(long_input).hexdigest())
long_input = hexstr_to_array(long_input.hex())

#fw_hash = b'f1ab54f3f73164d9990159bbd341ec74789caaff14fff7d26ddab56da44a3'
fw_sig = b'ac95651230dee1b857d29971fd5177cf4536ee4a819abaec950cccae27548a3823ff093cc2a64a8dab7f4df73dec98'

assert vk.verify(signature, my_hash)
test_vectors.code.append(C.variable("pub_x", typename='u8_t', array=32))
test_vectors.code.append(C.statement("= {" + pubkey_x +"}"))
test_vectors.code.append(C.variable("pub_y", typename='u8_t', array=32))
test_vectors.code.append(C.statement("= {" + pubkey_y +"}"))
test_vectors.code.append(C.variable("pub_concat", typename='u8_t', array=64))
test_vectors.code.append(C.statement("= {" + pubkey_concat +"}"))
test_vectors.code.append(C.variable("const_pub_concat", typename='static const u8_t', array=64))
test_vectors.code.append(C.statement("= {" + pubkey_concat +"}"))
test_vectors.code.append(C.variable("sig_r", typename='u8_t', array=32))
test_vectors.code.append(C.statement("= {" + sig_r +"}"))
test_vectors.code.append(C.variable("sig_s", typename='u8_t', array=32))
test_vectors.code.append(C.statement("= {" + sig_s +"}"))
test_vectors.code.append(C.variable("sig_concat", typename='u8_t', array=64))
test_vectors.code.append(C.statement("= {" + sig_concat +"}"))
test_vectors.code.append(C.variable("const_sig_concat", typename='static const u8_t', array=64))
test_vectors.code.append(C.statement("= {" + sig_concat +"}"))
test_vectors.code.append(C.variable("hash", typename='u8_t', array=len(my_hash.hex())))
test_vectors.code.append(C.statement("= {" + my_hash_array +"}"))
test_vectors.code.append(C.variable("const_hash", typename='static const u8_t', array=len(my_hash.hex())))
test_vectors.code.append(C.statement("= {" + my_hash_array +"}"))
test_vectors.code.append(C.variable("hash_sha256", typename='u8_t', array=32))
test_vectors.code.append(C.statement("= {" + sha256_hash +"}"))
test_vectors.code.append(C.variable("const_hash_sha256", typename='static const u8_t', array=32))
test_vectors.code.append(C.statement("= {" + sha256_hash +"}"))
test_vectors.code.append(C.variable("mcuboot_key", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + mcuboot_key +"}"))
test_vectors.code.append(C.variable("const_mcuboot_key", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + mcuboot_key +"}"))
test_vectors.code.append(C.variable("mcuboot_key_hash", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + mcuboot_key_hash +"}"))
test_vectors.code.append(C.variable("long_input", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + long_input +"}"))
test_vectors.code.append(C.variable("const_long_input", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + long_input +"}"))
test_vectors.code.append(C.variable("long_input_hash", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + long_input_hash +"}"))
test_vectors.code.append(C.variable("image_fw_data", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + hexstr_to_array(fw_hex.hex()) +"}"))
test_vectors.code.append(C.variable("image_fw_sig", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + hexstr_to_array(fw_sig_hex.hex()) +"}"))
test_vectors.code.append(C.variable("image_gen_sig", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + gen_sig +"}"))
test_vectors.code.append(C.variable("image_public_key", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + fw_pubkey +"}"))
test_vectors.code.append(C.variable("image_fw_hash", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + fw_hash +"}"))

test_vectors.code.append(C.variable("const_fw_sig", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + hexstr_to_array(fw_sig_hex.hex()) +"}"))
test_vectors.code.append(C.variable("const_gen_sig", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + gen_sig +"}"))
test_vectors.code.append(C.variable("const_public_key", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + fw_pubkey +"}"))
test_vectors.code.append(C.variable("const_fw_hash", typename='u8_t', array=""))
test_vectors.code.append(C.statement("= {" + fw_hash +"}"))
test_vectors.code.append(C.variable("const_fw_data", typename='static const u8_t', array=""))
test_vectors.code.append(C.statement("= {" + hexstr_to_array(fw_hex.hex()) +"}"))

with open("src/test_vectors.h", "w") as f:
    f.write(str(test_vectors))

