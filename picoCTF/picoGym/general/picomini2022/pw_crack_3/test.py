import hashlib

pos_pw_list = ["f09e", "4dcf", "87ab", "dba8", "752e", "3961", "f159"]

for x in pos_pw_list:
    m = hashlib.md5()
    m.update(x.encode())
    print(f"{m.digest().hex()} - {x}")
    # if m.digest() == b"6de1a555805ddddda852ab3e57ea7b2b":
    #     print(x)


correct_pw_hash = open("level3.hash.bin", "rb").read()
print(correct_pw_hash.hex())
