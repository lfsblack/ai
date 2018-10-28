#! /usr/bin/env python
# -*- coding=utf8 -*-

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import tensorflow as tf
import numpy as np

import model_keys

VGG_MEAN = [123.68, 116.779, 103.939]  # RGB


# code modified from:
# https://gist.github.com/omoindrot/dedc857cdc0e680dfb1be99762990c9c

# Standard preprocessing for VGG on ImageNet taken from here:
# https://github.com/tensorflow/models/blob/master/research/slim/preprocessing/vgg_preprocessing.py
# Also see the VGG paper for more details: https://arxiv.org/pdf/1409.1556.pdf


# Preprocessing (for both training and validation):
# (1) Decode the image from jpg/png/bmp format
# (2) Resize the image so its smaller side is 256 pixels long
def parse_function(img_path, label, opts):
    image_string = tf.read_file(img_path)
    # image_decoded = tf.image.decode_jpeg(image_string, channels=3)
    image_decoded = tf.image.decode_image(image_string, channels=3)
    image_decoded.set_shape([None, None, 3])
    image = tf.cast(image_decoded, tf.float32)

    smallest_side = 256.0
    height, width = tf.shape(image)[0], tf.shape(image)[1]
    height = tf.to_float(height)
    width = tf.to_float(width)

    scale = tf.cond(tf.greater(height, width),
                    lambda: smallest_side / width,
                    lambda: smallest_side / height)
    new_height = tf.to_int32(height * scale)
    new_width = tf.to_int32(width * scale)

    resized_image = tf.image.resize_images(image, [new_height, new_width])
    return resized_image, label


# Preprocessing (for training)
# (3) Take a random 224x224 crop to the scaled image
# (4) Horizontally flip the image with probability 1/2
# (5) Substract the per color mean `VGG_MEAN`
# Note: we don't normalize the data here, as VGG was trained without
# normalization
# Note(zhezhaoxu): we add rgb to bgr transform
def train_preprocess(image, label, opts):
    crop_image = tf.random_crop(image, [224, 224, 3])           # (3)
    flip_image = tf.image.random_flip_left_right(crop_image)    # (4)
    means = tf.reshape(tf.constant(VGG_MEAN), [1, 1, 3])
    centered_image = flip_image - means                         # (5)
    bgr_image = centered_image[:, :, ::-1]  # RGB -> BGR

    label = tf.one_hot(label, opts.num_classes)
    return {model_keys.DATA_COL: bgr_image}, label


# Preprocessing (for validation)
# (3) Take a central 224x224 crop to the scaled image
# (4) Substract the per color mean `VGG_MEAN`
# Note: we don't normalize the data here, as VGG was trained without
# normalization
# Note(zhezhaoxu): we add multi scale image predict and rgb to bgr transform
def val_preprocess(image, label, opts):
    if opts.multi_scale_predict:
        if opts.inference_shape is not None:
            crop_image = tf.image.resize_image_with_crop_or_pad(
                image, opts.inference_shape[0], opts.inference_shape[1])
        else:
            crop_image = image  # do not crop image
    else:
        crop_image = tf.image.resize_image_with_crop_or_pad(image, 224, 224)

    means = tf.reshape(tf.constant(VGG_MEAN), [1, 1, 3])
    centered_image = crop_image - means
    bgr_image = centered_image[:, :, ::-1]  # RGB -> BGR

    if label is not None:
        label = tf.one_hot(label, opts.num_classes)
        return {model_keys.DATA_COL: bgr_image}, label
    else:
        return {model_keys.DATA_COL: bgr_image}  # for predict


def read_txt_file(txt_file, has_label=True):
    """Read the content of the text file and store it into lists."""

    img_paths = []
    labels = []
    with open(txt_file, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            items = line.split(' ')
            img_paths.append(items[0])
            if has_label:
                labels.append(int(items[1]))

    if has_label:
        return img_paths, labels
    else:
        return img_paths


def shuffle_lists(img_paths, labels):
    """Conjoined shuffling of the list of paths and labels."""

    path = img_paths
    labels = labels
    permutation = np.random.permutation(len(labels))
    shuffle_img_paths = []
    shuffle_labels = []
    for i in permutation:
        shuffle_img_paths.append(path[i])
        shuffle_labels.append(labels[i])

    return shuffle_img_paths, shuffle_labels


def train_input_fn(opts):
    img_paths, img_labels = read_txt_file(opts.train_data_path, has_label=True)
    img_paths, img_labels = shuffle_lists(img_paths, img_labels)
    img_paths = tf.convert_to_tensor(img_paths, dtype=tf.string)
    labels = tf.convert_to_tensor(img_labels, dtype=tf.int32)

    ds = tf.data.Dataset.from_tensor_slices((img_paths, labels))
    num_parallel = opts.map_num_parallel_calls
    ds = ds.map(lambda filename, label: parse_function(filename, label, opts),
                num_parallel_calls=num_parallel)
    ds = ds.map(lambda image, label: train_preprocess(image, label, opts),
                num_parallel_calls=num_parallel)
    ds = ds.prefetch(opts.prefetch_size)
    if opts.shuffle_batch:
        ds = ds.shuffle(buffer_size=opts.shuffle_size)
    ds = ds.batch(opts.batch_size).repeat(opts.epoch)

    return ds


def eval_input_fn(opts):
    img_paths, img_labels = read_txt_file(opts.eval_data_path, has_label=True)
    img_paths = tf.convert_to_tensor(img_paths, dtype=tf.string)
    labels = tf.convert_to_tensor(img_labels, dtype=tf.int32)

    ds = tf.data.Dataset.from_tensor_slices((img_paths, labels))
    num_parallel = opts.map_num_parallel_calls
    ds = ds.map(lambda filename, label: parse_function(filename, label, opts),
                num_parallel_calls=num_parallel)
    ds = ds.map(lambda image, label: val_preprocess(image, label, opts),
                num_parallel_calls=num_parallel)

    ds = ds.prefetch(opts.prefetch_size)
    if opts.multi_scale_predict and opts.inference_shape is None:
        ds = ds.batch(1)
    else:
        ds = ds.batch(opts.batch_size)
    return ds


def predict_input_fn(opts):
    img_paths = read_txt_file(opts.predict_data_path, has_label=False)
    img_paths = tf.convert_to_tensor(img_paths, dtype=tf.string)

    ds = tf.data.Dataset.from_tensor_slices((img_paths))
    num_parallel = opts.map_num_parallel_calls
    ds = ds.map(lambda filename: parse_function(filename, None, opts),
                num_parallel_calls=num_parallel)
    ds = ds.map(lambda filename, label: val_preprocess(filename, label, opts),
                num_parallel_calls=num_parallel)
    ds = ds.prefetch(opts.prefetch_size)
    if opts.multi_scale_predict:
        ds = ds.batch(1)
    else:
        ds = ds.batch(opts.batch_size)
    return ds


def build_serving_input_fn(opts):
    def serving_input_receiver_fn():
        feature_spec = {
            model_keys.DATA_COL: tf.FixedLenFeature(shape=[224, 224, 3],
                                                    dtype=tf.float32)
        }

        serialized_tf_example = tf.placeholder(dtype=tf.string,
                                               shape=[None],
                                               name='input_example_tensor')

        receiver_tensors = {'examples': serialized_tf_example}
        features = tf.parse_example(serialized_tf_example, feature_spec)

        return tf.estimator.export.ServingInputReceiver(features,
                                                        receiver_tensors)

    return serving_input_receiver_fn