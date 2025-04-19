#!/usr/bin/env python3

import argparse
import subprocess
import sys

parser = argparse.ArgumentParser(description="Test different versions of the engine against each other")
parser.add_argument("-n", "--new-version", required=True, help="New version of the engine")
parser.add_argument("-o", "--old-version", required=True, help="Old version of the engine")
parser.add_argument("-b", "--opening-book", required=True, help="Opening book")
parser.add_argument("-f", "--output-file", required=False, help="Output file")


try:
    args = parser.parse_args()
except:
    parser.print_help()
    sys.exit(1)


command = f'''
    cutechess-cli \
    -engine cmd={args.new_version} -engine cmd={args.old_version} \
    -each proto=uci tc=inf/10+0.1 book={args.opening_book} bookdepth=4 \
    -rounds 2500 -games 2 -repeat 2 -maxmoves 200 \
    -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 \
    -concurrency 4 -ratinginterval 10 \
'''

if args.output_file:
    command += f' -pgnout {args.output_file}'

with subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True) as process:
    if process.stdout != None:
        for line in process.stdout:
            print(line, end='')
