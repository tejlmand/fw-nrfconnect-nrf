#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from intelhex import IntelHex

import re
import argparse
import struct
from ecdsa import SigningKey
from ecdsa import VerifyingKey
from hashlib import sha256

VERBOSE = False


def verbose_print(printstr):
    if VERBOSE:
        print(printstr)


def generate_provision_hex_file(s0_address, s1_address, hashes, provision_address, output):
    # Add addresses
    provision_data = struct.pack('III', s0_address, s1_address, len(hashes))
    provision_data += b''.join(hashes)

    ih = IntelHex()
    ih.frombytes(provision_data, offset=provision_address)
    ih.write_hex_file(output)


# Since cmake does not have access to DTS variables, fetch them manually.
def find_provision_memory_section(config_files):
    adr = dict()
    for lf in config_files:
        for line in lf.readlines():
            match = re.match('^#define CONFIG_SB_(\w*)_OFFSET 0x([0-9a-fA-F]*)', line)
            if match:
                adr[match.group(1).lower()] = int(match.group(2), 16)

    if 's0' not in adr.keys() or 's1' not in adr.keys() or 'provision' not in adr.keys():
        raise RuntimeError("Could not find value for one of S0, S1 or provision address.")

    return adr['s0'], adr['s1'], adr['provision']


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate provision hex file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("--generated-conf-files", type=argparse.FileType('r', encoding='UTF-8'), nargs="+",
                        help="Space separated list of conf files.", required=True)
    parser.add_argument("--public-key-files", required=True,
                        help="Semicolon-separated list of public key .pem files.")
    parser.add_argument("-o", "--output", required=False, default="provision.hex",
                        help="Output file name.")
    parser.add_argument("-v", "--verbose", required=False, action="count",
                        help="Verbose mode.")
    return parser.parse_args()


def get_hashes(public_key_files):
    hashes = list()
    for fn in public_key_files:
        verbose_print("Getting hash of %s" % fn)
        with open(fn, 'rb') as f:
            hashes.append(sha256(VerifyingKey.from_pem(f.read()).to_string()).digest()[:16])
        verbose_print("hash: " + hashes[-1].hex())
    return hashes


def main():
    args = parse_args()

    global VERBOSE
    VERBOSE = args.verbose

    s0_address, s1_address, provision_address = find_provision_memory_section(args.generated_conf_files)
    hashes = get_hashes(args.public_key_files.split(','))
    generate_provision_hex_file(s0_address=s0_address,
                                s1_address=s1_address,
                                hashes=hashes,
                                provision_address=provision_address,
                                output=args.output)


if __name__ == "__main__":
    main()
