from typing import List, Tuple
from io import BufferedReader
from tensorflow.python.framework.ops import EagerTensor
import tensorflow as tf
import numpy as np
import os
import json
import struct

ROOT: str = os.path.join(".", "..")
MODEL_NAME: str = "first"
MDL_DIR: str = os.path.join(ROOT, "def", "mdl")
DAT_DIR: str = os.path.join(ROOT, "dat", MODEL_NAME)
CKPT_DIR: str = os.path.join(ROOT, "net", "ckpt")

MODEL_SETTINGS: dict = json.load(open(os.path.join(MDL_DIR, MODEL_NAME + ".json")))

OG_DIM: Tuple[int, int] = (MODEL_SETTINGS["grid_rows"], MODEL_SETTINGS["grid_cols"])


def parse_dat(filename: str) -> List[List[List[bool]]]:
    filepath: str = os.path.join(DAT_DIR, filename + ".dat")

    file: BufferedReader
    with open(filepath, mode='rb') as file:
        byte_data: bytes = file.read()

    unsigned_long_bytes: int = 8
    unsigned_char_bytes: int = 1
    header_longs: int = 3

    bytes_idx: int = unsigned_long_bytes * header_longs

    metadata: Tuple[int, int, int] = struct.unpack("L" * header_longs, byte_data[:bytes_idx])
    bytes_idx += 1

    data: List[List[List[bool]]] = [[[False for col in range(metadata[2])]
                                     for row in range(metadata[1])]
                                    for timestep in range(metadata[0])]

    timestep_idx: int = 0
    row_idx: int = 0
    col_idx: int = 0

    for byte in range(header_longs, len(byte_data)):
        if timestep_idx == metadata[0]:
            break

        bool_pack: int = struct.unpack("B", byte_data[bytes_idx:(bytes_idx + unsigned_char_bytes)])[0]
        bytes_idx += unsigned_char_bytes

        for bit_idx in range(0, 8):
            cell_val: bool = (bool_pack >> bit_idx) % 2 == 1
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


def print_dat(filename: str, print_borders: bool = True) -> None:
    data: List[List[List[bool]]] = parse_dat(filename)

    timestep: List[List[bool]]
    for timestep in data:
        if print_borders:
            for i in range(0, len(data[0][0]) + 2):
                print('-', end='')

            print()

        row: List[bool]
        for row in timestep:
            if print_borders:
                print('|', end='')

            cell: bool
            for cell in row:
                print('*' if cell else ' ', end='')

            if print_borders:
                print('|', end='')

            print()

        if print_borders:
            for i in range(0, len(data[0][0]) + 2):
                print('-', end='')

            print()


def build_model(og_dim: Tuple[int, int]) -> tf.keras.models.Sequential:
    # use stateful if 1 batch != one 1 .dat

    model: tf.keras.models.Sequential = tf.keras.models.Sequential([
        tf.keras.layers.Input(shape=og_dim),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.LSTM(og_dim[1], return_sequences=True)
    ])

    # model = tf.keras.models.Sequential([
    #     tf.keras.layers.Input(shape=input_dim),
    #     tf.keras.layers.Dropout(0.2),
    #     tf.keras.layers.LSTM(input_dim[1], return_sequences=True),
    #     tf.keras.layers.Dropout(0.2),
    #     tf.keras.layers.Dense(input_dim[1])
    # ])

    return model


def print_confidence_frame(mat: List[List[float]]) -> None:
    print('{')

    row: List[float]
    for row in mat:
        print('\t', end='')
        val: float
        for val in row:
            print("{:.3f}, ".format(val), end='')
        print()

    print('}')


def main() -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)
    print(model.summary())

    data: EagerTensor = tf.stack(parse_dat("001"))
    out: EagerTensor = model(data)

    print(out[0])


def train_model() -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)

    ckpt: str = tf.train.latest_checkpoint(CKPT_DIR)
    if ckpt is None:
        print("No checkpoint loaded")
    else:
        model.load_weights(ckpt)
        print("Loaded checkpoint " + tf.train.latest_checkpoint(CKPT_DIR) + "\n")

    model.summary()
    model.compile(loss="binary_crossentropy")

    data: EagerTensor = tf.stack(parse_dat("001"))

    model.fit(data[0:len(data)-1], data[1:], batch_size=len(data)-1, shuffle=False)


train_model()
