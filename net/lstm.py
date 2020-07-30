import tensorflow as tf
import os
import json
import struct

ROOT = os.path.join(".", "..");
MODEL_NAME = "first"
MDL_DIR = os.path.join(ROOT, "def", "mdl")
DAT_DIR = os.path.join(ROOT, "dat", MODEL_NAME)

MODEL_SETTINGS = json.load(open(os.path.join(MDL_DIR, MODEL_NAME + ".json")))

OG_DIM = (MODEL_SETTINGS["grid_rows"], MODEL_SETTINGS["grid_cols"])


def print_tf_version():
    print(tf.__version__)


def parse_dat(filename):
    filepath = os.path.join(DAT_DIR, filename + ".dat")

    with open(filepath, mode='rb') as file:
        byte_data = file.read()

    unsigned_long_bytes = 8
    unsigned_char_bytes = 1
    header_longs = 3

    bytes_idx = unsigned_long_bytes * header_longs

    metadata = struct.unpack("L" * header_longs, byte_data[:bytes_idx])
    bytes_idx += 1

    data = [[[False for col in range(metadata[2])] for row in range(metadata[1])] for timestep in range(metadata[0])]

    timestep_idx = 0
    row_idx = 0
    col_idx = 0

    for byte in range(header_longs, len(byte_data)):
        if timestep_idx == metadata[0]:
            break

        bool_pack = struct.unpack("B", byte_data[bytes_idx:(bytes_idx + unsigned_char_bytes)])[0]
        bytes_idx += unsigned_char_bytes

        for bit_idx in range(0, 8):
            cell_val = (bool_pack >> bit_idx) % 2 == 1
            data[timestep_idx][row_idx][col_idx] = cell_val

            col_idx += 1
            if col_idx == metadata[2]:
                col_idx = 0
                row_idx += 1
                if row_idx == metadata[1]:
                    row_idx = 0
                    timestep_idx += 1
                    if timestep_idx == metadata[0]:
                        break

    return data


def print_dat(filename, print_borders=True):
    data = parse_dat(filename)
    for timestep in data:
        if print_borders:
            for i in range(0, len(data[0][0]) + 2):
                print('-', end='')
            print()
        for row in timestep:
            if print_borders:
                print('|', end='')
            for cell in row:
                print('*' if cell else ' ', end='')
            if print_borders:
                print('|', end='')
            print()
        if print_borders:
            for i in range(0, len(data[0][0]) + 2):
                print('-', end='')
            print()


def main():
    print(OG_DIM)
    # parse_dat("001")
    print_dat("001")


main()
