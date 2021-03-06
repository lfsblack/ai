#! /usr/bin/env python
# -*- coding=utf8 -*-

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


import tensorflow as tf

import input_data
import char_rnn_model


def train(opts):
    """Train model."""

    tf.logging.info("Preprocessing ...")
    input_data.preprocess(opts.train_data_path, opts.preprocessed_filename,
                          opts.min_count)
    tf.logging.info("Preprocessing done.")

    input_fn = input_data.build_train_input_fn_v2(opts, opts.train_data_path)
    model = create_char_rnn_model(opts)
    model.train(input_fn)


def sample(opts):
    model = create_char_rnn_model(opts)
    model.sample()


def create_char_rnn_model(opts):
    num_samples_per_epoch, vocab = get_preprocessed(opts)
    vocab_size = len(vocab)
    params = {'vocab_size': vocab_size,
              'num_samples_per_epoch': num_samples_per_epoch}
    model = char_rnn_model.CharRNN(opts, params)
    return model


def get_preprocessed(opts):
    load_dict = input_data.load_preprocessed(opts.preprocessed_filename)
    total_chars = load_dict['total_chars']
    vocab = load_dict['vocab']
    num_samples_per_epoch = int(total_chars / opts.seq_length)
    return num_samples_per_epoch, vocab
