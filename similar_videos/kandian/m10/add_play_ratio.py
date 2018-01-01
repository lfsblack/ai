#! /usr/bin/env python
# -*- coding=utf8 -*-


import argparse


# Basic model parameters as external flags.
FLAGS = None


def main():
    play_ratios = dict()
    for index, line in enumerate(open(FLAGS.input_play_ratio_file)):
        tokens = line.strip().split(' ')
        if len(tokens) < 2:
            print("[ratio file] line in {}: {}".format(index, line))
            continue
        play_ratios[tokens[0]] = tokens[1]

    with open(FLAGS.output_result_file, 'w') as fout:
        for index, line in enumerate(open(FLAGS.input_nn_result_file)):
            tokens = line.strip().split(' ')
            if len(tokens) < 2:
                print("[nn result file] line in {}: {}".format(index, line))
                continue
            rowkey = tokens[0]
            score = tokens[1]
            if rowkey not in play_ratios:
                print("[W] rowkey not in ratio file: {}".format(rowkey))
                ratio = '0.0'
            else:
                ratio = play_ratios[rowkey]

            mean = score * FLAGS.nn_score_weight + \
                ratio * (1 - FLAGS.nn_score_weight)
            fout.write(rowkey + ' ' + str(mean))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--input_play_ratio_file',
        type=str,
        default='',
        help=''
    )
    parser.add_argument(
        '--input_nn_result_file',
        type=str,
        default='',
        help=''
    )

    parser.add_argument(
        '--output_result_file',
        type=str,
        default='',
        help=''
    )

    parser.add_argument(
        '--nn_k',
        type=int,
        default=0,
        help=''
    )

    parser.add_argument(
        '--nn_score_weight',
        type=float,
        default=0.5,
        help=''
    )

    FLAGS, unparsed = parser.parse_known_args()
    main()
