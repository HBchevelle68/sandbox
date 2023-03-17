import hashlib

bUsername_trial = b"GOUGH"

key = ""

key += hashlib.sha256(bUsername_trial).hexdigest()[4]
key += hashlib.sha256(bUsername_trial).hexdigest()[5]
key += hashlib.sha256(bUsername_trial).hexdigest()[3]
key += hashlib.sha256(bUsername_trial).hexdigest()[6]
key += hashlib.sha256(bUsername_trial).hexdigest()[2]
key += hashlib.sha256(bUsername_trial).hexdigest()[7]
key += hashlib.sha256(bUsername_trial).hexdigest()[1]
key += hashlib.sha256(bUsername_trial).hexdigest()[8]

print(key)
