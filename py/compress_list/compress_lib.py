from typing import List, Optional
from pydantic import BaseModel
from random import randint
from datetime import datetime
import sys
import zlib


class MyClass1(BaseModel):
    a: Optional[int] = 0
    b: Optional[int] = 2245
    c: Optional[int] = 3456
    d: Optional[int] = 5678
    e: Optional[str] = "blah"

class MyClass2(BaseModel):
    mylist: Optional[List] = []

def get_change(current, previous):
    return  (abs(current-previous) / ((current+previous)/2)) * 100
    


if __name__ == "__main__":

    test_pre = MyClass2()

    # Generate data set
    for x in range(0,500000):
        test_pre.mylist.append(
            MyClass1(
                a=randint(0,9999),
                b=randint(0,9999),
                c=randint(0,9999),
                d=randint(0,9999)
            )
        )
    
    # Measure pre-compression
    pre_compress_str = test_pre.json()
    print(f"PRE-HASH: {hash(pre_compress_str)}")
    pre_len = len(pre_compress_str)
    pre_size = sys.getsizeof(pre_compress_str)
    print(f"BEFORE COMPRESSION Length {pre_len}, size {pre_size}")

    # Compress
    start = datetime.now()
    compressed_b_array = zlib.compress(pre_compress_str.encode(), level=zlib.Z_DEFAULT_COMPRESSION)
    end = datetime.now()
    comp_len = len(compressed_b_array)
    comp_size = sys.getsizeof(compressed_b_array)
    print(f"Total Compression Time: {(end-start).total_seconds()}")
    print(f"AFTER COMPRESSION Length {comp_len}, size {comp_size}")
    print(f"Size difference: {get_change(comp_size, pre_size)}%")

    # Decompress
    start = datetime.now()
    DE_compressed_b_array = zlib.decompress(compressed_b_array)
    end = datetime.now()
    print(f"Total Decompression Time: {(end-start).total_seconds()}")
    print(f"AFTER DECOMPRESSION Length {len(DE_compressed_b_array)}, size {sys.getsizeof(DE_compressed_b_array)}")


    # Convert back to pydantic object
    DE_compressed_str = DE_compressed_b_array.decode()
    print(f"AFTER CONVERT TO STR Length {len(DE_compressed_str)}, size {sys.getsizeof(DE_compressed_str)}")
    
    test_post = MyClass2.parse_raw(DE_compressed_str)
    print(f"POST-HASH: {hash(test_post.json())}")
    print(f"Hashes equal? - > {hash(pre_compress_str)==hash(test_post.json())}")