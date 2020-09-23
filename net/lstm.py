from typing import List, Tuple
from io import BufferedReader
from tensorflow.python.framework.ops import EagerTensor, Tensor
import tensorflow as tf
import numpy as np
from numpy import ndarray
import os
import json
import struct
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import warnings

MODEL_NAME: str = "second"
FILE_COUNT: int = 1000

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

    # arch4
    # dense_units: int = 128
    # model: tf.keras.models.Sequential = tf.keras.models.Sequential([
    #     tf.keras.layers.Input(shape=og_dim),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
    #     tf.keras.layers.Dense(dense_units),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
    #     tf.keras.layers.Dense(dense_units),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True),
    #     tf.keras.layers.Dense(dense_units),
    #     tf.keras.layers.LSTM(og_dim[1], return_sequences=True)
    # ])

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


def on_off_equal_loss(y_true: Tensor, y_pred: Tensor) -> Tensor:
    on_delta: float = 0.99

    on_mask: Tensor = tf.greater_equal(y_true, on_delta)
    off_mask: Tensor = tf.less(y_true, on_delta)

    on_loss: Tensor = tf.divide(tf.reduce_sum(tf.abs(tf.subtract(
        y_true[on_mask], y_pred[on_mask]
    ))), tf.cast(tf.math.count_nonzero(on_mask), tf.float32))

    off_loss: Tensor = tf.divide(tf.reduce_sum(tf.abs(tf.subtract(
        y_true[off_mask], y_pred[off_mask]
    ))), tf.cast(tf.math.count_nonzero(off_mask), tf.float32))

    # on_factor: float = 4.0
    # return tf.divide(tf.add(tf.multiply(on_factor, on_loss), off_loss), on_factor + 1.0)
    return tf.divide(tf.add(on_loss, off_loss), 2.0)


def tensor_harmonic(x: Tensor) -> Tensor:
    # courtesy of jdehesa (https://stackoverflow.com/questions/63457496/tensorflow-2-x-tensor-with-average-of-surrounding-cells/63466897)
    # x = tf.convert_to_tensor(x)
    dt = x.dtype
    # Compute surround sum
    filter1 = tf.constant([[1, 1, 1], [1, 0, 1], [1, 1, 1]], dtype=dt)
    x2 = x[tf.newaxis, :, :, tf.newaxis]
    filter2 = filter1[:, :, tf.newaxis, tf.newaxis]
    y2 = tf.nn.conv2d(x2, filter2, strides=1, padding='SAME')
    y = y2[0, :, :, 0]
    # Make matrix of number of surrounding elements
    s = tf.shape(x)
    d = tf.fill(s - 2, tf.constant(8, dtype=dt))
    d = tf.pad(d, [[0, 0], [1, 1]], constant_values=5)
    top_row = tf.concat([[3], tf.fill([s[1] - 2], tf.constant(5, dtype=dt)), [3]], axis=0)
    d = tf.concat([[top_row], d, [top_row]], axis=0)
    # Return average
    return y / d


def gradient_encouraging_loss(y_true: Tensor, y_pred: Tensor) -> Tensor:
    gradient_loss: Tensor = tf.divide(tf.reduce_sum(tf.abs(tf.subtract(
        y_pred, tensor_harmonic(y_pred)
    ))), tf.cast(tf.size(y_pred), tf.float32))

    return tf.divide(tf.add(gradient_loss, tf.multiply(on_off_equal_loss(y_true, y_pred), 2.0)), 3.0)


def get_file_name(idx: int) -> str:
    file_name: str = f"{(idx + 1):04d}"
    return file_name


class DataGenerator(tf.keras.utils.Sequence):
    def __len__(self) -> int:
        return FILE_COUNT

    def __getitem__(self, idx: int) -> Tuple[Tensor, Tensor]:
        data: Tensor = tf.stack(parse_dat(TRAIN_DAT_DIR, get_file_name(idx)))
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

    model.compile(loss=on_off_equal_loss, run_eagerly=True)

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

    data: Tensor = tf.stack(parse_dat(TEST_DAT_DIR, get_file_name(test_idx)))

    white_to_orange: LinearSegmentedColormap = LinearSegmentedColormap.from_list("my_cmap",
                                                                                 ["black", "white", "darkorange"])

    for i in range(min(len(data) - 1, start_frame), min(len(data) - 1, end_frame + 1)):
        out: Tensor = model(data[0:i + 1])

        print(f"Frame {i + 1:d} losses")
        print('-' * 50)
        # ("%10s: %+f" % ("OnOffBal", on_off_balance_loss(data[i + 1], out[i])))
        print("%10s: %+f" % ("OnOffEq", float(on_off_equal_loss(data[i + 1], out[i]))))
        print("%10s: %+f" % ("BinCross", tf.math.reduce_mean(tf.keras.losses.binary_crossentropy(data[i + 1], out[i]))))
        print("%10s: %+f" % ("MAE", tf.math.reduce_mean(tf.keras.losses.MAE(data[i + 1], out[i]))))
        print("%10s: %+f" % ("MSE", tf.math.reduce_mean(tf.keras.losses.MSE(data[i + 1], out[i]))))
        print("%10s: %+f" % ("MSLE", tf.math.reduce_mean(tf.keras.losses.MSLE(data[i + 1], out[i]))))
        print("%10s: %+f" % ("CosSim", tf.math.reduce_mean(tf.keras.losses.cosine_similarity(data[i + 1], out[i]))))
        print("%10s: %+f" % ("Hinge", tf.math.reduce_mean(tf.keras.losses.hinge(data[i + 1], out[i]))))
        print("%10s: %+f" % ("Huber", tf.math.reduce_mean(tf.keras.losses.huber(data[i + 1], out[i]))))
        print()

        ax = plt.subplots()[1]
        ax.set_title(f"Frame {i + 1:d} actual")
        ax.imshow(data[i], vmin=-1, vmax=1, cmap=white_to_orange)
        ax.invert_yaxis()

        ax = plt.subplots()[1]
        ax.set_title(f"Frame {i + 1:d} dpo_pdf")
        ax.imshow(out[i], vmin=-1, vmax=1, cmap=white_to_orange)
        ax.invert_yaxis()

    plt.show()


def self_propagate(test_idx: int, start_frame: int, num_gen: int) -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)

    ckpt: str = tf.train.latest_checkpoint(CKPT_DIR)

    if ckpt is None:
        print("No checkpoint loaded")
    else:
        model.load_weights(ckpt)
        print("Loaded checkpoint " + tf.train.latest_checkpoint(CKPT_DIR) + "\n")

    model.summary()

    data: Tensor = tf.stack(parse_dat(TEST_DAT_DIR, get_file_name(test_idx)))

    white_to_orange: LinearSegmentedColormap = LinearSegmentedColormap.from_list("my_cmap",
                                                                                 ["black", "white", "darkorange"])

    data = tf.cast(data[0:start_frame], tf.float32)

    ax = plt.subplots()[1]
    ax.set_title("n = 0")
    ax.imshow([x * -2 for x in data[-1]], vmin=-1, vmax=1, cmap=white_to_orange)
    ax.invert_yaxis()

    for i in range(num_gen):
        out: Tensor = model(data)

        ax = plt.subplots()[1]
        ax.set_title(f"n = {i + 1:d}")
        ax.imshow(out[-1], vmin=-1, vmax=1, cmap=white_to_orange)
        ax.invert_yaxis()

        out = tf.expand_dims(out[-1], axis=0)
        data = tf.concat([data, out], 0)

    plt.show()


def self_propagate_with_test(test_idx: int, start_frame: int, num_gen: int, trail_length: int) -> None:
    model: tf.keras.models.Sequential = build_model(OG_DIM)

    ckpt: str = tf.train.latest_checkpoint(CKPT_DIR)

    if ckpt is None:
        print("No checkpoint loaded")
    else:
        model.load_weights(ckpt)
        print("Loaded checkpoint " + tf.train.latest_checkpoint(CKPT_DIR) + "\n")

    model.summary()

    data: Tensor = tf.stack(parse_dat(TEST_DAT_DIR, get_file_name(test_idx)))

    white_to_orange: LinearSegmentedColormap = LinearSegmentedColormap.from_list("my_cmap",
                                                                                 ["black", "white", "darkorange"])

    data = tf.cast(data, tf.float32)
    known_data: Tensor = data[0:start_frame]

    on_delta: float = 0.99

    ax = plt.subplots()[1]
    ax.set_title("n = 0")
    first_show: ndarray = known_data[-1].numpy()
    for j in range(trail_length - 1, -1, -1):
        first_show[tf.greater_equal(data[start_frame - 1 - j], on_delta)] = (j - trail_length) / float(trail_length)
    ax.imshow(first_show, vmin=-1, vmax=1, cmap=white_to_orange)
    ax.invert_yaxis()

    for i in range(start_frame, start_frame + num_gen):
        out: Tensor = model(known_data)

        ax = plt.subplots()[1]
        ax.set_title(f"n = {i + 1 - start_frame:d} (probability distribution vs. actual paths)")

        on_mask: Tensor = tf.greater_equal(data[i], on_delta)

        to_show: ndarray = out[-1].numpy()

        for j in range(trail_length - 1, -1, -1):
            to_show[tf.greater_equal(data[i - j], on_delta)] = (j - trail_length) / float(trail_length)

        ax.imshow(to_show, vmin=-1, vmax=1, cmap=white_to_orange)
        ax.invert_yaxis()

        print(f"n = {i + 1 - start_frame:d} losses")
        print('-' * 50)
        print("%10s: %+f" % ("OnOffEq", float(on_off_equal_loss(data[i], out[-1]))))
        print(
            "%10s: %+f" % ("BinCross", tf.math.reduce_mean(tf.keras.losses.binary_crossentropy(data[i + 1], out[-1]))))
        print("%10s: %+f" % ("MAE", tf.math.reduce_mean(tf.keras.losses.MAE(data[i], out[-1]))))
        print("%10s: %+f" % ("MSE", tf.math.reduce_mean(tf.keras.losses.MSE(data[i], out[-1]))))
        print("%10s: %+f" % ("Gradient", float(gradient_encouraging_loss(data[i], out[-1]))))

        out = tf.expand_dims(out[-1], axis=0)
        known_data = tf.concat([data, out], 0)

    plt.show()


self_propagate_with_test(test_idx=9, start_frame=5, num_gen=10, trail_length=5)
