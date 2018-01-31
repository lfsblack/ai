
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


import os
import sys
import threading
import time

from six.moves import xrange  # pylint: disable=redefined-builtin

import numpy as np
import tensorflow as tf

fasttext_model = tf.load_op_library(os.path.join(
    os.path.dirname(os.path.realpath(__file__)),
    'fasttext_ops.so'))

flags = tf.app.flags
flags.DEFINE_string("train_data", None, "train data file")

FLAGS = flags.FLAGS


def main(_):
    """Train a fasttext model."""
    fasttext = fasttext_model.fasttext(train_data=FLAGS.train_data)

    with tf.Session() as sess:
        sess.run(fasttext)


if __name__ == "__main__":
    tf.app.run()
