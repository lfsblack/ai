#! /usr/bin/env python
# -*- coding=utf8 -*-

PADDING_ID = 0


POSITIVE_RECORDS_COL = 'positive_records'
NEGATIVE_RECORDS_COL = 'negative_records'
TARGETS_COL = 'targets'
FIRST_VIDEOS_COL = 'first_videos'
TARGET_ROWKEYS_COL = 'target_rowkeys'
FIRST_VIDEO_ROWKEYS_COL = 'first_video_rowkeys'
IS_TARGET_IN_DICT_COL = 'is_target_in_dict'
NUM_POSITIVE_COL = 'num_positive'
NUM_NEGATIVE_COl = 'num_negative'


class TrainParallelMode(object):
    """Train parallen mode."""

    DEFAULT = 'default'
    MULTI_THREAD = 'multi_thread'


class OptimizerType(object):
    """Optimizer type."""

    ADAGRAD = 'adagrad'
    ADADELTA = 'adadelta'
    ADAM = 'adam'
    SGD = 'sgd'
    RMSPROP = 'rmsprop'
    MOMENTUM = 'momentum'
    FTRL = 'ftrl'


class LossType(object):
    """Loss type."""

    CE = 'ce'
    MSE = 'mse'
