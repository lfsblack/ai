#! /usr/bin/env bash

TF_CFLAGS=( $(python -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_compile_flags()))') )
TF_LFLAGS=( $(python -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_link_flags()))') )
echo "TF_CFLAGS = " ${TF_CFLAGS[@]}
echo "TF_LFLAGS = " ${TF_LFLAGS[@]}

echo "compile fasttext_example_generate_ops.so ..."
g++ -std=c++11 -shared \
    fasttext_example_generate_ops.cc \
    fasttext_example_generate_kernels.cc \
    dictionary.cc \
    common.cc \
    -o fasttext_example_generate_ops.so \
    -fPIC ${TF_CFLAGS[@]} ${TF_LFLAGS[@]} -O2

echo "compile fasttext_negative_sampler_ops.so ..."
g++ -std=c++11 -shared \
    fasttext_negative_sampler_ops.cc \
    fasttext_negative_sampler_kernels.cc \
    -o fasttext_negative_sampler_ops.so \
    -fPIC ${TF_CFLAGS[@]} ${TF_LFLAGS[@]} -O2
