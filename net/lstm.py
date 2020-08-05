from typing import List, Tuple
from io import BufferedReader
from tensorflow.python.framework.ops import EagerTensor
import tensorflow as tf
import os
import json
import struct
import matplotlib.pyplot as plt
import warnings


MODEL_NAME: str = "second"
FILE_COUNT: int = 100

ROOT: str = os.path.join(".", "..")

MDL_DIR: str = os.path.join(ROOT, "def", "mdl")
DAT_DIR: str = os.path.join(ROOT, "dat", MODEL_NAME)
TRAIN_DAT_DIR: str = os.path.join(DAT_DIR, "train")
TEST_DAT_DIR: str = os.path.join(DAT_DIR, "test")
CKPT_DIR: str = os.path.join(ROOT, "net", "ckpt", MODEL_NAME)

MODEL_SETTINGS: dict = json.load(open(os.path.join(MDL_DIR, MODEL_NAME + ".json")))

PATIENCE = MODEL_SETTINGS["lstm_params"]["patience"]
OG_DIM: Tuple[int, int] = (MODEL_SETTINGS["grid_rows"], MODEL_SETTINGS["grid_cols"])


def parse_dat(directory: str, filename: str) -> List[List[List[float]]]:
    filepath: str = os.path.join(directory, filename + ".dat")

    file: BufferedReader
    with open(filepath, mode='rb') as file:
        byte_data: bytes = file.read()

    unsigned_long_bytes: int = 8
    unsigned_char_bytes: int = 1
    header_longs: int = 3

    bytes_idx: int = unsigned_long_bytes * header_longs

    metadata: Tuple[int, int, int] = struct.unpack("L" * header_longs, byte_data[:bytes_idx])
    bytes_idx += 1

    data: List[List[List[float]]] = [[[0.0 for col in range(metadata[2])]
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
            cell_val: float = 1.0 if (bool_pack >> bit_idx) % 2 == 1 else 0.0
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


def print_dat(directory: str, filename: str, print_borders: bool = True) -> None:
    data: List[List[List[bool]]] = parse_dat(directory, filename)

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

    # arch1
    # model: tf.keras.models.Sequential = tf.keras.models.Sequential([
    #     tf.keras.layers.Input(shape=og_dim),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True)
    # ])

    # arch2
    # model: tf.keras.models.Sequential = tf.keras.models.Sequential([
    #     tf.keras.layers.Input(shape=og_dim),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True)
    # ])

    # arch3
    dense_units: int = 64
    model: tf.keras.models.Sequential = tf.keras.models.Sequential([
        tf.keras.layers.Input(shape=og_dim),
        tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
        tf.keras.layers.Dense(dense_units),
        tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
        tf.keras.layers.Dense(dense_units),
        tf.keras.layers.LSTM(og_dim[1], return_sequences=True)
    ])

    return model


def print_frame(mat: List[List[float]]) -> None:
    print('{')

    row: List[float]
    for row in mat:
        print('\t', end='')
        val: float
        for val in row:
            print(f"{val:.3f}, ", end='')
        print()

    print('}')


def get_file_name(idx: int) -> str:
    file_name: str = f"{(idx + 1):04d}"
    return file_name


class DataGenerator(tf.keras.utils.Sequence):
    def __len__(self) -> int:
        return FILE_COUNT

    def __getitem__(self, idx: int) -> Tuple[EagerTensor, EagerTensor]:
        data: EagerTensor = tf.stack(parse_dat(TRAIN_DAT_DIR, get_file_name(idx)))
        return data[:-1], data[1:]


def train_model(model: tf.keras.models.Sequential) -> None:
    ckpt: str = tf.train.latest_checkpoint(CKPT_DIR)
    initial_epoch: int = 0

    if ckpt is None:
        print("No checkpoint loaded")
    else:
        model.load_weights(ckpt)
        print("Loaded checkpoint " + tf.train.latest_checkpoint(CKPT_DIR) + "\n")
        try:
            initial_epoch = int(ckpt.split('_')[-1])
        except ValueError:
            warnings.warn("WARNING: checkpoint name not in form of \"ckpt_[epoch]\" ==> epoch could not be inferred")

    model.compile(loss="binary_crossentropy")

    data_generator: DataGenerator = DataGenerator()
    early_stop = tf.keras.callbacks.EarlyStopping(monitor="loss", patience=PATIENCE)
    checkpoint_callback = tf.keras.callbacks.ModelCheckpoint(filepath=os.path.join(CKPT_DIR, "ckpt_{epoch}"),
                                                             save_weights_only=True)

    model.fit(
        x=data_generator,
        epochs=FILE_COUNT,
        initial_epoch=initial_epoch,
        shuffle=False,
        callbacks=[checkpoint_callback, early_stop]
    )


def main() -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)

    model.summary()

    train_model(model)


def test_model(test_idx: int, start_frame: int, end_frame: int) -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)

    ckpt: str = tf.train.latest_checkpoint(CKPT_DIR)

    if ckpt is None:
        print("No checkpoint loaded")
    else:
        model.load_weights(ckpt)
        print("Loaded checkpoint " + tf.train.latest_checkpoint(CKPT_DIR) + "\n")

    model.summary()

    data: EagerTensor = tf.stack(parse_dat(TEST_DAT_DIR, get_file_name(test_idx)))

    for i in range(min(len(data)-1, start_frame), min(len(data)-1, end_frame+1)):
        out: EagerTensor = model(data[0:i + 1])

        print(f"Frame {i + 1:d} losses")
        print('-' * 50)
        print("%10s: %+f" % ("BinCross", tf.math.reduce_mean(tf.keras.losses.binary_crossentropy(data[i+1], out[i]))))
        print("%10s: %+f" % ("MAE",      tf.math.reduce_mean(tf.keras.losses.MAE(                data[i+1], out[i]))))
        print("%10s: %+f" % ("MSE",      tf.math.reduce_mean(tf.keras.losses.MSE(                data[i+1], out[i]))))
        print("%10s: %+f" % ("MSLE",     tf.math.reduce_mean(tf.keras.losses.MSLE(               data[i+1], out[i]))))
        print("%10s: %+f" % ("CosSim",   tf.math.reduce_mean(tf.keras.losses.cosine_similarity(  data[i+1], out[i]))))
        print("%10s: %+f" % ("Hinge",    tf.math.reduce_mean(tf.keras.losses.hinge(              data[i+1], out[i]))))
        print("%10s: %+f" % ("Huber",    tf.math.reduce_mean(tf.keras.losses.huber(              data[i+1], out[i]))))
        print()

        ax = plt.subplots()[1]
        ax.set_title(f"Frame {i + 1:d} actual")
        ax.imshow(data[i])

        ax = plt.subplots()[1]
        ax.set_title(f"Frame {i + 1:d} dpo_pdf")
        ax.imshow(out[i])

    plt.show()


test_model(1, 8, 10)
