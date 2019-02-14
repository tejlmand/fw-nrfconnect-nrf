#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


import hashlib
import sys
import argparse
from intelhex import IntelHex


def parse_args():
    parser = argparse.ArgumentParser(
        description="Hash data from stdin or file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("--infile", "-i", "--in", "-in", required=True,
                        help="Hash the contents of the specified file instead of stdin.")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    if args.infile.endswith('.hex'):
        ih = IntelHex()
        ih.loadhex(args.infile)
        to_hash = ih.tobinstr()
    else:
        to_hash = open(args.infile, 'rb').read()
    sys.stdout.buffer.write(hashlib.sha256(to_hash).digest())
