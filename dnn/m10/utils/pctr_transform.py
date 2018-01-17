#! /usr/bin/env python
# -*-coding:utf-8 -*-


import argparse
import random
import numpy as np


# Basic model parameters as external flags.
FLAGS = None


def get_label_class(radio, class_num):
    # watch_duration_class
    if radio >= 1:
        # print("dirty data or protect")
        radio = 0.9
    y = int(np.floor(class_num * (1 / (1 + np.exp(6 - 10 * radio)))))
    if y < 0:
        y = 0
    return y


def records2binary_pctr(recordsfile,
                        watchedfile,
                        watched_size,
                        watched_ratio_file):
    fwatched = open(watchedfile, "w")
    fwatched_ratio = open(watched_ratio_file, "r")

    for lineindex, line in enumerate(open(recordsfile, "r")):
        watched_ratio_line = fwatched_ratio.readline()

        records = line.strip().split(' ')[1:]
        watched_ratios = watched_ratio_line.strip().split(' ')[1:]
        watched_ratios = map(float, watched_ratios)

        # generate binary records
        records_len = len(records)
        for w in xrange(1, records_len, FLAGS.pctr_step):
            boundary = random.randint(1, watched_size)
            score = watched_ratios[w]
            label = get_label_class(score, FLAGS.class_num_pctr)
            fwatched.write("__label__" + str(label) + ' ')
            # 第一个词是当前词
            for c in xrange(0, boundary + 1):
                if w-c >= 0:
                    fwatched.write(records[w-c] + ' ')
            fwatched.write('\n')
        if lineindex % 1000000 == 0:
            print("{} lines processed ...".format(lineindex))

    fwatched.close()


def main():
    records2binary_pctr(FLAGS.input_records,
                        FLAGS.output_watched_pctr,
                        FLAGS.watched_size_pctr,
                        FLAGS.input_watched_ratio_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--input_records',
        type=str,
        default='',
        help='records file generated by transform.py.'
    )

    parser.add_argument(
        '--output_watched_pctr',
        type=str,
        default='',
        help='Binary form of watched records pctr.'
    )

    parser.add_argument(
        '--input_watched_ratio_file',
        type=str,
        default='',
        help='Input watched ratio file for PCTR.'
    )

    parser.add_argument(
        '--watched_size_pctr',
        type=int,
        default=20,
        help='Watched size for pctr.'
    )

    parser.add_argument(
        '--max_per_user',
        type=int,
        default=5,
        help='Max number of watched windows selected per user.'
    )

    parser.add_argument(
        '--class_num_pctr',
        type=int,
        default=10,
        help=''
    )

    parser.add_argument(
        '--pctr_step',
        type=int,
        default=2,
        help=''
    )

    FLAGS, unparsed = parser.parse_known_args()
    main()